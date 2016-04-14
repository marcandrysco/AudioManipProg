#include "../common.h"


/**
 * Multiply structure.
 *   @left, right: The left and right parameter.
 */
struct amp_mul_t {
	struct amp_param_t *left, *right;
};


/*
 * global variables
 */
const struct amp_module_i amp_mul_iface = {
	(amp_info_f)amp_mul_info,
	(amp_module_f)amp_mul_proc,
	(amp_copy_f)amp_mul_copy,
	(amp_delete_f)amp_mul_delete
};


/**
 * Create a multiply.
 *   @left: Consumed. The left parameter.
 *   @right: Consumed. The right parameter.
 *   &returns: The multiply.
 */
struct amp_mul_t *amp_mul_new(struct amp_param_t *left, struct amp_param_t *right)
{
	struct amp_mul_t *mul;

	mul = malloc(sizeof(struct amp_mul_t));
	mul->left = left;
	mul->right = right;

	return mul;
}

/**
 * Copy a multiply.
 *   @mul: The original multiply.
 *   &returns: The copied multiply.
 */
struct amp_mul_t *amp_mul_copy(struct amp_mul_t *mul)
{
	return amp_mul_new(amp_param_copy(mul->left), amp_param_copy(mul->right));
}

/**
 * Delete a multiply.
 *   @mul: The multiply.
 */
void amp_mul_delete(struct amp_mul_t *mul)
{
	amp_param_delete(mul->left);
	amp_param_delete(mul->right);
	free(mul);
}


/**
 * Create a multiply from a value.
 *   @value: The value.
 *   @env: The environment.
 *   @err: The error.
 *   &returns: The value or null.
 */
struct ml_value_t *amp_mul_make(struct ml_value_t *value, struct ml_env_t *env, char **err)
{
	struct amp_param_t *left, *right;

	*err = amp_match_unpack(value, "(P,P)", &left, &right);
	if(*err == NULL)
		return amp_pack_module((struct amp_module_t){ amp_mul_new(left, right), &amp_mul_iface });
	else
		return NULL;
}


/**
 * Handle information on a multiply.
 *   @mul: The multiply.
 *   @info: The information.
 */
void amp_mul_info(struct amp_mul_t *mul, struct amp_info_t info)
{
	amp_param_info(mul->left, info);
	amp_param_info(mul->right, info);
}

/**
 * Process a multiply.
 *   @mul: The multiply.
 *   @buf: The buffer.
 *   @time: The time.
 *   @len: The length.
 *   @queue: The action queue.
 *   &returns: The continuation flag.
 */
bool amp_mul_proc(struct amp_mul_t *mul, double *buf, struct amp_time_t *time, unsigned int len, struct amp_queue_t *queue)
{
	unsigned int i;
	bool cont = false;
	double left[len], right[len];

	cont |= amp_param_proc(mul->left, left, time, len, queue);
	cont |= amp_param_proc(mul->right, right, time, len, queue);

	for(i = 0; i < len; i++)
		buf[i] = left[i] * right[i];

	return cont;
}
