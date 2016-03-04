#include "../common.h"


/**
 * Bitcrusher structure.
 *   @scale: The scale.
 */

struct amp_bitcrush_t {
	struct amp_param_t *bits;
};


/*
 * global variables
 */

const struct amp_effect_i amp_bitcrush_iface = {
	(amp_info_f)amp_bitcrush_info,
	(amp_effect_f)amp_bitcrush_proc,
	(amp_copy_f)amp_bitcrush_copy,
	(amp_delete_f)amp_bitcrush_delete
};


/**
 * Create a bitcrusher effect.
 *   @bits: The bits.
 *   &returns: The bitcrusher.
 */

struct amp_bitcrush_t *amp_bitcrush_new(struct amp_param_t *bits)
{
	struct amp_bitcrush_t *bitcrush;

	bitcrush = malloc(sizeof(struct amp_bitcrush_t));
	bitcrush->bits = bits;

	return bitcrush;
}

/**
 * Copy a bitcrusher effect.
 *   @bitcrush: The original bitcrush.
 *   &returns: The copied bitcrush.
 */

struct amp_bitcrush_t *amp_bitcrush_copy(struct amp_bitcrush_t *bitcrush)
{
	return amp_bitcrush_new(amp_param_copy(bitcrush->bits));
}

/**
 * Delete a bitcrusher effect.
 *   @bitcrush: The bitcrusher.
 */

void amp_bitcrush_delete(struct amp_bitcrush_t *bitcrush)
{
	amp_param_delete(bitcrush->bits);
	free(bitcrush);
}


/**
 * Create a bitcrusher from a value.
 *   @value: The value.
 *   @env: The environment.
 *   @err: The error.
 *   &returns: The value or null.
 */

struct ml_value_t *amp_bitcrush_make(struct ml_value_t *value, struct ml_env_t *env, char **err)
{
	struct amp_param_t *scale;

	*err = amp_match_unpack(value, "P", &scale);
	if(*err != NULL)
		return NULL;

	return amp_pack_effect((struct amp_effect_t){ amp_bitcrush_new(scale), &amp_bitcrush_iface });
}


/**
 * Handle information on a bitcrusher.
 *   @bitcrush: The bitcrusher.
 *   @info: The information.
 */

void amp_bitcrush_info(struct amp_bitcrush_t *bitcrush, struct amp_info_t info)
{
	amp_param_info(bitcrush->bits, info);
}

/**
 * Process a bitcrusher.
 *   @bitcrush: The bitcrusher.
 *   @buf: The buffer.
 *   @time: The time.
 *   @len: The length.
 */

void amp_bitcrush_proc(struct amp_bitcrush_t *bitcrush, double *buf, struct amp_time_t *time, unsigned int len)
{
	/*
	unsigned int i;
	double scale[len];

	amp_param_proc(bitcrush->scale, scale, time, len);

	for(i = 0; i < len; i++)
		buf[i] *= scale[i];
		*/
		//buf[i] *= bitcrush->scale.flt;
}
