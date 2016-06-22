#include "../common.h"

/*
 * global variables
 */
const struct amp_effect_i amp_bias_iface = {
	(amp_info_f)amp_bias_info,
	(amp_effect_f)amp_bias_proc,
	(amp_copy_f)amp_bias_copy,
	(amp_delete_f)amp_bias_delete
};


/**
 * Create a bias effect.
 *   @val: Consumed. The value.
 *   &returns: The bias.
 */
struct amp_bias_t *amp_bias_new(struct amp_param_t *value)
{
	struct amp_bias_t *bias;

	bias = malloc(sizeof(struct amp_bias_t));
	bias->value = value;

	return bias;
}

/**
 * Copy a bias effect.
 *   @bias: The original bias.
 *   &returns: The copied bias.
 */
struct amp_bias_t *amp_bias_copy(struct amp_bias_t *bias)
{
	return amp_bias_new(amp_param_copy(bias->value));
}

/**
 * Delete a bias effect.
 *   @bias: The bias.
 */
void amp_bias_delete(struct amp_bias_t *bias)
{
	amp_param_delete(bias->value);
	free(bias);
}


/**
 * Create a bias from a value.
 *   @ret: Ref. The returned value.
 *   @value: The value.
 *   @env: The environment.
 *   &returns: Error.
 */
char *amp_bias_make(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env)
{
#define onexit
	struct amp_param_t *val;

	chkfail(amp_match_unpack(value, "P", &val));

	*ret = amp_pack_effect((struct amp_effect_t){ amp_bias_new(val), &amp_bias_iface });
	return NULL;
#undef onexit
}


/**
 * Handle information on a bias.
 *   @bias: The bias.
 *   @info: The information.
 */
void amp_bias_info(struct amp_bias_t *bias, struct amp_info_t info)
{
	amp_param_info(bias->value, info);
}

/**
 * Process a bias.
 *   @bias: The bias.
 *   @buf: The buffer.
 *   @time: The time.
 *   @len: The length.
 *   @queue: The action queue.
 *   &returns: The continuation flag.
 */
bool amp_bias_proc(struct amp_bias_t *bias, double *buf, struct amp_time_t *time, unsigned int len, struct amp_queue_t *queue)
{
	unsigned int i;

	if(amp_param_isfast(bias->value)) {
		double val = bias->value->flt;

		for(i = 0; i < len; i++)
			buf[i] += val;
	}
	else {
		double val[len];

		amp_param_proc(bias->value, val, time, len, queue);

		for(i = 0; i < len; i++)
			buf[i] += val[i];
	}

	return false;
}
