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
        if(value->type == ml_value_num_v)
                *ret = ml_value_num(value->data.num / (double)UINT16_MAX, ml_tag_copy(value->tag));
        else
                return mprintf("%C: Type error. Expected integer.", ml_tag_chunk(&value->tag));

        return NULL;
}

/**
 * Compute an decibel from an amplitude.
 *   @ret: Ref. The returned value.
 *   @value: The value.
 *   @env: The environment.
 *   &returns: Error.
 */
char *amp_eval_amp2db(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env)
{
	double flt;

        if(value->type == ml_value_num_v)
                flt = value->data.num;
	else if(value->type == ml_value_flt_v)
                flt = value->data.flt;
        else
                return mprintf("%C: Type error. Expected number.", ml_tag_chunk(&value->tag));

	*ret = ml_value_flt(dsp_amp2db_d(flt), ml_tag_copy(value->tag));
        return NULL;
}

/**
 * Compute an amplitude from a decibel.
 *   @ret: Ref. The returned value.
 *   @value: The value.
 *   @env: The environment.
 *   &returns: Error.
 */
char *amp_eval_db2amp(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env)
{
	double flt;

        if(value->type == ml_value_num_v)
                flt = value->data.num;
	else if(value->type == ml_value_flt_v)
                flt = value->data.flt;
        else
                return mprintf("%C: Type error. Expected number.", ml_tag_chunk(&value->tag));

	*ret = ml_value_flt(dsp_db2amp_d(flt), ml_tag_copy(value->tag));
        return NULL;
}
