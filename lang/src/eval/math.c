#include "../common.h"


/*
 * global variables
 */
struct ml_eval0_t ml_eval_table[] = {
	{ "randf", ml_eval_randf },
	{ "pow10",   ml_eval_pow },
	{ NULL,    NULL }
};


/**
 * Retrieve a tuple of two numbers.
 *   @value: Consumed. The value.
 *   @left: The left pointer.
 *   @right: The right pointer.
 */
bool ml_unpack_num2(struct ml_value_t *value, double *left, double *right)
{
	struct ml_tuple_t *tuple;

	if(value->type != ml_value_tuple_e)
		return false;

	tuple = &value->data.tuple;
	if(tuple->len != 2)
		return false;
	else if((tuple->value[0]->type != ml_value_num_e) || (tuple->value[1]->type != ml_value_num_e))
		return false;

	*left = tuple->value[0]->data.num;
	*right = tuple->value[1]->data.num;

	return true;
}


/**
 * Floating point random evaluation.
 *   @ret: Ref. The return value.
 *   @value: Consumed. The value.
 *   @env: The environment.
 *   &returns: Error.
 */
char *ml_eval_randf(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env)
{
#define onexit ml_value_delete(value);
	if(value->type != ml_value_nil_e)
		fail("Type error. Function 'randf' requires type 'Nil'.");

	ml_value_delete(value);
	
	*ret = ml_value_num((double)rand() / (double)RAND_MAX);
	return NULL;
#undef onexit
}

/**
 * Floating point power evaluation.
 *   @ret: Ref. The return value.
 *   @value: Consumed. The value.
 *   @env: The environment.
 *   &returns: Error.
 */
char *ml_eval_pow(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env)
{
#define onexit ml_value_delete(value);
	double left, right;

	if(ml_unpack_num2(value, &left, &right))
		*ret = ml_value_num(pow(left, right));
	else
		fail("Type error. Function 'pow' requires type '(float,float)'.");

	ml_value_delete(value);
	
	return NULL;
#undef onexit
}
