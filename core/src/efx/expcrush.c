#include "../common.h"


/**
 * Exponential bitcrusher structure.
 *   @bits: The bits.
 */
struct amp_expcrush_t {
	struct amp_param_t *bits;
};


/*
 * global variables
 */
const struct amp_effect_i amp_expcrush_iface = {
	(amp_info_f)amp_expcrush_info,
	(amp_effect_f)amp_expcrush_proc,
	(amp_copy_f)amp_expcrush_copy,
	(amp_delete_f)amp_expcrush_delete
};


/**
 * Create a expcrusher effect.
 *   @bits: The bits.
 *   &returns: The expcrusher.
 */
struct amp_expcrush_t *amp_expcrush_new(struct amp_param_t *bits)
{
	struct amp_expcrush_t *crush;

	crush = malloc(sizeof(struct amp_expcrush_t));
	crush->bits = bits;

	return crush;
}

/**
 * Copy a expcrusher effect.
 *   @crush: The original expcrusher.
 *   &returns: The copied expcrusher.
 */
struct amp_expcrush_t *amp_expcrush_copy(struct amp_expcrush_t *crush)
{
	return amp_expcrush_new(amp_param_copy(crush->bits));
}

/**
 * Delete a expcrusher effect.
 *   @crush: The expcrusher.
 */
void amp_expcrush_delete(struct amp_expcrush_t *crush)
{
	amp_param_delete(crush->bits);
	free(crush);
}


/**
 * Create a expcrusher from a value.
 *   @value: The value.
 *   @env: The environment.
 *   @err: The error.
 *   &returns: The value or null.
 */
struct ml_value_t *amp_expcrush_make(struct ml_value_t *value, struct ml_env_t *env, char **err)
{
	struct amp_param_t *bits;

	*err = amp_match_unpack(value, "P", &bits);
	if(*err != NULL)
		return NULL;

	return amp_pack_effect((struct amp_effect_t){ amp_expcrush_new(bits), &amp_expcrush_iface });
}


/**
 * Handle information on a expcrusher.
 *   @crush: The expcrusher.
 *   @info: The information.
 */
void amp_expcrush_info(struct amp_expcrush_t *crush, struct amp_info_t info)
{
	amp_param_info(crush->bits, info);
}

/**
 * Process a single number.
 *   @x: The input.
 *   @bits: The number of bits
 *   &returns: The processed number.
 */
static inline float expcrush(float x, float bits)
{
	if(x > 0.0)
		return expf(roundf(bits * logf(x)) / bits);
	else if(x < 0.0)
		return -expf(roundf(bits * logf(-x)) / bits);
	else
		return 0;
}

/**
 * Process a expcrusher.
 *   @crush: The expcrusher.
 *   @buf: The buffer.
 *   @time: The time.
 *   @len: The length.
 *   @queue: The action queue.
 *   &returns: The contuation flag.
 */
bool amp_expcrush_proc(struct amp_expcrush_t *crush, double *buf, struct amp_time_t *time, unsigned int len, struct amp_queue_t *queue)
{
	bool cont = false;
	unsigned int i;

	if(!amp_param_isfast(crush->bits)) {
		double bits[len];

		cont = amp_param_proc(crush->bits, bits, time, len, queue);

		for(i = 0; i < len; i++)
			buf[i] = expcrush(buf[i], bits[i]);
	}
	else {
		double bits = crush->bits->flt;

		for(i = 0; i < len; i++)
			buf[i] = expcrush(buf[i], bits);
	}

	return cont;
}
