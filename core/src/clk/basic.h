#ifndef CLK_BASIC_H
#define CLK_BASIC_H

/*
 * basic clock declarations
 */
struct amp_basic_t;

extern const struct amp_clock_i amp_basic_iface;

struct amp_basic_t *amp_basic_new(double bpm, double nbeats, unsigned int rate);
struct amp_basic_t *amp_basic_copy(struct amp_basic_t *basic);
void amp_basic_delete(struct amp_basic_t *basic);

char *amp_basic_make(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env);

void amp_basic_seek(struct amp_basic_t *basic, double bar);

void amp_basic_info(struct amp_basic_t *basic, struct amp_info_t info);
void amp_basic_proc(struct amp_basic_t *basic, struct amp_time_t *time, unsigned int len);


/**
 * Create a clock instance from a basic clock.
 *   @basic: The basic clock.
 *   &returns: The clock instance.
 */
static inline struct amp_clock_t amp_basic_clock(struct amp_basic_t *basic)
{
	return (struct amp_clock_t){ basic, &amp_basic_iface };
}


/**
 * Repeat a time.
 *   @time: The time.
 *   @off: The offset.
 *   @len: The length.
 *   &returns: The repeated time.
 */
static inline struct amp_time_t amp_time_repeat(struct amp_time_t time, int off, unsigned int len)
{
	time.bar = fmod(fmod(time.bar + off, len) + len, len);

	return time;
}

/**
 * Modulus a time.
 *   @time: The time.
 *   @mod: The modulus.
 */
static inline struct amp_time_t amp_time_mod(struct amp_time_t time, struct amp_time_t mod)
{
	if(mod.idx > 0)
		time.idx = dsp_mod_i(time.idx, mod.idx);

	if(mod.bar > 0)
		time.bar = dsp_mod_d(time.bar, mod.bar);

	if(mod.beat > 0)
		time.beat = dsp_mod_d(time.beat, mod.beat);

	return time;
}

/**
 * Compare two times.
 *   @left: The left time.
 *   @right: The right time.
 *   &returns: Their order.
 */
static inline int amp_time_cmp(struct amp_time_t left, struct amp_time_t right)
{
	if((int)floor(left.bar) > (int)floor(right.bar))
		return 2;
	else if((int)floor(left.bar) < (int)floor(right.bar))
		return -2;
	else if(left.beat > right.beat)
		return 1;
	else if(left.beat < right.beat)
		return -1;
	else
		return 0;
}

/**
 * Check if a time falls between two other times.
 *   @time: The time.
 *   @left: The left time.
 *   @right: The right time.
 *   &returns: True if between.
 */
static inline bool amp_time_between(struct amp_time_t time, struct amp_time_t left, struct amp_time_t right)
{
	if(amp_time_cmp(left, right) < 0)
		return (amp_time_cmp(left, time) <= 0) && (amp_time_cmp(time, right) < 0);
	else
		return (amp_time_cmp(left, time) <= 0) || (amp_time_cmp(time, right) < 0);
}

static inline bool amp_time_chkbeat(struct amp_time_t time, struct amp_time_t left, struct amp_time_t right)
{
	if(amp_time_cmp(left, right) < 0)
		return (amp_time_cmp(left, time) <= 0) && (amp_time_cmp(time, right) < 0);
	else
		return (amp_time_cmp(left, time) <= 0) || (amp_time_cmp(time, right) < 0);
}

/**
 * Calculate a time given an index and beat parameters.
 *   @idx: The index.
 *   @bpm: The beats-per-minute.
 *   @nbeats: The beats-per-measure.
 *   @rate: The sample rate.
 *   &returns: The time.
 */
static inline struct amp_time_t amp_time_calc(int idx, double bpm, double nbeats, unsigned int rate)
{
	double beat;
	struct amp_time_t time;
	
	beat = (idx * bpm) / (rate * 60);

	time.idx = idx;
	time.bar = beat / nbeats;
	time.beat = beat - (int)time.bar * nbeats;

	if(time.beat < 0) {
		time.beat += nbeats;
		time.bar -= 1.0;
	}

	return time;
}

/**
 * Calculate a location given an index and beat parameters.
 *   @idx: The index.
 *   @bpm: The beats-per-minute.
 *   @nbeats: The beats-per-measure.
 *   @rate: The sample rate.
 *   &returns: The location.
 */
static inline struct amp_loc_t amp_loc_calc(int idx, double bpm, double nbeats, unsigned int rate)
{
	double beat;
	struct amp_loc_t loc;
	
	beat = (idx * bpm) / (rate * 60);

	loc.bar = beat / nbeats;
	loc.beat = beat - (int)loc.bar * nbeats;

	if(loc.beat < 0) {
		loc.beat += nbeats;
		loc.bar -= 1.0;
	}

	return loc;
}

static inline bool amp_time_isequal(struct amp_time_t left, struct amp_time_t right)
{
	return (left.bar == right.bar) && (left.beat == right.beat);
}
#endif
