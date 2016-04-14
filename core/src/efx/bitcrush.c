#include "../common.h"


/**
 * Bitcrusher structure.
 *   @bits: The bits.
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
	struct amp_bitcrush_t *crush;

	crush = malloc(sizeof(struct amp_bitcrush_t));
	crush->bits = bits;

	return crush;
}

/**
 * Copy a bitcrusher effect.
 *   @crush: The original bitcrusher.
 *   &returns: The copied bitcrusher.
 */
struct amp_bitcrush_t *amp_bitcrush_copy(struct amp_bitcrush_t *crush)
{
	return amp_bitcrush_new(amp_param_copy(crush->bits));
}

/**
 * Delete a bitcrusher effect.
 *   @crush: The bitcrusher.
 */
void amp_bitcrush_delete(struct amp_bitcrush_t *crush)
{
	amp_param_delete(crush->bits);
	free(crush);
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
	struct amp_param_t *bits;

	*err = amp_match_unpack(value, "P", &bits);
	if(*err != NULL)
		return NULL;

	return amp_pack_effect((struct amp_effect_t){ amp_bitcrush_new(bits), &amp_bitcrush_iface });
}


/**
 * Handle information on a bitcrusher.
 *   @crush: The bitcrusher.
 *   @info: The information.
 */

void amp_bitcrush_info(struct amp_bitcrush_t *crush, struct amp_info_t info)
{
	amp_param_info(crush->bits, info);
}

/**
 * Process a bitcrusher.
 *   @crush: The bitcrusher.
 *   @buf: The buffer.
 *   @time: The time.
 *   @len: The length.
 *   @queue: The action queue.
 *   &returns: The continuation flag.
 */
bool amp_bitcrush_proc(struct amp_bitcrush_t *crush, double *buf, struct amp_time_t *time, unsigned int len, struct amp_queue_t *queue)
{
	unsigned int i;

	if(!amp_param_isfast(crush->bits)) {
		double t, inc[len];

		amp_param_proc(crush->bits, inc, time, len, queue);

		for(i = 0; i < len; i++) {
			t = dsp_pow_f(2.0f, fmax(inc[i], 1e-10));
			buf[i] = round(buf[i] * t) / t;
		}
	}
	else {
		double inc = dsp_pow_f(2.0f, crush->bits->flt);

		for(i = 0; i < len; i++)
			buf[i] = round(buf[i] * inc) / inc;
	}

	return false;
}
