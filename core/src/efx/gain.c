#include "../common.h"


/**
 * Gain structure.
 *   @scale: The scale.
 */

struct amp_gain_t {
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
 *   @scale: The scale.
 *   &returns: The gain.
 */

struct amp_gain_t *amp_gain_new(struct amp_param_t *scale)
{
	struct amp_gain_t *gain;

	gain = malloc(sizeof(struct amp_gain_t));
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
	return amp_gain_new(amp_param_copy(gain->scale));
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
 *   @value: The value.
 *   @env: The environment.
 *   @err: The error.
 *   &returns: The value or null.
 */

struct ml_value_t *amp_gain_make(struct ml_value_t *value, struct ml_env_t *env, char **err)
{
	struct amp_param_t *scale;

	*err = amp_match_unpack(value, "P", &scale);

	return (*err == NULL) ? amp_pack_effect((struct amp_effect_t){ amp_gain_new(scale), &amp_gain_iface }) : NULL;
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
 */

void amp_gain_proc(struct amp_gain_t *gain, double *buf, struct amp_time_t *time, unsigned int len)
{
	unsigned int i;
	double scale[len];

	amp_param_proc(gain->scale, scale, time, len);

	for(i = 0; i < len; i++)
		buf[i] *= scale[i];
		//buf[i] *= gain->scale.flt;
}
