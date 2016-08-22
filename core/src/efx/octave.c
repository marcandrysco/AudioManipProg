#include "../common.h"

/*
 * global variables
 */
const struct amp_effect_i amp_octave_iface = {
	(amp_info_f)amp_octave_info,
	(amp_effect_f)amp_octave_proc,
	(amp_copy_f)amp_octave_copy,
	(amp_delete_f)amp_octave_delete
};


/**
 * Tone structure.
 *   @t, q, f, g, s: The time, quality, frequency, gain, and filter state.
 */
struct dsp_tone2_t {
	double t, q, f, g, s[2];
};

/**
 * Tuner structure.
 *   @vol: The volume.
 *   @last: The last value.
 *   @tm, rate: Time since previous crossing and smaple rate.
 *   @len: The tone array length.
 *   @tone: The tone array.
 */
struct dsp_tuner_t {
	struct dsp_vol_t vol;
	double last;
	unsigned int tm, rate;

	unsigned int len;
	struct dsp_tone2_t tone[];
};

/**
 * Create a tuner.
 *   @lpf: The low-pass frequency.
 *   @rate: The sample rate.
 *   &returns: The tuner.
 */
struct dsp_tuner_t *dsp_tuner_new(double lpf, unsigned int rate)
{
	unsigned int i;
	struct dsp_tuner_t *tuner;

	unsigned int n = 2;

	tuner = malloc(n * sizeof(struct dsp_tone2_t) + sizeof(struct dsp_tuner_t));
	tuner->vol = dsp_vol_init(lpf, rate);
	tuner->tm = 1;
	tuner->last = 0.0;
	tuner->len = n;
	tuner->rate = rate;

	for(i = 0; i < n; i++)
		tuner->tone[i] = (struct dsp_tone2_t){ 0.0, 0.0, 0.0, 0.0, { 0.0, 0.0 } };

	return tuner;
}

/**
 * Delete a tuner.
 *   @tuner: The tuner.
 */
void dsp_tuner_delete(struct dsp_tuner_t *tuner)
{
	free(tuner);
}

static inline void dsp_tuner_proc(struct dsp_tuner_t *tuner, double v)
{
	unsigned int i;

	//for(i = 0; i < 20; i++)
		//printf("tri: %.2f -> %.3f\n", i / 20.0, dsp_osc_tri(i / 20.0));
	//exit(0);
	for(i = 0; i < tuner->len; i++) {
		double w, m, t, s;
		struct dsp_tone2_t *tone = &tuner->tone[i];

		if(tone->g < 0.0001)
			continue;

		s = dsp_res_proc(v, dsp_res_init(tone->f, 1.0, tuner->rate), tone->s);
		w = tone->g * dsp_osc_sine_f(tone->t);

		if(fabs(s - w) > (tone->g)) {
			*tone = (struct dsp_tone2_t){ 0.0, 0.0, 0.0, 0.0, { 0.0, 0.0 } };
			continue;
		}

		/* adjust gain */
		m = dsp_osc_tri(tone->t);
		t = s - w;
		t = 0.005 * t * m;
		tone->g += t;

		/* adjust freq */
		m = dsp_osc_tri(tone->t + 0.25);
		t = (s - w) * m;
		tone->f += t * 0.03;
		tone->t = dsp_osc_inc(tone->t, t * 0.03);

		tone->t = dsp_osc_inc(tone->t, dsp_osc_step(tone->f, tuner->rate));

		v -= w;
	}

	if((tuner->last <= 0.0) && (v > 0.0)) {
		double freq = (double)tuner->rate / (double)tuner->tm;

		if(freq < 8000.0) {
			for(i = 0; i < tuner->len; i++) {
				if(tuner->tone[i].g < (tuner->vol.v * 0.25))
					break;
			}

			if(i < tuner->len) {
				tuner->tone[i] = (struct dsp_tone2_t){ 0.0, 1.0, freq, tuner->vol.v, { 0.0, 0.0 } };
			}
		}

		tuner->tm = 1;
	}
	else
		tuner->tm++;

	tuner->last = v;
	dsp_vol_proc(&tuner->vol, v);
}


/**
 * Octave structure.
 *   @vol: The volume tracker.
 *   @mode: The mode.
 *   @thresh: The threshold.
 *   @rate: The sample rate.
 *   @pos: The positive edge boolean.
 *   @out: The output.
 *   @cnt: The count.
 */
struct amp_octave_t {
	struct dsp_vol_t vol;

	int mode;
	double thresh;
	unsigned int rate;

	bool pos;
	double out;
	unsigned int cnt;
};


/**
 * Create a octave effect.
 *   @mode: The mode.
 *   @thresh: The threshold.
 *   @rate: Sample rate.
 *   &returns: The octave.
 */
struct amp_octave_t *amp_octave_new(int mode, double thresh, unsigned int rate)
{
	struct amp_octave_t *octave;

	octave = malloc(sizeof(struct amp_octave_t));
	octave->vol = dsp_vol_init(20.0, rate);
	octave->mode = mode;
	octave->thresh = thresh;
	octave->rate = rate;
	octave->pos = false;
	octave->cnt = 0;
	octave->out = 1.0;

	return octave;
}

/**
 * Copy a octave effect.
 *   @octave: The original octave.
 *   &returns: The copied octave.
 */
struct amp_octave_t *amp_octave_copy(struct amp_octave_t *octave)
{
	return amp_octave_new(octave->mode, octave->thresh, octave->rate);
}

/**
 * Delete a octave effect.
 *   @octave: The octave.
 */
void amp_octave_delete(struct amp_octave_t *octave)
{
	free(octave);
}


/**
 * Create a octave from a value.
 *   @ret: Ref. The returned value.
 *   @value: The value.
 *   @env: The environment.
 *   &returns: Error.
 */
char *amp_octave_make(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env)
{
#define onexit
	int mode;
	double thresh;

	chkfail(amp_match_unpack(value, "(d,f)", &mode, &thresh));

	*ret = amp_pack_effect((struct amp_effect_t){ amp_octave_new(mode, thresh, amp_core_rate(env)), &amp_octave_iface });
	return NULL;
#undef onexit
}


/**
 * Handle information on a octave.
 *   @octave: The octave.
 *   @info: The information.
 */
void amp_octave_info(struct amp_octave_t *octave, struct amp_info_t info)
{
}

/**
 * Process a octave.
 *   @octave: The octave.
 *   @buf: The buffer.
 *   @time: The time.
 *   @len: The length.
 *   @queue: The action queue.
 *   &returns: The continuation flag.
 */

bool amp_octave_proc(struct amp_octave_t *octave, double *buf, struct amp_time_t *time, unsigned int len, struct amp_queue_t *queue)
{
	bool pos;
	double thresh, out;
	struct dsp_vol_t *vol;
	unsigned int i;

	pos = octave->pos;
	out = octave->out;
	vol = &octave->vol;
	thresh = octave->thresh;

	for(i = 0; i < len; i++) {
		dsp_vol_proc(vol, buf[i]);

		if(pos && (buf[i] < (-thresh * vol->v))) {
			pos = false;
		}
		else if(!pos && (buf[i] > (thresh * vol->v))) {
			pos = true;

			if(octave->mode <= 0) {
				if(octave->cnt++ >= 1)
					out = 1.0 - out, octave->cnt = 0;
			}
		}

		buf[i] = out;
	}

	octave->pos = pos;
	octave->out = out;

	return false;
}
