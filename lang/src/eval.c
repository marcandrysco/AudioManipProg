#include "common.h"


/*
 * common failure definition
 */

#undef fail
#define fail(format, ...) do { if(*err == NULL) ml_eprintf(err, format, ##__VA_ARGS__); return NULL; } while(0)
#define retfalse() do { ml_value_delete(value); return false; } while(0)


/**
 * Retrieve a tuple of two numbers.
 *   @value: Consumed. The value.
 *   @left: The left pointer.
 *   @right: The right pointer.
 */

static bool get_num2(struct ml_value_t *value, double *left, double *right)
{
	struct ml_tuple_t *tuple;

	if(value->type != ml_value_tuple_e)
		retfalse();

	tuple = &value->data.tuple;
	if(tuple->len != 2)
		retfalse();
	else if((tuple->value[0]->type != ml_value_num_e) || (tuple->value[1]->type != ml_value_num_e))
		retfalse();

	*left = tuple->value[0]->data.num;
	*right = tuple->value[1]->data.num;

	ml_value_delete(value);

	return true;
}


/**
 * Negate a value.
 *   @value: Consumed. The value.
 *   @env: The environment.
 *   @err: The error.
 *   &returns: The value or null.
 */

struct ml_value_t *ml_eval_neg(struct ml_value_t *value, struct ml_env_t *env, char **err)
{
	if(value->type != ml_value_num_e)
		fail("Negation requires a numeric input.");
	
	ml_value_delete(value);

	return ml_value_num(-value->data.num);
}

/**
 * Add two values.
 *   @value: Consumed. The value.
 *   @env: The environment.
 *   @err: The error.
 *   &returns: The value or null.
 */

struct ml_value_t *ml_eval_add(struct ml_value_t *value, struct ml_env_t *env, char **err)
{
	double left, right;

	if(!get_num2(value, &left, &right))
		fail("Type error. Addition requires type '(Num, Num)'.");

	return ml_value_num(left + right);
}

/**
 * Subtract two values.
 *   @value: Consumed. The value.
 *   @env: The environment.
 *   @err: The error.
 *   &returns: The value or null.
 */

struct ml_value_t *ml_eval_sub(struct ml_value_t *value, struct ml_env_t *env, char **err)
{
	double left, right;

	if(!get_num2(value, &left, &right))
		fail("Type error. Subtraction requires type '(Num, Num)'.");

	return ml_value_num(left - right);
}

/**
 * Multiply two values.
 *   @value: Consumed. The value.
 *   @env: The environment.
 *   @err: The error.
 *   &returns: The value or null.
 */

struct ml_value_t *ml_eval_mul(struct ml_value_t *value, struct ml_env_t *env, char **err)
{
	double left, right;

	if(!get_num2(value, &left, &right))
		fail("Type error. Subtraction requires type '(Num, Num)'.");

	return ml_value_num(left * right);
}

/**
 * Divide two values.
 *   @value: Consumed. The value.
 *   @env: The environment.
 *   @err: The error.
 *   &returns: The value or null.
 */

struct ml_value_t *ml_eval_div(struct ml_value_t *value, struct ml_env_t *env, char **err)
{
	double left, right;

	if(!get_num2(value, &left, &right))
		fail("Type error. Subtraction requires type '(Num, Num)'.");

	return ml_value_num(left / right);
}

/**
 * Modulus two values.
 *   @value: Consumed. The value.
 *   @env: The environment.
 *   @err: The error.
 *   &returns: The value or null.
 */

struct ml_value_t *ml_eval_mod(struct ml_value_t *value, struct ml_env_t *env, char **err)
{
	double left, right;

	if(!get_num2(value, &left, &right))
		fail("Type error. Subtraction requires type '(Num, Num)'.");

	return ml_value_num(fmod(left, right));
}

/**
 * Compare two values as with greater than.
 *   @value: Consumed. The value.
 *   @env: The environment.
 *   @err: The error.
 *   &returns: The value or null.
 */

struct ml_value_t *ml_eval_gt(struct ml_value_t *value, struct ml_env_t *env, char **err)
{
	double left, right;

	if(!get_num2(value, &left, &right))
		fail("Type error. Comparison requires type '(Num, Num)'.");

	return ml_value_bool(left > right);
}

/**
 * Compare two values as with greater than or equal.
 *   @value: Consumed. The value.
 *   @env: The environment.
 *   @err: The error.
 *   &returns: The value or null.
 */

