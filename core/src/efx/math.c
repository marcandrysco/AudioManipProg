#include "../common.h"


/**
 * Math function structure.
 *   @type: The function type.
 *   @param: Function parameter.
 *   @rate: The ssample rate.
 */
struct amp_math_t {
	enum amp_math_e type;
	struct amp_param_t *param;
	
	unsigned int rate;
};


/*
 * global variables
 */
const struct amp_effect_i amp_math_iface = {
	(amp_info_f)amp_math_info,
	(amp_effect_f)amp_math_proc,
	(amp_copy_f)amp_math_copy,
	(amp_delete_f)amp_math_delete
};


/**
 * Create a math function.
 *   @type: The function type.
 *   @param: The parameter, may be optional.
 *   @rate: The sample rate.
 *   &returns: The math function.
 */
struct amp_math_t *amp_math_new(enum amp_math_e type, struct amp_param_t *param, unsigned int rate)
{
	struct amp_math_t *math;

	math = malloc(sizeof(struct amp_math_t));
	math->type = type;
	math->param = param;
	math->rate = rate;

	return math;
}

/**
 * Copy a math function.
 *   @math: The original math function.
 *   &returns: The copied math function.
 */
struct amp_math_t *amp_math_copy(struct amp_math_t *math)
{
	return amp_math_new(math->type, math->param ? amp_param_copy(math->param) : NULL, math->rate);
}

/**
 * Delete a math function.
 *   @math: The math function.
 */
void amp_math_delete(struct amp_math_t *math)
{
	amp_param_erase(math->param);
	free(math);
}


/**
 * Create a math function from a value.
 *   @ret: Ref. The return value.
 *   @value: The value.
 *   @env: The environment.
 *   &returns: Error
 */
char *amp_exp_make(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env)
{
	if(value->type != ml_value_nil_v)
		return mprintf("%C: Expected '()'.", ml_tag_chunk(&value->tag));

	*ret = amp_pack_effect((struct amp_effect_t){ amp_math_new(amp_math_exp_v, NULL, amp_core_rate(env)), &amp_math_iface });

	return NULL;
}

/**
 * Create a math function from a value.
 *   @ret: Ref. The return value.
 *   @value: The value.
 *   @env: The environment.
 *   &returns: Error
 */
char *amp_hz2sec_make(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env)
{
	if(value->type != ml_value_nil_v)
		return mprintf("%C: Expected '()'.", ml_tag_chunk(&value->tag));

	*ret = amp_pack_effect((struct amp_effect_t){ amp_math_new(amp_math_hz2sec_v, NULL, amp_core_rate(env)), &amp_math_iface });

	return NULL;
}


/**
 * Handle information on a math function.
 *   @math: The math function.
 *   @info: The information.
 */
void amp_math_info(struct amp_math_t *math, struct amp_info_t info)
{
	if(math->param != NULL)
		amp_param_info(math->param, info);
}

/**
 * Process a math function.
 *   @math: The math function.
 *   @buf: The buffer.
 *   @time: The time.
 *   @len: The length.
 *   @queue: The action queue.
 *   &returns: The mathinuation flag.
 */
bool amp_math_proc(struct amp_math_t *math, double *buf, struct amp_time_t *time, unsigned int len, struct amp_queue_t *queue)
{
	unsigned int i;

	switch(math->type) {
	case amp_math_exp_v:
		for(i = 0; i < len; i++)
			buf[i] = expf(buf[i]);

		break;

	case amp_math_hz2sec_v:
		for(i = 0; i < len; i++)
			buf[i] = 1.0 / buf[i];

		break;
	}

	return false;
}
