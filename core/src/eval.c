#include "common.h"


/**
 * Compute a velocity from a float.
 *   @ret: Ref. The returned value.
 *   @value: The value.
 *   @env: The environment.
 *   &returns: Error.
 */
char *amp_eval_vel(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env)
{
        if(value->type == ml_value_num_e)
                *ret = ml_value_num(value->data.num / (double)UINT16_MAX);
        else
                return mprintf("%C: Type error. Expected number.", ml_tag_chunk(&value->tag));

        return NULL;
}
