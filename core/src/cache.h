#ifndef CACHE_H
#define CACHE_H

/*
 * cache declarations
 */
struct amp_cache_t;

struct amp_cache_t *amp_cache_new(void);
void amp_cache_delete(struct amp_cache_t *cache);

struct amp_file_t *amp_cache_lookup(struct amp_cache_t *cache, const char *path, unsigned int chan);

struct amp_file_t *amp_cache_open(struct amp_cache_t *cache, const char *path, unsigned int chan, unsigned int rate);
void amp_cache_close(struct amp_cache_t *cache, struct amp_file_t *file);

/*
 *  file declarations
 */
struct amp_file_t;

struct amp_file_t *amp_file_copy(struct amp_file_t *file);
void amp_file_delete(struct amp_file_t *file);

struct acw_buf_t amp_file_buf(struct amp_file_t *file);


/**
 * Player structure.
 *   @acw: The ACW player.
 *   @
 */
struct amp_play_t {
	struct acw_play_t acw;
	float idx, ratio, prev, next;
	float vol, mul;
};

/**
 * Initialize the playback.
 *   @play: The playback array.
 *   @n: The number of players.
 */
static inline void amp_play_init(struct amp_play_t *play, unsigned int n)
{
	unsigned int i;

	for(i = 0; i < n; i++)
		play[i].vol = 0.0;
}

/**
 * Add to a playback.
 *   @play: The playback array.
 *   @n: The number of players.
 *   @buf: The buffer.
 *   @vol: The initial volume.
 *   @rate: The sample rate.
 *   &returns: The allocated number.
 */
static inline unsigned int amp_play_add(struct amp_play_t *play, unsigned int n, struct acw_buf_t buf, float vol, float rate)
{
	double min = INFINITY;
	unsigned int i, sel = 0;

	for(i = 0; i < n; i++) {
		if(play[i].vol < min)
			sel = i, min = play[i].vol;
	}

	buf.info.rate = 44100;
	play[sel] = (struct amp_play_t){ acw_play_init(buf.raw), 0.0f, buf.info.rate / rate, 0.0f, 0.0f, vol, 1.0f };

	return sel;
}

/**
 * Process the playback.
 *   @play: The playback array.
 *   @n: The number of players.
 *   &returns: The value.
 */
static inline double amp_play_proc(struct amp_play_t *play, unsigned int n)
{
	unsigned int i;
	double v = 0.0;

	for(i = 0; i < n; i++) {
		if(play[i].vol == 0.0)
			continue;

		if(floorf(play[i].idx * play[i].ratio) != floorf((play[i].idx-1.0f) * play[i].ratio)) {
			play[i].prev = play[i].next;
			play[i].next = (float)acw_play_next(&play[i].acw) / (float)(1 << 23);
		}

		float r = play[i].idx - floorf(play[i].idx);
		float val = play[i].prev * (1.0 - r) + r * play[i].next;

		v += play[i].vol * val;

		play[i].idx++;
		play[i].vol *= play[i].mul;
		if(acw_play_eos(&play[i].acw) || (play[i].vol < 0.001))
			play[i].vol = 0.0f;
	}
	
	return v;
}

#endif
