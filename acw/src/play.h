#ifndef PLAY_H
#define PLAY_H

/*
 * buffer declarations
 */
char *acw_buf_load(struct acw_buf_t *buf, const char *path);
char *acw_buf_save(struct acw_buf_t buf, const char *path);
void acw_buf_delete(struct acw_buf_t buf);

struct acw_buf_t acw_buf_enc32(int32_t *arr, unsigned int len);

int32_t *acw_buf_pcm32(struct acw_buf_t buf);


/*
 * loading declarations
 */
void *acw_load_raw(const char *path, struct acw_info_t *info);


/**
 * Player structure.
 *   @ptr: The pointer.
 *   @bits, len: The number of bits and the length.
 *   @init, add, frac: The initial, addend, and fraction values.
 */
struct acw_play_t {
	void *ptr;
	unsigned int bits, len;
	int init, add, frac;
};

/**
 * Read a block header.
 *   @play: The player.
 */
static inline void acw_play_block(struct acw_play_t *play)
{
	uint16_t info;

	info = *(uint16_t *)play->ptr;
	play->ptr += 2;
	play->bits = acw_getbits(info >> 12);
	assert((int)play->bits >= 0);

	if(play->bits == 24) {
		play->len = info & 0x0FFF;
		play->init = play->add = 0;
	}
	else {
		play->add = acw_ext(info & 0x0FFF, 12);
		play->init = acw_ext(*(uint32_t *)play->ptr & 0x00FFFFFF, 24);
		play->ptr += 3;
		play->len = *(uint8_t *)play->ptr;
		play->ptr += 1;
		play->frac = 0;

		//printf("blk(%d): %d %d  %d\n", play->bits, play->init, play->add, play->len);
	}
}


/**
 * Initialize a player on memory.
 *   @ptr: The memory pointer.
 *   &returns: The player.
 */
static inline struct acw_play_t acw_play_init(void *ptr)
{
	struct acw_play_t play;

	play.ptr = ptr;
	play.len = 0;

	return play;
}

/**
 * Retrieve the next value from a player.
 *   @player: The player.
 *   &returns: The value.
 */
static inline int acw_play_next(struct acw_play_t *play)
{
	int val;

	if(play->ptr == NULL)
		return 0;

	if(play->len == 0) {
		acw_play_block(play);

		if(play->bits == 24) {
			if(play->len == 0) {
				play->ptr = NULL;

				return 0;
			}
		}
		else
			return play->init;
	}
	
	play->len--;
	val = play->init += play->add;

	switch(play->bits) {
	case 24:
		val += acw_ext(*(int32_t *)play->ptr, 24);
		play->ptr += 3;

		return val;

	case 20:
		fatal("stub");

	case 16:
		val += *(int16_t *)play->ptr;
		play->ptr += 2;

		return val;

	case 12:
		fatal("stub");

	case 8:
		val += *(int8_t *)play->ptr;
		play->ptr++;

		return val;

	case 6:
		fatal("stub");

	case 4:
		if(play->frac == 0)
			val += acw_ext(*(int8_t *)play->ptr++ & 0x0F, 4);
		else
			val += acw_ext((((int8_t *)play->ptr)[-1] & 0xF0) >> 4, 4);

		play->frac = 1 - play->frac;

		return val;
	}

	fatal("Invalid bit width.");
}

/**
 * Check if player is at the end of stream.
 *   @play: The player.
 *   &returns: True if EOS.
 */
static inline bool acw_play_eos(struct acw_play_t *play)
{
	return play->ptr == NULL;
}

#endif
