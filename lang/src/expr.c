#include "common.h"


/**
 * Create a new expression.
 *   @type: The type.
 *   @data: Consumed. The data.
 *   @tag: Consumed. The tag.
 *   &returns: The expression.
 */
struct ml_expr_t *ml_expr_new(enum ml_expr_e type, union ml_expr_u data, struct ml_tag_t tag)
{
	struct ml_expr_t *expr;

	expr = malloc(sizeof(struct ml_expr_t));
	expr->type = type;
	expr->data = data;
	expr->tag = tag;

	return expr;
}

/**
 * Copy an expression.
 *   @expr: The expression.
 *   &returns: The copy.
 */
struct ml_expr_t *ml_expr_copy(struct ml_expr_t *expr)
{
	struct ml_expr_t *copy;

	copy = malloc(sizeof(struct ml_expr_t));
	copy->type = expr->type;
	copy->tag = ml_tag_copy(expr->tag);

	switch(copy->type) {
	case ml_expr_value_v:
		copy->data.value = ml_value_copy(expr->data.value);
		break;

	case ml_expr_var_v:
		copy->data.var = strdup(expr->data.var);
		break;
		
	case ml_expr_app_v:
		copy->data.app = ml_app_copy(expr->data.app);
		break;
		
	case ml_expr_let_v:
		copy->data.let = ml_let_copy(expr->data.let);
		break;
		
	case ml_expr_cond_v:
		copy->data.cond = ml_cond_copy(expr->data.cond);
		break;
		
	case ml_expr_match_v:
		copy->data.match = ml_match_copy(expr->data.match);
		break;

	case ml_expr_tuple_v:
		copy->data.tuple = ml_tuple_copy(expr->data.tuple);
		break;

	case ml_expr_fun_v:
		copy->data.fun = ml_fun_copy(expr->data.fun);
		break;
	}

	return copy;
}

/**
 * Delete an expression.
 *   @expr: The expression.
 */
void ml_expr_delete(struct ml_expr_t *expr)
{
	switch(expr->type) {
	case ml_expr_value_v:
		ml_value_delete(expr->data.value);
		break;

	case ml_expr_var_v:
		free(expr->data.var);
		break;

	case ml_expr_app_v:
		ml_app_delete(expr->data.app);
		break;

	case ml_expr_let_v:
		ml_let_delete(expr->data.let);
		break;

	case ml_expr_cond_v:
		ml_cond_delete(expr->data.cond);
		break;

	case ml_expr_match_v:
		ml_match_delete(expr->data.match);
		break;

	case ml_expr_tuple_v:
		ml_tuple_delete(expr->data.tuple);
		break;

	case ml_expr_fun_v:
		ml_fun_delete(expr->data.fun);
		break;
	}

	ml_tag_delete(expr->tag);
	free(expr);
}


/**
 * Create a value expression.
 *   @value: Consumed. The value.
 *   @tag: Consumed. The tag.
 *   &returns: The expression.
 */
struct ml_expr_t *ml_expr_value(struct ml_value_t *value, struct ml_tag_t tag)
{
	return ml_expr_new(ml_expr_value_v, (union ml_expr_u){ .value = value }, tag);
}

/**
 * Create a variable expression.
 *   @var: Consumed. The variable identifier.
 *   @tag: Consumed. The tag.
 *   &returns: The expression.
 */
struct ml_expr_t *ml_expr_var(char *var, struct ml_tag_t tag)
{
	return ml_expr_new(ml_expr_var_v, (union ml_expr_u){ .var = var }, tag);
}

/**
 * Create an application expression.
 *   @app: Consumed. The application.
 *   @tag: Consumed. The tag.
 *   &returns: The expression.
 */
struct ml_expr_t *ml_expr_app(struct ml_app_t *app, struct ml_tag_t tag)
{
	return ml_expr_new(ml_expr_app_v, (union ml_expr_u){ .app = app }, tag);
}

/**
 * Create a let expression.
 *   @let: Consumed. The let.
 *   @tag: Consumed. The tag.
 *   &returns: The expression.
 */
struct ml_expr_t *ml_expr_let(struct ml_let_t *let, struct ml_tag_t tag)
{
	return ml_expr_new(ml_expr_let_v, (union ml_expr_u){ .let = let }, tag);
}

