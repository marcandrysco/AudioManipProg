#include "../common.h"


/**
 * Evaluate an conversion from value to string.
 *   @ret: Ref. The return value.
 *   @value: The value.
 *   @env: The environment.
 *   &returns: Error.
 */
char *ml_eval_val2str(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env)
{
	*ret = ml_value_str(mprintf("%C", ml_value_chunk(value)), ml_tag_copy(value->tag));
	return NULL;
}

/**
 * Evaluate an conversion from float to integer.
 *   @ret: Ref. The return value.
 *   @value: The value.
 *   @env: The environment.
 *   &returns: Error.
 */
char *ml_eval_flt2int(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env)
{
	if(value->type != ml_value_flt_v)
		return mprintf("%C: Expected float.", ml_tag_chunk(&value->tag));

	*ret = ml_value_num(round(value->data.flt), ml_tag_copy(value->tag));
	return NULL;
}


/**
 * Evaluate if a value is an integer.
 *   @ret: Ref. The return value.
 *   @value: The value.
 *   @env: The environment.
 *   &returns: Error.
 */
char *ml_eval_isint(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env)
{
	*ret = ml_value_bool(value->type == ml_value_num_v, ml_tag_copy(value->tag));
	return NULL;
}

/**
 * Evaluate if a value is a float.
 *   @ret: Ref. The return value.
 *   @value: The value.
 *   @env: The environment.
 *   &returns: Error.
 */
char *ml_eval_isflt(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env)
{
	*ret = ml_value_bool(value->type == ml_value_flt_v, ml_tag_copy(value->tag));
	return NULL;
}

/**
 * Evaluate if a value is numeric.
 *   @ret: Ref. The return value.
 *   @value: The value.
 *   @env: The environment.
 *   &returns: Error.
 */
char *ml_eval_isnum(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env)
{
	*ret = ml_value_bool((value->type == ml_value_num_v) || (value->type == ml_value_flt_v), ml_tag_copy(value->tag));
	return NULL;
}

/**
 * Evaluate if a value is a list.
 *   @ret: Ref. The return value.
 *   @value: The value.
 *   @env: The environment.
 *   &returns: Error.
 */
char *ml_eval_islist(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env)
{
	*ret = ml_value_bool(value->type == ml_value_list_v, ml_tag_copy(value->tag));
	return NULL;
}
