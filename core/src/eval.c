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
 * Compute a value from a float.
 *   @ret: Ref. The returned value.
 *   @value: The value.
 *   @env: The environment.
 *   &returns: Error.
 */
char *amp_eval_val(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env)
{
        if(value->type == ml_value_flt_v)
                *ret = ml_value_num(UINT16_MAX * value->data.flt, ml_tag_copy(value->tag));
        else
                return mprintf("%C: Type error. Expected float.", ml_tag_chunk(&value->tag));

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


/**
 * Humanize a schedule of events.
 *   @ret: Ref. The returned value.
 *   @value: The value.
 *   @env: The environment.
 *   &returns: Error.
 */
char *amp_eval_human(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env)
{
#define onexit ml_list_delete(list);
	struct ml_link_t *link;
	struct ml_list_t *list, *tuple;
	struct ml_tag_t tag = value->tag;

	if(value->type != ml_value_list_v)
		return mprintf("%C: Type error. Expected list of events ((int,float),(int,int),int).", ml_tag_chunk(&value->tag));

	list = ml_list_new();

	for(link = value->data.list->head; link != NULL; link = link->next) {
		double beat;
		int bar, dev, key, val;

		chkfail(amp_match_unpack(link->value, "((d,f),(d,d),d)", &bar, &beat, &dev, &key, &val));

		if(beat == 0.0)
			;
		else if(fmod(beat, 2.0) == 0.0)
			val *= 0.8, beat += 0.01;
		else if(fmod(beat, 1.0) == 0.0)
			val *= 0.6, beat += 0.02;
		else if(fmod(beat, 0.5) == 0.0)
			val *= 0.4, beat += 0.03;
		else
			val *= 0.2, beat += 0.04;

		tuple = ml_list_copy(link->value->data.list);
		tuple->tail->value->data.num = val;
		tuple->head->value->data.list->tail->value->data.flt = beat;

		ml_list_append(list, ml_value_tuple(tuple, ml_tag_copy(tag)));
	}

	*ret = ml_value_list(list, ml_tag_copy(tag));
	return NULL;
#undef onexit
}