/**
 * Create a conditional expression.
 *   @cond: Consumed. The conditional.
 *   @tag: Consumed. The tag.
 *   &returns: The expression.
 */
struct ml_expr_t *ml_expr_cond(struct ml_cond_t *cond, struct ml_tag_t tag)
{
	return ml_expr_new(ml_expr_cond_v, (union ml_expr_u){ .cond = cond }, tag);
}

/**
 * Create a match expression.
 *   @match: Consumed. The match.
 *   @tag: Consumed. The tag.
 *   &returns: The expression.
 */
struct ml_expr_t *ml_expr_match(struct ml_match_t *match, struct ml_tag_t tag)
{
	return ml_expr_new(ml_expr_match_v, (union ml_expr_u){ .match = match }, tag);
}

/**
 * Create a tuple expression.
 *   @tuple: Consumed. The tuple.
 *   @tag: Consumed. The tag.
 *   &returns: The expression.
 */
struct ml_expr_t *ml_expr_tuple(struct ml_tuple_t *tuple, struct ml_tag_t tag)
{
	return ml_expr_new(ml_expr_tuple_v, (union ml_expr_u){ .tuple = tuple }, tag);
}

/**
 * Create a function expression.
 *   @fun: Consumed. The function.
 *   @tag: Consumed. The tag.
 *   &returns: The expression.
 */
struct ml_expr_t *ml_expr_fun(struct ml_fun_t *fun, struct ml_tag_t tag)
{
	return ml_expr_new(ml_expr_fun_v, (union ml_expr_u){ .fun = fun }, tag);
}


/**
 * Evaluate an expression.
 *   @ret: Ref. The returned value.
 *   @expr: The expresion.
 *   @env: The environment.
 *   &returns: Error.
 */
