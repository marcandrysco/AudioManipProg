#include "../common.h"


/**
 * Evaluate a list creation.
 *   @ret: @ref: The return value.
 *   @value: The value.
 *   @env: The environment.
 *   &returns: Error.
 */
char *ml_eval_list(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env)
{
	*ret = ml_value_list(ml_list_copy(value->data.list), ml_tag_copy(value->tag));
	return NULL;
}

/**
 * Evaluate a cons.
 *   @ret: The return value.
 *   @value: The value.
 *   @env: The environment.
 *   &returns: Error.
 */
char *ml_eval_cons(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env)
{
#define onexit
#define error() fail("%C: Type error. Expected '(Value,List).", ml_tag_chunk(&value->tag))
	struct ml_list_t *tuple, *list;

	if(value->type != ml_value_tuple_v)
		error();

	tuple = value->data.list;
	if(tuple->len != 2)
		error();
	else if(tuple->tail->value->type != ml_value_list_v)
		error();

	list = ml_list_copy(tuple->tail->value->data.list);
	ml_list_prepend(list, ml_value_copy(tuple->head->value));
	*ret = ml_value_list(list, ml_tag_copy(value->tag));

	return NULL;
#undef onexit
#undef error
}

/**
 * Evaluate a concat.
 *   @ret: The return value.
 *   @value: The value.
 *   @env: The environment.
 *   &returns: Error.
 */
char *ml_eval_concat(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env)
{
#define onexit
#define error() fail("%C: Type error. Expected '(List,List).", ml_tag_chunk(&value->tag))
	struct ml_list_t *tuple, *list;

	if(value->type != ml_value_tuple_v)
		error();

	tuple = value->data.list;
	if(tuple->len != 2)
		error();

	if((tuple->head->value->type == ml_value_list_v) && (tuple->tail->value->type == ml_value_list_v)) {
		list = ml_list_merge(ml_list_copy(tuple->head->value->data.list), ml_list_copy(tuple->tail->value->data.list));
		*ret = ml_value_list(list, ml_tag_copy(value->tag));
	}
	else if((tuple->head->value->type == ml_value_str_v) && (tuple->tail->value->type == ml_value_str_v))
		*ret = ml_value_str(mprintf("%s%s", tuple->head->value->data.str, tuple->tail->value->data.str), ml_tag_copy(value->tag));
	else 
		error();

	return NULL;
#undef onexit
#undef error
}


/**
 * Evaluate a sequence.
 *   @ret: The return value.
 *   @value: The value.
 *   @env: The environment.
 *   &returns: Error.
 */
char *ml_eval_seq(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env)
{
	unsigned int i;
	struct ml_list_t *list;

	if(value->type != ml_value_num_v)
		return mprintf("%C: Function 'seq' expected type 'Int'.", ml_tag_chunk(&value->tag));
	else if(value->data.num < 0)
		return mprintf("%C: Function 'seq' requires positive integer.", ml_tag_chunk(&value->tag));

	list = ml_list_new();

	for(i = 0; i < value->data.num; i++)
		ml_list_append(list, ml_value_num(i, ml_tag_copy(value->tag)));

	*ret = ml_value_list(list, ml_tag_copy(value->tag));

	return NULL;
}

/**
 * Evaluate a floating-point sequence.
 *   @ret: The return value.
 *   @value: The value.
 *   @env: The environment.
 *   &returns: Error.
 */
char *ml_eval_seqf(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env)
{
	double flt[2];
	unsigned int i;
	struct ml_list_t *list;

	if(!ml_get_flt(flt, value, 2))
		return mprintf("%C: Function 'seqf' expected type '(float,float)'.", ml_tag_chunk(&value->tag));

	list = ml_list_new();

	flt[1] -= 0.01 * flt[0];
	for(i = 0; i * flt[0] < flt[1]; i++)
		ml_list_append(list, ml_value_flt(i * flt[0], ml_tag_copy(value->tag)));

	*ret = ml_value_list(list, ml_tag_copy(value->tag));

	return NULL;
}


