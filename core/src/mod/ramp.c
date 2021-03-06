#include "../common.h"


/**
 * Sample structure.
 *   @type: The ramp type.
 *   @freq: The frequency.
 *   @v, rate: The current value and sample rate.
 */
struct amp_ramp_t {
	enum amp_ramp_e type;
	struct amp_param_t *freq;

	double v, rate;
};


/*
 * global variables
 */
const struct amp_module_i amp_ramp_iface = {
	(amp_info_f)amp_ramp_info,
	(amp_module_f)amp_ramp_proc,
	(amp_copy_f)amp_ramp_copy,
	(amp_delete_f)amp_ramp_delete
};


/**
 * Create a ramp.
 *   @type: The ramp type.
 *   @freq: Consumed. The ramping frequency.
 *   &returns: The ramp.
 */
struct amp_ramp_t *amp_ramp_new(enum amp_ramp_e type, struct amp_param_t *freq, unsigned int rate)
{
	struct amp_ramp_t *ramp;

	ramp = malloc(sizeof(struct amp_ramp_t));
	ramp->type = type;
	ramp->freq = freq;
	ramp->v = 0.0;
	ramp->rate = rate;

	return ramp;
}

/**
 * Copy a ramp.
 *   @ramp: The original ramp.
 *   &returns: The copied ramp.
 */
struct amp_ramp_t *amp_ramp_copy(struct amp_ramp_t *ramp)
{
	return amp_ramp_new(ramp->type, amp_param_copy(ramp->freq), ramp->rate);
}

/**
 * Delete a ramp.
 *   @ramp: The ramp.
 */
void amp_ramp_delete(struct amp_ramp_t *ramp)
{
	amp_param_delete(ramp->freq);
	free(ramp);
}


/**
 * Create a second-based ramp from a value.
 *   @ret: Ref. The return value.
 *   @value: The value.
 *   @env: The environment.
 *   &returns: Error
 */
char *amp_ramp_make(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env)
{
#define onexit
	struct amp_param_t *freq;

	chkfail(amp_match_unpack(value, "P", &freq));
	*ret = amp_pack_module((struct amp_module_t){ amp_ramp_new(amp_ramp_sec_v, freq, amp_core_rate(env)), &amp_ramp_iface });

	return NULL;
#undef onexit
}

/**
 * Create a beat-based ramp from a value.
 *   @ret: Ref. The return value.
 *   @value: The value.
 *   @env: The environment.
 *   &returns: Error
 */
char *amp_beat_make(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env)
{
#define onexit
	struct amp_param_t *freq;

	chkfail(amp_match_unpack(value, "P", &freq));
	*ret = amp_pack_module((struct amp_module_t){ amp_ramp_new(amp_ramp_beat_v, freq, amp_core_rate(env)), &amp_ramp_iface });

	return NULL;
#undef onexit
}


/**
 * Handle information on a ramp.
 *   @ramp: The ramp.
 *   @info: The information.
 */
void amp_ramp_info(struct amp_ramp_t *ramp, struct amp_info_t info)
{
	amp_param_info(ramp->freq, info);

	if(info.type == amp_info_note_e) {
		if(info.data.note->init)
			ramp->v = 0.0;
	}
}

/**
 * Process a ramp.
 *   @ramp: The ramp.
 *   @buf: The buffer.
 *   @time: The time.
 *   @len: The length.
 *   @queue: The action queue.
 *   &returns: The continuation flag.
 */
bool amp_ramp_proc(struct amp_ramp_t *ramp, double *buf, struct amp_time_t *time, unsigned int len, struct amp_queue_t *queue)
{
	unsigned int i;
	bool cont = false;

	switch(ramp->type) {
	case amp_ramp_sec_v:
		{
			double v, rate;

			v = ramp->v;
			rate = ramp->rate;

			if(amp_param_isfast(ramp->freq)) {
				double inc = dsp_osc_step(ramp->freq->flt, rate);

				for(i = 0; i < len; i++)
					buf[i] = v = dsp_osc_inc(v, inc);
			}
			else {
				double freq[len];

				cont |= amp_param_proc(ramp->freq, freq, time, len, queue);

				for(i = 0; i < len; i++)
					buf[i] = v = dsp_osc_inc(v, dsp_osc_step(freq[i], rate));
			}

			ramp->v = v;
		}
		break;

	case amp_ramp_beat_v:
		{
			if(amp_param_isfast(ramp->freq)) {
				double freq = ramp->freq->flt;

				for(i = 0; i < len; i++)
					buf[i] = fmod(time[i].beat / freq, 1.0);
			}
			else {
				double freq[len];

				cont |= amp_param_proc(ramp->freq, freq, time, len, queue);

				for(i = 0; i < len; i++)
					buf[i] = fmod(time[i].beat / freq[i], 1.0);
			}
		}
		break;
	}

	return cont;
}
