#include "common.h"


/*
 * performance variables
 */
uint32_t perf_tm, *perf_cur;

/**
 * Read the time source clock.
 *   &returns: The TSC.
 */
static inline uint32_t perf_tsc(void)
{
	uint64_t rax, rdx;

	asm volatile("rdtscp" : "=a"(rax), "=d"(rdx) : : "%ecx" );

	return rax;
}

/**
 * Mark performance time.
 *   @mark: Ref. The mark. Should be reset to start.
 */
static inline void perf_mark(uint32_t *mark)
{
#if 1
	uint32_t tsc = perf_tsc();

	if(mark != NULL)
		*mark += tsc - perf_tm;

	perf_tm = tsc;
#endif
}


/*
 * encoding definitions
 */
#define POOL_SIZE (64*1024)
#define BUFLEN 512


/**
 * Encode structure.
 *   @nbytes: The number of bytes encoded.
 *   @block, tail: The block list and tail reference.
 *   @rd, wr: The read write indices.
 *   @buf: Buffer.
 */
struct acw_encode_t {
	size_t nbytes;
	struct acw_block_t *block, **tail;

	unsigned int rd, wr;
	int buf[BUFLEN];
};

/**
 * Trial structure.
 *   @off, len: The offset and length.
 *   @init, add: The initial and addend.
 */
struct acw_trial_t {
	unsigned int off, len;
	int init, add;
};

/**
 * Block structure.
 *   @bits, len: The number of bits and length.
 *   @init, add: Initial and add values.
 *   @nbytes: The number of bytes.
 *   @raw: The raw data.
 *   @next: The next block.
 */
struct acw_block_t {
	unsigned int bits, len;
	int init, add;

	unsigned int nbytes;
	void *raw;

	struct acw_block_t *next;
};
static inline struct acw_block_t acw_block(unsigned int bits, unsigned int len, int init, int add)
{
	return (struct acw_block_t){ bits, len, init, add };
}


/*
 * local declarations
 */
void enc_flush(struct acw_encode_t *enc);
struct acw_block_t *enc_block(struct acw_encode_t *enc, unsigned int bits, struct acw_trial_t trial);


/**
 * Create an encoder.
 *   &returns: The encoder.
 */
struct acw_encode_t *acw_encode_new(void)
{
	struct acw_encode_t *enc;

	enc = malloc(sizeof(struct acw_encode_t));
	enc->nbytes = 0;
	enc->tail = &enc->block;
	enc->rd = enc->wr = 0;

	return enc;
}

/**
 * Process a value on the encoder.
 *   @enc: The encoder.
 *   @val: The value.
 */
void acw_encode_proc(struct acw_encode_t *enc, int *arr, unsigned int len)
{
	while(len-- > 0) {
		enc->buf[enc->wr] = *arr++;
		enc->wr = (enc->wr + 1) % BUFLEN;
		if(enc->wr == enc->rd)
			enc_flush(enc);
	}
}

/**
 * Finish encoding.
 *   @enc: The enocder.
 *   &returns: The buffer.
 */
struct acw_buf_t acw_encode_done(struct acw_encode_t *enc)
{
	struct acw_block_t *block;

	while(enc->rd != enc->wr)
		enc_flush(enc);

	void *ptr;
	struct acw_buf_t buf;

	buf.raw = ptr = malloc(enc->nbytes += 2);
	buf.info.nblocks = 0;
	buf.info.length = 0;
	buf.info.nbytes = enc->nbytes;
	buf.info.magic = ACW_MAGIC_1_0;

	while(enc->block != NULL) {
		block = enc->block;
		enc->block = block->next;

		buf.info.nblocks++;
		buf.info.length += block->len;

		memcpy(ptr, block->raw, block->nbytes);
		ptr += block->nbytes;

		free(block->raw);
		free(block);
	}

	*(uint16_t *)ptr = ACW_24BIT << 12;
	ptr += 2;

	assert((ptr - buf.raw) == enc->nbytes);

	free(enc);

	return buf;
}


/**
 * Run a trial for encoding at a given bit width and offset.
 *   @enc: The encoder.
 *   @bits: The number of bits.
 *   @off: The offset.
 *   &returns: The trial.
 */
struct acw_trial_t enc_trial(struct acw_encode_t *enc, unsigned int bits, unsigned int off)
{
	int val, init, addlo = ACW_ADD_MIN, addhi = ACW_ADD_MAX, lo, hi;
	unsigned int rd;
	int idx = 1;

	int max = 1 << (bits - 1);
	int min = -max + 1;

	rd = (enc->rd + off) % BUFLEN;
	init = enc->buf[rd];

	for(idx = 1; idx < 256; idx++) {
		rd = (rd + 1) % BUFLEN;
		if(rd == enc->wr)
			break;

		val = enc->buf[rd];
		lo = m_max_i(m_div_p(val + min - init, idx), addlo);
		hi = m_min_i(m_div_n(val + max - init, idx), addhi);
		if(lo > hi)
			break;

		addlo = lo;
		addhi = hi;
	}

	int add = (addlo + addhi) / 2;
	val = init;

	for(int i = 1; i < idx; i++) {
		rd = (enc->rd + off + i) % BUFLEN;
		val += add;

		int diff = val - enc->buf[rd];
		if(diff > max)
			fatal("too big");
		else if(diff < min)
			fatal("too small");
	}

	return (struct acw_trial_t){ off, idx, init, add, NULL };
}