char *ml_expr_eval(struct ml_value_t **ret, struct ml_expr_t *expr, struct ml_env_t *env)
{
	*ret = NULL;

	switch(expr->type) {
	case ml_expr_value_v:
		*ret = ml_value_copy(expr->data.value);
		return NULL;
		
	case ml_expr_var_v:
		*ret = ml_env_lookup(env, expr->data.var);
		if(*ret == NULL) {
			ml_eval_f func;

			func = ml_eval_find(expr->data.var);
			if(func != NULL)
				*ret = ml_value_eval(func, ml_tag_copy(expr->tag));
			else if((*ret = ml_eval_closure(expr->data.var)) != NULL)
				ml_tag_replace(&(*ret)->tag, ml_tag_copy(expr->tag));
			else
				return mprintf("%C: Unknown variable '%s'.", ml_tag_chunk(&expr->tag), expr->data.var);
		}
		else
			*ret = ml_value_copy(*ret);

		return NULL;

	case ml_expr_app_v:
		{
#define onexit ml_value_erase(func); ml_value_erase(value); ml_env_erase(sub); *ret = NULL;
			struct ml_env_t *sub = NULL;
			struct ml_value_t *func = NULL, *value = NULL;

			chkfail(ml_expr_eval(&func, expr->data.app->left, env));
			chkfail(ml_expr_eval(&value, expr->data.app->right, env));

			if(func->type == ml_value_closure_v) {
				struct ml_pat_t *pat;

				pat = func->data.closure->pat;
				sub = ml_env_copy(func->data.closure->env);

				if(!ml_pat_match(pat, value, &sub))
					fail("%C: Pattern match between '%C' and '%C' failed.", ml_tag_chunk(&expr->tag), ml_pat_chunk(pat), ml_value_chunk(value));

				if(func->data.closure->rec != NULL)
					ml_env_add(&sub, strdup(func->data.closure->rec), ml_value_copy(func));

				if(pat->next == NULL) {
					chkfail(ml_expr_eval(ret, func->data.closure->expr, sub));
					ml_env_delete(sub);
				}
				else
					*ret = ml_value_closure(ml_closure_new(NULL, sub, ml_pat_copy(pat->next), ml_expr_copy(func->data.closure->expr)), ml_tag_copy(expr->tag));
			}
			else if(func->type == ml_value_eval_v)
				chkfail(func->data.eval(ret, value, env));
			else
				fail("%C: Cannot apply non-function.", ml_tag_chunk(&expr->tag));

			ml_value_delete(func);
			ml_value_delete(value);

			ml_tag_replace(&(*ret)->tag, ml_tag_copy(expr->tag));

			return NULL;
#undef onexit
		}

	case ml_expr_let_v:
		{
#define onexit ml_value_erase(value); ml_env_delete(env); *ret = NULL;
			struct ml_value_t *value = NULL;
			struct ml_pat_t *pat = expr->data.let->pat;

			env = ml_env_copy(env);

			if(pat->next != NULL) {
				struct ml_closure_t *closure;

				if(pat->type != ml_pat_var_v)
					fail("%C: Invalid function declaration.", ml_tag_chunk(&pat->tag));

				closure = ml_closure_new(strdup(pat->data.var), ml_env_copy(env), ml_pat_copy(pat->next), ml_expr_copy(expr->data.let->value));
				ml_env_add(&env, strdup(pat->data.var), ml_value_closure(closure, ml_tag_copy(pat->tag)));
			}
			else {
				chkfail(ml_expr_eval(&value, expr->data.let->value, env));
				if(!ml_pat_match(expr->data.let->pat, value, &env))
					fail("%C: Pattern match between '%C' and '%C' failed.", ml_tag_chunk(&expr->tag), ml_pat_chunk(expr->data.let->pat), ml_value_chunk(value));
			}

			chkfail(ml_expr_eval(ret, expr->data.let->expr, env));

			ml_value_erase(value);
			ml_env_delete(env);

			return NULL;
#undef onexit
		}

	case ml_expr_cond_v:
		{
#define onexit ml_value_erase(value); *ret = NULL;
			struct ml_value_t *value = NULL;

			chkfail(ml_expr_eval(&value, expr->data.cond->eval, env));
			if(value->type != ml_value_bool_v)
				fail("%C from %C: Conditional must evaluate to a boolean.", ml_tag_chunk(&expr->data.cond->eval->tag), ml_tag_chunk(&value->tag));

			chkfail(ml_expr_eval(ret, value->data.flag ? expr->data.cond->ontrue : expr->data.cond->onfalse, env));
			ml_value_delete(value);

			return NULL;
#undef onexit
		}

	case ml_expr_match_v:
		{
#define onexit ml_value_erase(value); ml_env_erase(sub); *ret = NULL;
			struct ml_with_t *with;
			struct ml_env_t *sub = NULL;
			struct ml_value_t *value = NULL;

			chkfail(ml_expr_eval(&value, expr->data.cond->eval, env));

			for(with = expr->data.match->with; with != NULL; with = with->next) {
				sub = ml_env_copy(env);

				if(ml_pat_match(with->pat, value, &sub)) {
					chkfail(ml_expr_eval(ret, with->expr, sub));
					ml_env_delete(sub);
					ml_value_delete(value);

					return NULL;
				}

				ml_env_delete(sub);
			}

			fail("Match failed.");
#undef onexit
		}

	case ml_expr_tuple_v:
		{
#define onexit ml_list_delete(list); *ret = NULL;
			struct ml_list_t *list;
			struct ml_elem_t *elem;

			list = ml_list_new();

			for(elem = expr->data.tuple->head; elem != NULL; elem = elem->next) {
				chkfail(ml_expr_eval(ret, elem->expr, env));
				ml_list_append(list, *ret);
			}

			*ret = ml_value_tuple(list, ml_tag_copy(expr->tag));

			return NULL;
#undef onexit
		}

	case ml_expr_fun_v:
		{
			struct ml_closure_t *closure;

			closure = ml_closure_new(NULL, ml_env_copy(env), ml_pat_copy(expr->data.fun->pat), ml_expr_copy(expr->data.fun->expr));
			*ret = ml_value_closure(closure, ml_tag_copy(expr->tag));

			return NULL;
		}
	}

	fatal("Invalid expression type.");
}


/**
 * Create an application.
 *   @left: Consumed. The left or function expression.
 *   @right: Consumed. The right or value expression.
 *   &returns: The application.
 */
struct ml_app_t *ml_app_new(struct ml_expr_t *left, struct ml_expr_t *right)
{
	struct ml_app_t *app;

	app = malloc(sizeof(struct ml_app_t));
	*app = (struct ml_app_t){ left, right };