struct ml_value_t *ml_eval_gte(struct ml_value_t *value, struct ml_env_t *env, char **err)
{
	double left, right;

	if(!get_num2(value, &left, &right))
		fail("Type error. Comparison requires type '(Num, Num)'.");

	return ml_value_bool(left >= right);
}

/**
 * Compare two values as with less than.
 *   @value: Consumed. The value.
 *   @env: The environment.
 *   @err: The error.
 *   &returns: The value or null.
 */

struct ml_value_t *ml_eval_lt(struct ml_value_t *value, struct ml_env_t *env, char **err)
{
	double left, right;

	if(!get_num2(value, &left, &right))
		fail("Type error. Comparison requires type '(Num, Num)'.");

	return ml_value_bool(left < right);
}

/**
 * Compare two values as with less than or equal.
 *   @value: Consumed. The value.
 *   @env: The environment.
 *   @err: The error.
 *   &returns: The value or null.
 */

struct ml_value_t *ml_eval_lte(struct ml_value_t *value, struct ml_env_t *env, char **err)
{
	double left, right;

	if(!get_num2(value, &left, &right))
		fail("Type error. Comparison requires type '(Num, Num)'.");

	return ml_value_bool(left <= right);
}

/**
 * Create a list.
 *   @value: Consumed. The value.
 *   @env: The environment.
 *   @err: The error.
 *   &returns: The value or null.
 */

struct ml_value_t *ml_eval_list(struct ml_value_t *value, struct ml_env_t *env, char **err)
{
	unsigned int i;
	struct ml_list_t list;

	if(value->type != ml_value_tuple_e)
		fail("List evaluation requires tuple.");

	list = ml_list_new();

	for(i = 0; i < value->data.tuple.len; i++)
		ml_list_append(&list, ml_value_copy(value->data.tuple.value[i]));

	ml_value_delete(value);

	return ml_value_list(list);
}

/**
 * Evaluate a cons.
 *   @value: Consumed. The value.
 *   @env: The environment.
 *   @err: The error.
 *   &returns: The value or null.
 */

struct ml_value_t *ml_eval_cons(struct ml_value_t *value, struct ml_env_t *env, char **err)
{
	struct ml_list_t list;
	struct ml_tuple_t tuple;

	if(value->type != ml_value_tuple_e)
		fail("Cons evaluation requires tuple.");

	tuple = value->data.tuple;
	if(tuple.len != 2)
		fail("Cons requires a tuple of length two.");

	if(tuple.value[1]->type != ml_value_list_e)
		fail("Cannot cons onto non-list type.");

	list = ml_list_copy(tuple.value[1]->data.list);
	ml_list_prepend(&list, ml_value_copy(tuple.value[0]));

	ml_value_delete(value);

	return ml_value_list(list);
}

/**
 * Evaluate a concatenation.
 *   @value: Consumed. The value.
 *   @env: The environment.
 *   @err: The error.
 *   &returns: The value or null.
 */

struct ml_value_t *ml_eval_concat(struct ml_value_t *value, struct ml_env_t *env, char **err)
{
	struct ml_list_t list, left, right;
	struct ml_tuple_t tuple;

	if(value->type != ml_value_tuple_e)
		fail("Cons evaluation requires tuple.");

	tuple = value->data.tuple;
	if(tuple.len != 2)
		fail("Cons requires a tuple of length two.");

	if((tuple.value[0]->type == ml_value_list_e) || (tuple.value[1]->type == ml_value_list_e)) {
		list = ml_list_new();
		left = ml_list_copy(tuple.value[0]->data.list);
		right = ml_list_copy(tuple.value[1]->data.list);

		if(left.head == NULL) {
			list.head = right.head;
			list.tail = right.tail;
		}
		else if(right.head == NULL) {
			list.head = left.head;
			list.tail = left.tail;
		}
		else {
			left.tail->next = right.head;
			right.head->prev = left.tail;

			list.head = left.head;
			list.tail = right.tail;
		}

		ml_value_delete(value);

		return ml_value_list(list);
	}
	else if((tuple.value[0]->type == ml_value_str_e) || (tuple.value[1]->type == ml_value_str_e)) {
		char *str;

		str = ml_aprintf("%s%s", tuple.value[0]->data.str, tuple.value[1]->data.str);
		ml_value_delete(value);

		return ml_value_str(str);
	}
	else
		fail("Cannot concat non-list types.");
}


/**
 * Compute an exponential.
 *   @value: Consumed. The value.
 *   @env: The environment.
 *   @err: The error.
 *   &returns: The value or null.
 */

