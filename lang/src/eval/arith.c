#include "../common.h"


bool ml_get_num2(int *num, struct ml_value_t *value)
{
	unsigned int i;
	struct ml_link_t *link;

	if(value->type != ml_value_tuple_v)
		return false;

	for(i = 0, link = value->data.list->head; i < 2; i++, link = link->next) {
		if((link == NULL) || (link->value->type != ml_value_num_v))
			return false;

		num[i] = link->value->data.num;
	}

	return link == NULL;
}

bool ml_get_flt2(double *flt, struct ml_value_t *value)
{
	unsigned int i;
	struct ml_link_t *link;

	if(value->type != ml_value_tuple_v)
		return false;

	for(i = 0, link = value->data.list->head; i < 2; i++, link = link->next) {
		if(link == NULL)
			return false;

		if(link->value->type == ml_value_flt_v)
			flt[i] = link->value->data.flt;
		else if(link->value->type == ml_value_num_v)
			flt[i] = link->value->data.num;
		else
			return false;
	}

	return link == NULL;
}


/**
 * Evaluate a negation.
 *   @ret: @Ref: The return value.
 *   @value: The value.
 *   @env: The environment.
 *   &returns: Error.
 */
char *ml_eval_neg(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env)
{
	if(value->type == ml_value_num_v)
		*ret = ml_value_num(-value->data.num, ml_tag_copy(ml_tag_null));
	else if(value->type == ml_value_flt_v)
		*ret = ml_value_flt(-value->data.flt, ml_tag_copy(ml_tag_null));
	else
		return mprintf("%C: Type error. Expected '(Num,Num)'.", ml_tag_chunk(&value->tag));

	return NULL;
}

/**
 * Evaluate an addition.
 *   @ret: @Ref: The return value.
 *   @value: The value.
 *   @env: The environment.
 *   &returns: Error.
 */
char *ml_eval_add(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env)
{
	int num[2];
	double flt[2];

	if(ml_get_num2(num, value))
		*ret = ml_value_num(num[0] + num[1], ml_tag_copy(ml_tag_null));
	else if(ml_get_flt2(flt, value))
		*ret = ml_value_flt(flt[0] + flt[1], ml_tag_copy(ml_tag_null));
	else
		return mprintf("%C: Type error. Expected '(Num,Num)'.", ml_tag_chunk(&value->tag));

	return NULL;
}

/**
 * Evaluate a subtraction.
 *   @ret: @Ref: The return value.
 *   @value: The value.
 *   @env: The environment.
 *   &returns: Error.
 */
char *ml_eval_sub(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env)
{
	int num[2];
	double flt[2];

	if(ml_get_num2(num, value))
		*ret = ml_value_num(num[0] - num[1], ml_tag_copy(ml_tag_null));
	else if(ml_get_flt2(flt, value))
		*ret = ml_value_flt(flt[0] - flt[1], ml_tag_copy(ml_tag_null));
	else
		return mprintf("%C: Type error. Expected '(Num,Num)'.", ml_tag_chunk(&value->tag));

	return NULL;
}

/**
 * Evaluate a multiplication.
 *   @ret: @Ref: The return value.
 *   @value: The value.
 *   @env: The environment.
 *   &returns: Error.
 */
char *ml_eval_mul(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env)
{
	int num[2];
	double flt[2];

	if(ml_get_num2(num, value))
		*ret = ml_value_num(num[0] * num[1], ml_tag_copy(ml_tag_null));
	else if(ml_get_flt2(flt, value))
		*ret = ml_value_flt(flt[0] * flt[1], ml_tag_copy(ml_tag_null));
	else
		return mprintf("%C: Type error. Expected '(Num,Num)'.", ml_tag_chunk(&value->tag));

	return NULL;
}

/**
 * Evaluate a division.
 *   @ret: @Ref: The return value.
 *   @value: The value.
 *   @env: The environment.
 *   &returns: Error.
 */
char *ml_eval_div(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env)
{
	int num[2];
	double flt[2];

	if(ml_get_num2(num, value))
		*ret = ml_value_num(num[0] / num[1], ml_tag_copy(ml_tag_null));
	else if(ml_get_flt2(flt, value))
		*ret = ml_value_flt(flt[0] / flt[1], ml_tag_copy(ml_tag_null));
	else
		return mprintf("%C: Type error. Expected '(Num,Num)'.", ml_tag_chunk(&value->tag));

	return NULL;
}

/**
 * Evaluate a modulus.
 *   @ret: @Ref: The return value.
 *   @value: The value.
 *   @env: The environment.
 *   &returns: Error.
 */
char *ml_eval_mod(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env)
{
	int num[2];
	double flt[2];

	if(ml_get_num2(num, value))
		*ret = ml_value_num(num[0] % num[1], ml_tag_copy(ml_tag_null));
	else if(ml_get_flt2(flt, value))
		*ret = ml_value_flt(fmod(flt[0], flt[1]), ml_tag_copy(ml_tag_null));
	else
		return mprintf("%C: Type error. Expected '(Num,Num)'.", ml_tag_chunk(&value->tag));

	return NULL;
}


/**
 * Evaluate a square root.
 *   @ret: @Ref: The return value.
 *   @value: The value.
 *   @env: The environment.
 *   &returns: Error.
 */