	return app;
}

/**
 * Copy an application.
 *   @app: The application.
 *   &returns: The copy.
 */
struct ml_app_t *ml_app_copy(struct ml_app_t *app)
{
	return ml_app_new(ml_expr_copy(app->left), ml_expr_copy(app->right));
}

/**
 * Delete an application.
 *   @app: The application.
 */
void ml_app_delete(struct ml_app_t *app)
{
	ml_expr_delete(app->left);
	ml_expr_delete(app->right);
	free(app);
}


/**
 * Create a conditional.
 *   @eval: Consumed. The evalution expression.
 *   @ontrue: Consumed. The on true expression.
 *   @onfalse: Consumed. The on false expression.
 *   &returns: The conditional.
 */
struct ml_cond_t *ml_cond_new(struct ml_expr_t *eval, struct ml_expr_t *ontrue, struct ml_expr_t *onfalse)
{
	struct ml_cond_t *cond;

	cond = malloc(sizeof(struct ml_cond_t));
	*cond = (struct ml_cond_t){ eval, ontrue, onfalse };

	return cond;
}

/**
 * Copy a conditional.
 *   @cond: The original conditional.
 *   &returns: The copied conditional.
 */
struct ml_cond_t *ml_cond_copy(struct ml_cond_t *cond)
{
	return ml_cond_new(ml_expr_copy(cond->eval), ml_expr_copy(cond->ontrue), ml_expr_copy(cond->onfalse));
}

/**
 * Delete a conditional.
 *   @cond: The conditional.
 */
void ml_cond_delete(struct ml_cond_t *cond)
{
	ml_expr_delete(cond->eval);
	ml_expr_delete(cond->ontrue);
	ml_expr_delete(cond->onfalse);
	free(cond);
}


/**
 * Create a let.
 *   @pat: Consumed. The pattern.
 *   @value: Consumed. The value expression.
 *   @expr: Consumed. The in expression.
 *   &returns: The let.
 */
struct ml_let_t *ml_let_new(struct ml_pat_t *pat, struct ml_expr_t *value, struct ml_expr_t *expr)
{
	struct ml_let_t *let;

	let = malloc(sizeof(struct ml_let_t));
	*let = (struct ml_let_t){ pat, value, expr };

	return let;
}

/**
 * Copy a let.
 *   @let: The original let.
 *   &returns: The copied let.
 */
struct ml_let_t *ml_let_copy(struct ml_let_t *let)
{
	return ml_let_new(ml_pat_copy(let->pat), ml_expr_copy(let->value), ml_expr_copy(let->expr));
}

/**
 * Delete a let.
 *   @let: The let.
 */
void ml_let_delete(struct ml_let_t *let)
{
	ml_pat_delete(let->pat);
	ml_expr_delete(let->value);
	ml_expr_delete(let->expr);
	free(let);
}


/**
 * Create a new match.
 *   @expr: Consumed. The expression.
 *   &returns: The match.
 */
struct ml_match_t *ml_match_new(struct ml_expr_t *expr)
{
	struct ml_match_t *match;

	match = malloc(sizeof(struct ml_match_t));
	match->expr = expr;
	match->with = NULL;

	return match;
}

/**
 * Copy a match.
 *   @match: The original match.
 *   &returns: The copied match.
 */
struct ml_match_t *ml_match_copy(struct ml_match_t *match)
{
	struct ml_match_t *copy;
	struct ml_with_t *with, **ref;

	copy = ml_match_new(ml_expr_copy(match->expr));
	ref = &copy->with;

	for(with = match->with; with != NULL; with = with->next) {
		*ref = ml_with_new(ml_pat_copy(with->pat), ml_expr_copy(with->expr));
		ref = &(*ref)->next;
	}

	*ref = NULL;

	return copy;
}

/**
 * Delete a match.
 *   @match: The match.
 */
void ml_match_delete(struct ml_match_t *match)
{
	struct ml_with_t *cur, *next;

	for(cur = match->with; cur != NULL; cur = next) {
		next = cur->next;

		ml_pat_delete(cur->pat);
		ml_expr_delete(cur->expr);
		free(cur);
	}

	ml_expr_delete(match->expr);
	free(match);
}