struct ml_value_t *ml_eval_exp(struct ml_value_t *value, struct ml_env_t *env, char **err)
{
	double ret;

	if(value->type != ml_value_num_e)
		fail("Type error. Exponential requires type 'Num'.");

	ret = exp(value->data.num);
	ml_value_delete(value);

	return ml_value_num(ret);
}

/**
 * Compute a logarithm.
 *   @value: Consumed. The value.
 *   @env: The environment.
 *   @err: The error.
 *   &returns: The value or null.
 */

struct ml_value_t *ml_eval_log(struct ml_value_t *value, struct ml_env_t *env, char **err)
{
	double ret;

	if(value->type != ml_value_num_e)
		fail("Type error. Logarithm requires type 'Num'.");

	ret = log(value->data.num);
	ml_value_delete(value);

	return ml_value_num(ret);
}

/**
 * Compute a floor.
 *   @value: Consumed. The value.
 *   @env: The environment.
 *   @err: The error.
 *   &returns: The value or null.
 */

struct ml_value_t *ml_eval_floor(struct ml_value_t *value, struct ml_env_t *env, char **err)
{
	double ret;

	if(value->type != ml_value_num_e)
		fail("Type error. Function 'floor' requires type 'Num'.");

	ret = floor(value->data.num);
	ml_value_delete(value);

	return ml_value_num(ret);
}

/**
 * Compute a ceiling.
 *   @value: Consumed. The value.
 *   @env: The environment.
 *   @err: The error.
 *   &returns: The value or null.
 */

struct ml_value_t *ml_eval_ceil(struct ml_value_t *value, struct ml_env_t *env, char **err)
{
	double ret;

	if(value->type != ml_value_num_e)
		fail("Type error. Function 'ceil' requires type 'Num'.");

	ret = ceil(value->data.num);
	ml_value_delete(value);

	return ml_value_num(ret);
}

/**
 * Compute a round.
 *   @value: Consumed. The value.
 *   @env: The environment.
 *   @err: The error.
 *   &returns: The value or null.
 */

struct ml_value_t *ml_eval_round(struct ml_value_t *value, struct ml_env_t *env, char **err)
{
	double ret;

	if(value->type != ml_value_num_e)
		fail("Type error. Function 'round' requires type 'Num'.");

	ret = round(value->data.num);
	ml_value_delete(value);

	return ml_value_num(ret);
}


struct ml_eval_t ml_eval_arr[] = {
	{ "fail", ml_eval_fail },
	{ "print", ml_eval_print },
	{ "i2str", ml_eval_i2str },
	{ "strlen", ml_eval_strlen },
	{ NULL, NULL }
};


/**
 * Print evaluation.
 *   @value: Consumed. The value.
 *   @env: The environment.
 *   @err: The error.
 *   &returns: The value or null.
 */

struct ml_value_t *ml_eval_print(struct ml_value_t *value, struct ml_env_t *env, char **err)
{
	if(value->type != ml_value_str_e)
		fail("Type error. Function 'print' requires type 'string'.");

	printf("%s", value->data.str);
	ml_value_delete(value);
	
	return ml_value_nil();;
}

/**
 * Fail evaluation.
 *   @value: Consumed. The value.
 *   @env: The environment.
 *   @err: The error.
 *   &returns: The value or null.
 */

struct ml_value_t *ml_eval_fail(struct ml_value_t *value, struct ml_env_t *env, char **err)
{
	if(value->type != ml_value_str_e)
		fail("Type error. Function 'fail' requires type 'string'.");

	*err = ml_aprintf("fail: %s", value->data.str);
	ml_value_delete(value);
	
	return NULL;
}

struct ml_value_t *ml_eval_i2str(struct ml_value_t *value, struct ml_env_t *env, char **err)
{
	struct ml_value_t *ret;

	if(value->type != ml_value_num_e)
		fail("Type error. Function 'i2str' requires type 'Num'.");

	ret = ml_value_str(ml_aprintf("%d", (int)value->data.num));
	ml_value_delete(value);

	return ret;
}

struct ml_value_t *ml_eval_strlen(struct ml_value_t *value, struct ml_env_t *env, char **err)
{
	struct ml_value_t *ret;

	if(value->type != ml_value_str_e)
		fail("Type error. Function 'strlen' requires type 'string'.");

	ret = ml_value_num(strlen(value->data.str));
	ml_value_delete(value);

	return ret;
}