char *ml_eval_sqrt(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env)
{
	if(value->type == ml_value_num_v)
		*ret = ml_value_flt(sqrt(value->data.num), ml_tag_copy(ml_tag_null));
	else if(value->type == ml_value_flt_v)
		*ret = ml_value_flt(sqrt(value->data.flt), ml_tag_copy(ml_tag_null));
	else
		return mprintf("%C: Type error. Expected 'Num'.", ml_tag_chunk(&value->tag));

	return NULL;
}

/**
 * Evaluate an exponential.
 *   @ret: @Ref: The return value.
 *   @value: The value.
 *   @env: The environment.
 *   &returns: Error.
 */
char *ml_eval_exp(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env)
{
	if(value->type == ml_value_num_v)
		*ret = ml_value_flt(exp(value->data.num), ml_tag_copy(ml_tag_null));
	else if(value->type == ml_value_flt_v)
		*ret = ml_value_flt(exp(value->data.flt), ml_tag_copy(ml_tag_null));
	else
		return mprintf("%C: Type error. Expected 'Num'.", ml_tag_chunk(&value->tag));

	return NULL;
}

/**
 * Evaluate a logrithm.
 *   @ret: @Ref: The return value.
 *   @value: The value.
 *   @env: The environment.
 *   &returns: Error.
 */
char *ml_eval_log(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env)
{
	if(value->type == ml_value_num_v)
		*ret = ml_value_flt(log(value->data.num), ml_tag_copy(ml_tag_null));
	else if(value->type == ml_value_flt_v)
		*ret = ml_value_flt(log(value->data.flt), ml_tag_copy(ml_tag_null));
	else
		return mprintf("%C: Type error. Expected 'Num'.", ml_tag_chunk(&value->tag));

	return NULL;
}

/**
 * Evaluate an power.
 *   @ret: @Ref: The return value.
 *   @value: The value.
 *   @env: The environment.
 *   &returns: Error.
 */
char *ml_eval_pow(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env)
{
	int num[2];
	double flt[2];

	if(ml_get_num2(num, value))
		*ret = ml_value_flt(pow(num[0], num[1]), ml_tag_copy(ml_tag_null));
	else if(ml_get_flt2(flt, value))
		*ret = ml_value_flt(pow(flt[0], flt[1]), ml_tag_copy(ml_tag_null));
	else
		return mprintf("%C: Type error. Expected '(Num,Num)'.", ml_tag_chunk(&value->tag));

	return NULL;
}


/**
 * Evaluate a less than.
 *   @ret: @Ref: The return value.
 *   @value: The value.
 *   @env: The environment.
 *   &returns: Error.
 */
char *ml_eval_lt(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env)
{
	int num[2];
	double flt[2];

	if(ml_get_num2(num, value))
		*ret = ml_value_bool(num[0] < num[1], ml_tag_copy(ml_tag_null));
	else if(ml_get_flt2(flt, value))
		*ret = ml_value_bool(flt[0] < flt[1], ml_tag_copy(ml_tag_null));
	else
		return mprintf("%C: Type error. Expected '(Num,Num)'.", ml_tag_chunk(&value->tag));

	return NULL;
}

/**
 * Evaluate a less than or equal.
 *   @ret: @Ref: The return value.
 *   @value: The value.
 *   @env: The environment.
 *   &returns: Error.
 */
char *ml_eval_lte(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env)
{
	int num[2];
	double flt[2];

	if(ml_get_num2(num, value))
		*ret = ml_value_bool(num[0] <= num[1], ml_tag_copy(ml_tag_null));
	else if(ml_get_flt2(flt, value))
		*ret = ml_value_bool(flt[0] <= flt[1], ml_tag_copy(ml_tag_null));
	else
		return mprintf("%C: Type error. Expected '(Num,Num)'.", ml_tag_chunk(&value->tag));

	return NULL;
}

/**
 * Evaluate a greater than.
 *   @ret: @Ref: The return value.
 *   @value: The value.
 *   @env: The environment.
 *   &returns: Error.
 */
char *ml_eval_gt(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env)
{
	int num[2];
	double flt[2];

	if(ml_get_num2(num, value))
		*ret = ml_value_bool(num[0] > num[1], ml_tag_copy(ml_tag_null));
	else if(ml_get_flt2(flt, value))
		*ret = ml_value_bool(flt[0] > flt[1], ml_tag_copy(ml_tag_null));
	else
		return mprintf("%C: Type error. Expected '(Num,Num)'.", ml_tag_chunk(&value->tag));

	return NULL;
}

/**
 * Evaluate a greater than or equal.
 *   @ret: @Ref: The return value.
 *   @value: The value.
 *   @env: The environment.
 *   &returns: Error.
 */
char *ml_eval_gte(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env)
{
	int num[2];
	double flt[2];

	if(ml_get_num2(num, value))
		*ret = ml_value_bool(num[0] >= num[1], ml_tag_copy(ml_tag_null));
	else if(ml_get_flt2(flt, value))
		*ret = ml_value_bool(flt[0] >= flt[1], ml_tag_copy(ml_tag_null));
	else
		return mprintf("%C: Type error. Expected '(Num,Num)'.", ml_tag_chunk(&value->tag));

	return NULL;
}