/**
 * Evaluate a map.
 *   @ret: The return value.
 *   @value: The value.
 *   @env: The environment.
 *   &returns: Error.
 */
char *ml_eval_map(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env)
{
#define onexit ml_list_delete(list); ml_env_delete(sub);
#define error() return mprintf("%C: Type error. Expected '(Fun,List).", ml_tag_chunk(&value->tag))
	struct ml_link_t *link;
	struct ml_closure_t *closure;
	struct ml_list_t *tuple, *list;

	if(value->type != ml_value_tuple_v)
		error();

	tuple = value->data.list;
	if(tuple->len != 2)
		error();

	if((tuple->head->value->type != ml_value_closure_v) || (tuple->tail->value->type != ml_value_list_v))
		error();

	list = ml_list_new();
	closure = tuple->head->value->data.closure;

	for(link = tuple->tail->value->data.list->head; link != NULL; link = link->next) {
		struct ml_env_t *sub;
		struct ml_value_t *value;

		sub = ml_env_copy(closure->env);
		if(!ml_pat_match(closure->pat, link->value, &sub))
			fail("%C: Failed to match pattern between %C and %C.", ml_tag_chunk(&value->tag), ml_tag_chunk(&closure->pat->tag), ml_tag_chunk(&link->value->tag));

		if(closure->pat->next)
			value = ml_value_closure(ml_closure_new(NULL, ml_env_copy(sub), ml_pat_copy(closure->pat->next), ml_expr_copy(closure->expr)), ml_tag_copy(value->tag));
		else
			chkfail(ml_expr_eval(&value, closure->expr, sub));

		ml_env_delete(sub);
		ml_list_append(list, value);
	}

	*ret = ml_value_list(list, ml_tag_copy(value->tag));
#undef onexit
#undef error
	return NULL;
}

/**
 * Evaluate a fold right.
 *   @ret: The return value.
 *   @value: The value.
 *   @env: The environment.
 *   &returns: Error.
 */
char *ml_eval_foldr(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env)
{
#define onexit ml_env_delete(sub); ml_value_erase(accum);
#define error() return mprintf("%C: Type error. Expected '(Fun,Value,List).", ml_tag_chunk(&value->tag))
	struct ml_link_t *link;
	struct ml_list_t *tuple;
	struct ml_value_t *accum;
	struct ml_closure_t *closure;

	if(value->type != ml_value_tuple_v)
		error();

	tuple = value->data.list;
	if(tuple->len != 3)
		error();

	if((tuple->head->value->type != ml_value_closure_v) || (tuple->tail->value->type != ml_value_list_v))
		error();

	accum = ml_value_copy(tuple->head->next->value);
	closure = tuple->head->value->data.closure;

	for(link = tuple->tail->value->data.list->tail; link != NULL; link = link->prev) {
		struct ml_env_t *sub;

		sub = ml_env_copy(closure->env);

		if(closure->pat->next == NULL)
			return mprintf("%C: Type error. Fold function must take two inputs.", ml_tag_chunk(&value->tag));

		if(!ml_pat_match(closure->pat, link->value, &sub))
			fail("%C: Failed to match pattern between %C and %C.", ml_tag_chunk(&value->tag), ml_tag_chunk(&closure->pat->tag), ml_tag_chunk(&link->value->tag));

		if(!ml_pat_match(closure->pat->next, accum, &sub))
			fail("%C: Failed to match pattern between %C and %C.", ml_tag_chunk(&value->tag), ml_tag_chunk(&closure->pat->tag), ml_tag_chunk(&link->value->tag));

		ml_value_delete(accum);
		accum = NULL;

		if(closure->pat->next->next)
			accum = ml_value_closure(ml_closure_new(NULL, ml_env_copy(sub), ml_pat_copy(closure->pat->next->next), ml_expr_copy(closure->expr)), ml_tag_copy(value->tag));
		else
			chkfail(ml_expr_eval(&accum, closure->expr, sub));

		ml_env_delete(sub);
	}

	*ret = accum;

	return NULL;
#undef onexit
#undef error
}
