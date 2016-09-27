#ifndef PLAY_H
#define PLAY_H

/**
 * Buffer structure.
 *   @info: The buffer information.
 *   @data: The data.
 */
struct acw_buf_t {
	struct acw_info_t info;

	void *data;
};

/*
 * buffer declarations
 */
struct acw_buf_t *acw_buf_load(const char *path);
void acw_buf_delete(struct acw_buf_t *buf);

int32_t *acw_buf_pcm32(struct acw_buf_t *buf);


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

	if(play->len == 0) {
		acw_play_block(play);

		if(play->bits != 24)
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

#endif
