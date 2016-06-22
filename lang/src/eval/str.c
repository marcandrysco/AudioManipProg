#include "../common.h"


/**
 * Evaluate if a value is a numeric.
 *   @ret: Ref. The return value.
 *   @value: The value.
 *   @env: The environment.
 *   &returns: Error.
 */
char *ml_eval_strlen(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env)
{
	if(value->type != ml_value_str_v)
		return mprintf("%C: Expected string.", ml_tag_chunk(&value->tag));

	*ret = ml_value_num(strlen(value->data.str), ml_tag_copy(value->tag));
	return NULL;
}
