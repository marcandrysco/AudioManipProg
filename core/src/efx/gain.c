#include "../common.h"

/**
 * Gain structure.
 *   @type: The type.
 *   @scale: The scale.
 */
struct amp_gain_t {
	enum amp_gain_e type;
	struct amp_param_t *scale;
};


/*
 * global variables
 */
const struct amp_effect_i amp_gain_iface = {
	(amp_info_f)amp_gain_info,
	(amp_effect_f)amp_gain_proc,
	(amp_copy_f)amp_gain_copy,
	(amp_delete_f)amp_gain_delete
};


/**
 * Create a gain effect.
 *   @scale: Consumed. The scale.
 *   &returns: The gain.
 */
struct amp_gain_t *amp_gain_new(enum amp_gain_e type, struct amp_param_t *scale)
{
	struct amp_gain_t *gain;

	gain = malloc(sizeof(struct amp_gain_t));
	gain->type = type;
	gain->scale = scale;

	return gain;
}

/**
 * Copy a gain effect.
 *   @gain: The original gain.
 *   &returns: The copied gain.
 */
struct amp_gain_t *amp_gain_copy(struct amp_gain_t *gain)
{
	return amp_gain_new(gain->type, amp_param_copy(gain->scale));
}

/**
 * Delete a gain effect.
 *   @gain: The gain.
 */
void amp_gain_delete(struct amp_gain_t *gain)
{
	amp_param_delete(gain->scale);
	free(gain);
}


/**
 * Create a gain from a value.
 *   @ret: Ref. The returned value.
 *   @value: The value.
 *   @env: The environment.
 *   &returns: Error.
 */
char *amp_gain_make(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env)
{
#define onexit
	struct amp_param_t *scale;

	chkfail(amp_match_unpack(value, "P", &scale));
	*ret = amp_pack_effect((struct amp_effect_t){ amp_gain_new(amp_gain_mul_v, scale), &amp_gain_iface });

	return NULL;
#undef onexit
}

/**
 * Create a boost from a value.
 *   @ret: Ref. The returned value.
 *   @value: The value.
 *   @env: The environment.
 *   &returns: Error.
 */
char *amp_boost_make(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env)
{
#define onexit
	struct amp_param_t *scale;

	chkfail(amp_match_unpack(value, "P", &scale));
	*ret = amp_pack_effect((struct amp_effect_t){ amp_gain_new(amp_gain_boost_v, scale), &amp_gain_iface });

	return NULL;
#undef onexit
}

/**
 * Create a cut from a value.
 *   @ret: Ref. The returned value.
 *   @value: The value.
 *   @env: The environment.
 *   &returns: Error.
 */
char *amp_cut_make(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env)
{
#define onexit
	struct amp_param_t *scale;

	chkfail(amp_match_unpack(value, "P", &scale));
	*ret = amp_pack_effect((struct amp_effect_t){ amp_gain_new(amp_gain_cut_v, scale), &amp_gain_iface });

	return NULL;
#undef onexit
}


/**
 * Handle information on a gain.
 *   @gain: The gain.
 *   @info: The information.
 */
void amp_gain_info(struct amp_gain_t *gain, struct amp_info_t info)
{
	amp_param_info(gain->scale, info);
}

/**
 * Process a gain.
 *   @gain: The gain.
 *   @buf: The buffer.
 *   @time: The time.
 *   @len: The length.
 *   @queue: The action queue.
 *   &returns: The continuation flag.
 */
bool amp_gain_proc(struct amp_gain_t *gain, double *buf, struct amp_time_t *time, unsigned int len, struct amp_queue_t *queue)
{
	unsigned int i;

	switch(gain->type) {
	case amp_gain_mul_v:
		if(amp_param_isfast(gain->scale)) {
			double scale = gain->scale->flt;

			for(i = 0; i < len; i++)
				buf[i] *= scale;
		}
		else {
			double scale[len];

			amp_param_proc(gain->scale, scale, time, len, queue);

			for(i = 0; i < len; i++)
				buf[i] *= scale[i];
		}

		break;

	case amp_gain_boost_v:
		if(amp_param_isfast(gain->scale)) {
			double scale = dsp_db2amp_f(gain->scale->flt);

			for(i = 0; i < len; i++)
				buf[i] *= scale;
		}
		else {
			double scale[len];

			amp_param_proc(gain->scale, scale, time, len, queue);

			for(i = 0; i < len; i++)
				buf[i] *= dsp_db2amp_f(scale[i]);
		}

		break;

	case amp_gain_cut_v:
		if(amp_param_isfast(gain->scale)) {
			double scale = dsp_db2amp_f(-gain->scale->flt);

			for(i = 0; i < len; i++)
				buf[i] *= scale;
		}
		else {
			double scale[len];

			amp_param_proc(gain->scale, scale, time, len, queue);

			for(i = 0; i < len; i++)
				buf[i] *= dsp_db2amp_f(-scale[i]);
		}

		break;
	}

	return false;
}
