#include "../common.h"

/**
 * Evaluate a print statement.
 *   @ret: @Ref: The return value.
 *   @value: The value.
 *   @env: The environment.
 *   &returns: Error.
 */
char *ml_eval_print(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env)
{
	if(value->type == ml_value_str_v)
		printf("%s", value->data.str);
	else
		printf("%C", ml_value_chunk(value));

	*ret = ml_value_nil(ml_tag_copy(ml_tag_null));

	return NULL;
}

/**
 * Evaluate a print line statement.
 *   @ret: @Ref: The return value.
 *   @value: The value.
 *   @env: The environment.
 *   &returns: Error.
 */
char *ml_eval_println(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env)
{
	if(value->type == ml_value_str_v)
		printf("%s\n", value->data.str);
	else
		printf("%C\n", ml_value_chunk(value));

	*ret = ml_value_nil(ml_tag_copy(ml_tag_null));

	return NULL;
}