/**
 * Probe to find a the first effecient trial.
 *   @enc: The encoder.
 *   @bits: The number of bits.
 *   &returns: The trail. Failure will return a trial with UINT_MAX offset.
 */
struct acw_trial_t enc_probe(struct acw_encode_t *enc, unsigned int bits)
{
	unsigned int rd, off;
	struct acw_trial_t trial;

	off = 0;
	rd = enc->rd;
	do {
		trial = enc_trial(enc, bits, off);
		if(trial.len >= 8)
			return trial;

		off++;
		rd = (rd + 1) % BUFLEN;
	} while((off < (BUFLEN / 2) && (rd != enc->wr)));

	return (struct acw_trial_t){ UINT_MAX, 0, 0, 0 };
}

/**
 * Flush data into a node chain.
 *   @enc: The encoder.
 */
void enc_flush(struct acw_encode_t *enc)
{
	struct acw_trial_t trial[3];

	trial[0] = enc_probe(enc, 4);
	trial[1] = enc_probe(enc, 8);
	trial[2] = enc_probe(enc, 16);

	if(trial[0].off == 0) {
		*enc->tail = enc_block(enc, 4, trial[0]);
	}
	else if(trial[1].off == 0) {
		trial[1].len = m_min_u_vec((unsigned int []){ trial[0].off, trial[1].len }, 2);
		*enc->tail = enc_block(enc, 8, trial[1]);
	}
	else if(trial[2].off == 0) {
		trial[2].len = m_min_u_vec((unsigned int []){ trial[0].off, trial[1].off, trial[2].len }, 3);
		*enc->tail = enc_block(enc, 16, trial[2]);
	}
	else {
		struct acw_trial_t uncomp = { 0, 0, 0, 0 };

		uncomp.len = m_min_u_vec((unsigned int []){ trial[0].off, trial[1].off, trial[2].off, 256, (enc->wr - enc->rd + BUFLEN - 1) % BUFLEN + 1 }, 5);
		*enc->tail = enc_block(enc, 24, uncomp);
	}

	enc->rd = (enc->rd + (*enc->tail)->len) % BUFLEN;
	enc->tail = &(*enc->tail)->next;
}

struct acw_block_t *enc_block(struct acw_encode_t *enc, unsigned int bits, struct acw_trial_t trial)
{
	int val, off;
	void *raw;
	unsigned int i;
	struct acw_block_t *block;

	block = malloc(sizeof(struct acw_block_t));
	block->bits = bits;
	block->len = trial.len;
	block->init = trial.init;
	block->add = trial.add;

	switch(bits) {
	case 24:
		block->nbytes = 2 + 3 * block->len;
		block->raw = raw = malloc(block->nbytes);

		*(uint16_t *)raw = (ACW_24BIT << 12) | block->len;
		raw += 2;

		for(i = 0; i < block->len; i++) {
			val = enc->buf[(enc->rd + i) % BUFLEN];

			*(uint32_t *)raw = (*(uint32_t *)raw & 0xFF000000) | (val & 0x00FFFFFF);
			raw += 3;
		}

		break;

	case 16:
		off = block->init;
		block->nbytes = 6 + 2 * (block->len - 1);
		block->raw = raw = malloc(block->nbytes);

		*(uint16_t *)raw = (ACW_16BIT << 12) | (block->add & 0x0FFF);
		raw += 2;
		*(uint32_t *)raw = (*(uint32_t *)raw & 0xFF000000) | (off & 0x00FFFFFF);
		raw += 3;
		*(uint8_t *)raw = block->len - 1;
		raw += 1;

		for(i = 1; i < block->len; i++) {
			off += block->add;
			val = enc->buf[(enc->rd + i) % BUFLEN] - off;

			*(uint16_t *)raw = (int16_t)val;
			raw += 2;
		}

		break;

	case 8:
		off = block->init;
		block->nbytes = 6 + block->len - 1;
		block->raw = raw = malloc(block->nbytes);

		*(uint16_t *)raw = (ACW_8BIT << 12) | (block->add & 0x0FFF);
		raw += 2;
		*(uint32_t *)raw = (*(uint32_t *)raw & 0xFF000000) | (off & 0x00FFFFFF);
		raw += 3;
		*(uint8_t *)raw = block->len - 1;
		raw += 1;

		for(i = 1; i < block->len; i++) {
			off += block->add;
			val = enc->buf[(enc->rd + i) % BUFLEN] - off;

			*(uint8_t *)raw = (int8_t)val;
			raw += 1;
		}

		break;

	case 4:
		off = block->init;
		block->nbytes = 6 + block->len / 2;
		block->raw = raw = malloc(block->nbytes+4);

		*(uint16_t *)raw = (ACW_4BIT << 12) | (block->add & 0x0FFF);
		raw += 2;
		*(uint32_t *)raw = (*(uint32_t *)raw & 0xFF000000) | (off & 0x00FFFFFF);
		raw += 3;
		*(uint8_t *)raw = block->len - 1;
		raw += 1;

		for(i = 1; i < block->len; i++) {
			off += block->add;
			val = enc->buf[(enc->rd + i) % BUFLEN] - off;

			if((i % 2) == 1)
				*(uint8_t *)raw = val & 0x0F;
			else
				*(uint8_t *)raw++ |= (val & 0x0F) << 4;
		}

		if((i % 2) == 0)
			raw++;

		break;

	default:
		fatal("Invalid bit width.");
	}

	enc->nbytes += block->nbytes;
	assert((raw - block->raw) == block->nbytes);

	return block;
}