/**
 * Create a new with.
 *   @pat: Consuemd. The pattern.
 *   @expr: Consuemd. The exprression.
 *   &returns: The with.
 */
struct ml_with_t *ml_with_new(struct ml_pat_t *pat, struct ml_expr_t *expr)
{
	struct ml_with_t *with;

	with = malloc(sizeof(struct ml_with_t));
	with->pat = pat;
	with->expr = expr;
	with->next = NULL;

	return with;
}



/**
 * Create a new tuple.
 *   &returns: The tuple.
 */
struct ml_tuple_t *ml_tuple_new(void)
{
	struct ml_tuple_t *tuple;

	tuple = malloc(sizeof(struct ml_tuple_t));
	tuple->head = tuple->tail = NULL;
	tuple->len = 0;

	return tuple;
}

/**
 * Create a tuple from a list.
 *   @expr, ...: The expression list.
 *   &returns: The tuple.
 */
struct ml_tuple_t *ml_tuple_newl(struct ml_expr_t *expr, ...)
{
	va_list args;
	struct ml_tuple_t *tuple;

	tuple = ml_tuple_new();

	va_start(args, expr);

	while(expr != NULL) {
		ml_tuple_append(tuple, expr);
		expr = va_arg(args, struct ml_expr_t *);
	}

	va_end(args);

	return tuple;
}

/**
 * Copy a tuple.
 *   @tuple: The tuple.
 *   &returns: The copy.
 */
struct ml_tuple_t *ml_tuple_copy(struct ml_tuple_t *tuple)
{
	struct ml_tuple_t *copy;
	struct ml_elem_t *elem;

	copy = ml_tuple_new();

	for(elem = tuple->head; elem != NULL; elem = elem->next)
		ml_tuple_append(copy, ml_expr_copy(elem->expr));

	return copy;
}

/**
 * Delete a tuple.
 *   @tuple: The tuple.
 */
void ml_tuple_delete(struct ml_tuple_t *tuple)
{
	struct ml_elem_t *cur, *next;

	for(cur = tuple->head; cur != NULL; cur = next) {
		next = cur->next;

		ml_expr_delete(cur->expr);
		free(cur);
	}

	free(tuple);
}


/**
 * Prepend an expression onto the tuple.
 *   @tuple: The tuple.
 *   @expr: Consumed. The expression.
 */
void ml_tuple_prepend(struct ml_tuple_t *tuple, struct ml_expr_t *expr)
{
	struct ml_elem_t *elem;

	elem = malloc(sizeof(struct ml_elem_t));
	elem->expr = expr;
	elem->next = tuple->head;
	elem->prev = NULL;
	*(tuple->head ? &tuple->head->prev : &tuple->tail) = elem;

	tuple->len++;
	tuple->head = elem;
}

/**
 * Append an expression onto the tuple.
 *   @tuple: The tuple.
 *   @expr: Consumed. The expression.
 */
void ml_tuple_append(struct ml_tuple_t *tuple, struct ml_expr_t *expr)
{
	struct ml_elem_t *elem;

	elem = malloc(sizeof(struct ml_elem_t));
	elem->expr = expr;
	elem->prev = tuple->tail;
	elem->next = NULL;
	*(tuple->tail ? &tuple->tail->next : &tuple->head) = elem;

	tuple->len++;
	tuple->tail = elem;
}


/**
 * Create a function.
 *   @pat: Consumed. The pattern.
 *   @expr: Consumed. The in expression.
 *   &returns: The function.
 */
struct ml_fun_t *ml_fun_new(struct ml_pat_t *pat, struct ml_expr_t *expr)
{
	struct ml_fun_t *fun;

	fun = malloc(sizeof(struct ml_fun_t));
	*fun = (struct ml_fun_t){ pat, expr };

	return fun;
}

/**
 * Copy a function.
 *   @fun: The original function.
 *   &returns: The copied function.
 */
struct ml_fun_t *ml_fun_copy(struct ml_fun_t *fun)
{
	return ml_fun_new(ml_pat_copy(fun->pat), ml_expr_copy(fun->expr));
}

/**
 * Defune a function.
 *   @fun: The function.
 */
void ml_fun_delete(struct ml_fun_t *fun)
{
	ml_pat_delete(fun->pat);
	ml_expr_delete(fun->expr);
	free(fun);
}
