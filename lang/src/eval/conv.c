#include "../common.h"


/**
 * Evaluate an conversion from value to string.
 *   @ret: @Ref: The return value.
 *   @value: The value.
 *   @env: The environment.
 *   &returns: Error.
 */
char *ml_eval_val2str(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env)
{
	*ret = ml_value_str(mprintf("%C", ml_value_chunk(value)), ml_tag_copy(value->tag));

	return NULL;
}
