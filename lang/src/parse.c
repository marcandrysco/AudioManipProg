#include "common.h"

/*
 * local declarations
 */
static char *parse_pat(struct ml_pat_t **pat, struct ml_token_t **token, struct ml_env_t *env);
static char *parse_pat_cons(struct ml_pat_t **ret, struct ml_token_t **token, struct ml_env_t *env);
static char *parse_pat_value(struct ml_pat_t **pat, struct ml_token_t **token, struct ml_env_t *env);
static char *parse_pat_tuple(struct ml_pat_t **pat, struct ml_token_t **token, struct ml_env_t *env);

static char *parse_expr(struct ml_expr_t **expr, struct ml_token_t **token, struct ml_env_t *env);
static char *parse_expr_concat(struct ml_expr_t **expr, struct ml_token_t **token, struct ml_env_t *env);
static char *parse_expr_cons(struct ml_expr_t **expr, struct ml_token_t **token, struct ml_env_t *env);
static char *parse_expr_app(struct ml_expr_t **expr, struct ml_token_t **token, struct ml_env_t *env);
static char *parse_expr_cmp(struct ml_expr_t **expr, struct ml_token_t **token, struct ml_env_t *env);
static char *parse_expr_addsub(struct ml_expr_t **expr, struct ml_token_t **token, struct ml_env_t *env);
static char *parse_expr_muldiv(struct ml_expr_t **expr, struct ml_token_t **token, struct ml_env_t *env);
static char *parse_expr_unary(struct ml_expr_t **ret, struct ml_token_t **token, struct ml_env_t *env);
static char *parse_expr_value(struct ml_expr_t **ret, struct ml_token_t **token, struct ml_env_t *env);
static char *parse_expr_tuple(struct ml_expr_t **ret, struct ml_token_t **token, struct ml_env_t *env);
static char *parse_expr_list(struct ml_expr_t **ret, struct ml_token_t **token, struct ml_env_t *env);


/**
 * Parse a file from a path.
 *   @env: The environment pointer.
 *   @path: The path.
 *   &returns: Error.
 */
char *ml_parse_file(struct ml_env_t **env, const char *path)
{
#define onexit ml_token_delete(token);
	struct ml_token_t *token = NULL;

	chkfail(ml_token_load(&token, path));
	ml_token_strip(&token);
	chkfail(ml_parse_top(env, token, path));
	ml_token_delete(token);

	return NULL;
#undef onexit
}

/**
 * Parse the top of a file.
 *   @env: The environment pointer.
 *   @token: The token.
 *   @path: The path used for imports.
 *   &returns: Error.
 */
char *ml_parse_top(struct ml_env_t **env, struct ml_token_t *token, const char *path)
{
	while(token->id != 0) {
		if(token->id == ml_token_let_v) {
#define onexit ml_pat_erase(pat); ml_expr_erase(expr); ml_value_erase(value);
			bool suc;
			struct ml_pat_t *pat = NULL;
			struct ml_expr_t *expr = NULL;
			struct ml_value_t *value = NULL;

			token = token->next;
			chkfail(parse_pat(&pat, &token, *env));
			if(pat == NULL)
				fail("%C: Missing pattern.", ml_tag_chunk(&token->tag));

			if(token->id != '=')
				fail("%C: Missing '='.", ml_tag_chunk(&token->tag));

			token = token->next;
			chkfail(parse_expr(&expr, &token, *env));
			if(expr == NULL)
				fail("%C: Missing expression.", ml_tag_chunk(&token->tag));

			if(pat->next != NULL) {
				struct ml_closure_t *closure;

				if(pat->type != ml_pat_var_v)
					fail("%C: Invalid function declaration.", ml_tag_chunk(&pat->tag));

				closure = ml_closure_new(strdup(pat->data.var), ml_env_copy(*env), ml_pat_copy(pat->next), ml_expr_copy(expr));
				ml_env_add(env, strdup(pat->data.var), ml_value_closure(closure, ml_tag_copy(pat->tag)));
			}
			else {
				chkfail(ml_expr_eval(&value, expr, *env));

				suc = ml_pat_match(pat, value, env);
				if(!suc)
					fail("%C: Failed to match.", ml_tag_chunk(&pat->tag));

				ml_value_delete(value);
			}

			ml_pat_delete(pat);
			ml_expr_delete(expr);
#undef onexit
		}
		else if(token->id == ml_token_import_v) {
#define onexit ml_expr_erase(expr); ml_value_erase(value);
			struct ml_expr_t *expr = NULL;
			struct ml_value_t *value = NULL;

			token = token->next;
			chkfail(parse_expr(&expr, &token, *env));
			if(expr == NULL)
				fail("%C: Missing expression.", ml_tag_chunk(&token->tag));

			chkfail(ml_expr_eval(&value, expr, *env));
			if(value->type != ml_value_str_v)
				fail("%C: Import expects string value.", ml_tag_chunk(&value->tag));

			chkfail(ml_parse_file(env, value->data.str));

			ml_expr_delete(expr);
			ml_value_delete(value);
#undef onexit
		}
		else
			fatal("%C: Unexpected token '%C'. Expected statement.", ml_tag_chunk(&token->tag), ml_token_chunk(token));
	}

	return NULL;
}


/**
 * Parse a pattern.
 *   @pat: Ref. The pattern.
 *   @token: Ref. The current token.
 *   @env: The environment.
 *   &returns: Error.
 */
static char *parse_pat(struct ml_pat_t **pat, struct ml_token_t **token, struct ml_env_t *env)
{
#define onexit ml_pat_delete(head); *pat = NULL;
	struct ml_pat_t *head = *pat;

	while(true) {
		chkfail(parse_pat_cons(pat, token, env));
		if(*pat == NULL)
			break;

		pat = &(*pat)->next;
	}

	return NULL;
#undef onexit
}

/**
 * Parse cons a pattern.
 *   @pat: Ref. The returned pattern.
 *   @token: Ref. The current token.
 *   @env: The environment.
 *   &returns: Error.
 */
static char *parse_pat_cons(struct ml_pat_t **ret, struct ml_token_t **token, struct ml_env_t *env)
{
#define onexit ml_pat_erase(*ret);
	chkfail(parse_pat_value(ret, token, env));
	if((*token)->id != ml_token_cons_v)
		return NULL;

	*token = (*token)->next;
	chkfail(parse_pat_cons(&(*ret)->next, token, env));
	*ret = ml_pat_cons(*ret, ml_tag_copy((*ret)->tag));

	return NULL;
#undef onexit
}

/**
 * Parse a value pattern.
 *   @pat: Ref. The pattern.
 *   @token: Ref. The current token.
 *   @env: The environment.
 *   &returns: Error.
 */
static char *parse_pat_value(struct ml_pat_t **pat, struct ml_token_t **token, struct ml_env_t *env)
{
#define onexit ml_pat_delete(*pat); *pat = NULL;
	if((*token)->id == ml_token_id_v) {
		*pat = ml_pat_var(strdup((*token)->data.str), ml_tag_copy((*token)->tag));
		*token = (*token)->next;
	}
	else if((*token)->id == ml_token_nil_v) {
		*pat = ml_pat_value(ml_value_nil(ml_tag_copy((*token)->tag)), ml_tag_copy((*token)->tag));
		*token = (*token)->next;
	}
	else if((*token)->id == ml_token_empty_v) {
		*pat = ml_pat_value(ml_value_list(ml_list_new(), ml_tag_copy((*token)->tag)), ml_tag_copy((*token)->tag));
		*token = (*token)->next;
	}
	else if((*token)->id == '(') {
		*token = (*token)->next;
		chkfail(parse_pat_tuple(pat, token, env));

		if((*token)->id != ')')
			fail("%C: Missing ')'.", ml_tag_chunk(&(*token)->tag));

		*token = (*token)->next;
	}
	else
		*pat = NULL;

	return NULL;
#undef onexit
}

/**
 * Parse a tuple pattern.
 *   @pat: Ref. The pattern.
 *   @token: Ref. The current token.
 *   @env: The environment.
 *   &returns: Error.
 */
static char *parse_pat_tuple(struct ml_pat_t **pat, struct ml_token_t **token, struct ml_env_t *env)
{
#define onexit ml_pat_erase(head); *pat = NULL;
	struct ml_pat_t *head = NULL, **cur;

	chkfail(parse_pat_cons(pat, token, env));
	if(*pat == NULL)
		fail("%C: Missing pattern.", ml_tag_chunk(&(*token)->tag));

	if((*token)->id != ',')
		return NULL;

	head = *pat;
	cur = &head->next;

	do {
		*token = (*token)->next;
		chkfail(parse_pat(cur, token, env));
		if(*cur == NULL)
			fail("%C: Missing pattern in tuple.", ml_tag_chunk(&(*token)->tag));

		cur = &(*cur)->next;
	} while((*token)->id == ',');

	*pat = ml_pat_tuple(head, ml_tag_copy(head->tag));

	return NULL;
#undef onexit
}


/**
 * Parse an expression.
 *   @ret: Ref. The returned expression.
 *   @token: Ref. The current token.
 *   @env: The environment.
 *   &returns: Error.
 */
static char *parse_expr(struct ml_expr_t **ret, struct ml_token_t **token, struct ml_env_t *env)
{
	if((*token)->id == ml_token_let_v) {
#define onexit ml_pat_erase(pat); ml_expr_erase(value);
		struct ml_tag_t tag;
		struct ml_pat_t *pat;
		struct ml_expr_t *value = NULL;

		tag = (*token)->tag;

		*token = (*token)->next;
		chkfail(parse_pat(&pat, token, env));
		if(pat == NULL)
			fail("%C: Missing pattern.", ml_tag_chunk(&(*token)->tag));

		if((*token)->id != '=')
			fail("%C: Missing '='.", ml_tag_chunk(&(*token)->tag));

		*token = (*token)->next;
		chkfail(parse_expr(&value, token, env));
		if(value == NULL)
			fail("%C: Missing value expression.", ml_tag_chunk(&(*token)->tag));

		if((*token)->id != ml_token_in_v)
			fail("%C: Missing 'in'.", ml_tag_chunk(&(*token)->tag));

		*token = (*token)->next;
		chkfail(parse_expr(ret, token, env));
		if(*ret == NULL)
			fail("%C: Missing in expression.", ml_tag_chunk(&(*token)->tag));

		*ret = ml_expr_let(ml_let_new(pat, value, *ret), ml_tag_copy(tag));
		return NULL;
#undef onexit
	}
	else if((*token)->id == ml_token_if_v) {
#define onexit ml_expr_erase(eval); ml_expr_erase(ontrue); ml_expr_erase(onfalse);
		struct ml_tag_t tag;
		struct ml_expr_t *eval = NULL, *ontrue = NULL, *onfalse = NULL;

		tag = (*token)->tag;

		*token = (*token)->next;
		chkfail(parse_expr(&eval, token, env));
		if(eval == NULL)
			fail("%C: Missing conditional expression.", ml_tag_chunk(&(*token)->tag));

		if((*token)->id != ml_token_then_v)
			fail("%C: Missing 'then'.", ml_tag_chunk(&(*token)->tag));

		*token = (*token)->next;
		chkfail(parse_expr(&ontrue, token, env));
		if(eval == NULL)
			fail("%C: Missing true expression.", ml_tag_chunk(&(*token)->tag));

		if((*token)->id != ml_token_else_v)
			fail("%C: Missing 'else'.", ml_tag_chunk(&(*token)->tag));

		*token = (*token)->next;
		chkfail(parse_expr(&onfalse, token, env));
		if(eval == NULL)
			fail("%C: Missing false expression.", ml_tag_chunk(&(*token)->tag));

		*ret = ml_expr_cond(ml_cond_new(eval, ontrue, onfalse), ml_tag_copy(tag));

		return NULL;
#undef onexit
	}
	else if((*token)->id == ml_token_match_v) {
#define onexit if(match != NULL) ml_match_delete(match); ml_pat_erase(pat); ml_expr_erase(expr);
		struct ml_tag_t tag;
		struct ml_match_t *match = NULL;
		struct ml_with_t **with;
		struct ml_pat_t *pat = NULL;
		struct ml_expr_t *expr = NULL;

		tag = (*token)->tag;
		*token = (*token)->next;
		chkfail(parse_expr(&expr, token, env));
		if(expr == NULL)
			fail("%C: Missing match expression.", ml_tag_chunk(&(*token)->tag));

		match = ml_match_new(expr);
		with = &match->with;

		if((*token)->id != ml_token_with_v)
			fail("%C: Missing 'else'.", ml_tag_chunk(&(*token)->tag));

		*token = (*token)->next;
		while((*token)->id == '|') {
			pat = NULL;
			expr = NULL;

			*token = (*token)->next;
			chkfail(parse_pat(&pat, token, env));
			if(pat == NULL)
				fail("%C: Missing pattern.", ml_tag_chunk(&(*token)->tag));

			if((*token)->id != ml_token_arrow_v)
				fail("%C: Missing '->' in match statement.", ml_tag_chunk(&(*token)->tag));

			*token = (*token)->next;
			chkfail(parse_expr(&expr, token, env));
			if(expr == NULL)
				fail("%C: Missing expression.", ml_tag_chunk(&(*token)->tag));

			*with = ml_with_new(pat, expr);
			with = &(*with)->next;
		}

		*with = NULL;
		*ret = ml_expr_match(match, ml_tag_copy(tag));

		return NULL;
#undef onexit
	}
	else if((*token)->id == ml_token_fun_v) {
#define onexit ml_pat_erase(pat); ml_expr_erase(expr);
		struct ml_pat_t *pat = NULL;
		struct ml_expr_t *expr = NULL;

		*token = (*token)->next;
		chkfail(parse_pat(&pat, token, env));
		if((*token)->id != ml_token_arrow_v)
			fail("%C: Missing '->'.", ml_tag_chunk(&(*token)->tag));

		*token = (*token)->next;
		chkfail(parse_expr(&expr, token, env));

		*ret = ml_expr_fun(ml_fun_new(pat, expr), ml_tag_copy(pat->tag));

		return NULL;
#undef onexit
	}
	else
		return parse_expr_concat(ret, token, env);
}

/**
 * Parse a concat expression.
 *   @expr: Ref. The expression.
 *   @token: Ref. The current token.
 *   @env: The environment.
 *   &returns: Error.
 */
static char *parse_expr_concat(struct ml_expr_t **expr, struct ml_token_t **token, struct ml_env_t *env)
{
#define onexit
	struct ml_tag_t tag;
	struct ml_expr_t *right, *func, *value;

	chkfail(parse_expr_cons(expr, token, env));
	if(*expr == NULL)
		return NULL;

	while(true) {
		if((*token)->id != ml_token_concat_v)
			break;

		tag = (*token)->tag;
		*token = (*token)->next;
		chkfail(parse_expr_cons(&right, token, env));
		if(right == NULL)
			fail("%C: Missing right-hand expression.", ml_tag_chunk(&(*token)->tag));

		func = ml_expr_value(ml_value_eval(ml_eval_concat, ml_tag_copy(tag)), ml_tag_copy(tag));
		value = ml_expr_tuple(ml_tuple_newl(*expr, right, NULL), ml_tag_copy((*expr)->tag));
		*expr = ml_expr_app(ml_app_new(func, value), ml_tag_copy((*expr)->tag));
	}

#undef onexit
	return NULL;
}

/**
 * Parse a cons expression.
 *   @expr: Ref. The expression.
 *   @token: Ref. The current token.
 *   @env: The environment.
 *   &returns: Error.
 */
static char *parse_expr_cons(struct ml_expr_t **expr, struct ml_token_t **token, struct ml_env_t *env)
{
#define onexit
	struct ml_tag_t tag;
	struct ml_expr_t *right, *func, *value;

	chkfail(parse_expr_cmp(expr, token, env));
	if(*expr == NULL)
		return NULL;

	while(true) {
		if((*token)->id != ml_token_cons_v)
			break;

		tag = (*token)->tag;
		*token = (*token)->next;
		chkfail(parse_expr_cons(&right, token, env));
		if(right == NULL)
			fail("%C: Missing right-hand expression.", ml_tag_chunk(&(*token)->tag));

		func = ml_expr_value(ml_value_eval(ml_eval_cons, ml_tag_copy(tag)), ml_tag_copy(tag));
		value = ml_expr_tuple(ml_tuple_newl(*expr, right, NULL), ml_tag_copy((*expr)->tag));
		*expr = ml_expr_app(ml_app_new(func, value), ml_tag_copy((*expr)->tag));
	}

#undef onexit
	return NULL;
}

/**
 * Parse a comparison expression.
 *   @expr: Ref. The expression.
 *   @token: Ref. The current token.
 *   @env: The environment.
 *   &returns: Error.
 */
static char *parse_expr_cmp(struct ml_expr_t **expr, struct ml_token_t **token, struct ml_env_t *env)
{
#define onexit
	ml_eval_f eval;
	struct ml_tag_t tag;
	struct ml_expr_t *right, *func, *value;

	chkfail(parse_expr_addsub(expr, token, env));
	if(*expr == NULL)
		return NULL;

	while(true) {
		tag = (*token)->tag;

		if((*token)->id == '>')
			eval = ml_eval_gt;
		else if((*token)->id == '<')
			eval = ml_eval_lt;
		else if((*token)->id == ml_token_lte_v)
			eval = ml_eval_lte;
		else if((*token)->id == ml_token_gte_v)
			eval = ml_eval_gte;
		else
			break;

		*token = (*token)->next;
		chkfail(parse_expr_addsub(&right, token, env));
		if(right == NULL)
			fail("%C: Missing right-hand expression.", ml_tag_chunk(&(*token)->tag));

		func = ml_expr_value(ml_value_eval(eval, ml_tag_copy(tag)), ml_tag_copy(tag));
		value = ml_expr_tuple(ml_tuple_newl(*expr, right, NULL), ml_tag_copy((*expr)->tag));
		*expr = ml_expr_app(ml_app_new(func, value), ml_tag_copy((*expr)->tag));
	}

#undef onexit
	return NULL;
}

/**
 * Parse an add or subtract expression.
 *   @expr: Ref. The expression.
 *   @token: Ref. The current token.
 *   @env: The environment.
 *   &returns: Error.
 */
static char *parse_expr_addsub(struct ml_expr_t **expr, struct ml_token_t **token, struct ml_env_t *env)
{
#define onexit
	ml_eval_f eval;
	struct ml_tag_t tag;
	struct ml_expr_t *right, *func, *value;

	chkfail(parse_expr_muldiv(expr, token, env));
	if(*expr == NULL)
		return NULL;

	while(true) {
		tag = (*token)->tag;

		if((*token)->id == '+')
			eval = ml_eval_add;
		else if((*token)->id == '-')
			eval = ml_eval_sub;
		else
			break;

		*token = (*token)->next;
		chkfail(parse_expr_muldiv(&right, token, env));
		if(right == NULL)
			fail("%C: Missing right-hand expression.", ml_tag_chunk(&(*token)->tag));

		func = ml_expr_value(ml_value_eval(eval, ml_tag_copy(tag)), ml_tag_copy(tag));
		value = ml_expr_tuple(ml_tuple_newl(*expr, right, NULL), ml_tag_copy((*expr)->tag));
		*expr = ml_expr_app(ml_app_new(func, value), ml_tag_copy((*expr)->tag));
	}

#undef onexit
	return NULL;
}

/**
 * Parse a multiply or divide expression.
 *   @expr: Ref. The expression.
 *   @token: Ref. The current token.
 *   @env: The environment.
 *   &returns: Error.
 */
static char *parse_expr_muldiv(struct ml_expr_t **expr, struct ml_token_t **token, struct ml_env_t *env)
{
#define onexit
	ml_eval_f eval;
	struct ml_tag_t tag;
	struct ml_expr_t *right, *func, *value;

	chkfail(parse_expr_unary(expr, token, env));
	if(*expr == NULL)
		return NULL;

	while(true) {
		tag = (*token)->tag;

		if((*token)->id == '*')
			eval = ml_eval_mul;
		else if((*token)->id == '/')
			eval = ml_eval_div;
		else if((*token)->id == '%')
			eval = ml_eval_mod;
		else
			break;

		*token = (*token)->next;
		chkfail(parse_expr_unary(&right, token, env));
		if(right == NULL)
			fail("%C: Missing right-hand expression.", ml_tag_chunk(&(*token)->tag));

		func = ml_expr_value(ml_value_eval(eval, ml_tag_copy(tag)), ml_tag_copy(tag));
		value = ml_expr_tuple(ml_tuple_newl(*expr, right, NULL), ml_tag_copy((*expr)->tag));
		*expr = ml_expr_app(ml_app_new(func, value), ml_tag_copy((*expr)->tag));
	}

#undef onexit
	return NULL;
}

/**
 * Parse a unary expression.
 *   @ret: Ref. The return expression.
 *   @token: Ref. The current token.
 *   @env: The environment.
 *   &returns: Error.
 */
static char *parse_expr_unary(struct ml_expr_t **ret, struct ml_token_t **token, struct ml_env_t *env)
{
#define onexit
	if((*token)->id == '-') {
		struct ml_tag_t tag = (*token)->tag;

		*token = (*token)->next;
		chkfail(parse_expr_app(ret, token, env));
		if(*ret == NULL)
			fail("%C: Missing expression to negate.", ml_tag_chunk(&(*token)->tag));

		*ret = ml_expr_app(ml_app_new(ml_expr_value(ml_value_eval(ml_eval_neg, ml_tag_copy(tag)), ml_tag_copy(tag)), *ret), ml_tag_copy(tag));
	}
	else if((*token)->id == '+') {
		*token = (*token)->next;
		chkfail(parse_expr_app(ret, token, env));
	}
	else
		chkfail(parse_expr_app(ret, token, env));

	return NULL;
#undef onexit
}

/**
 * Parse an application expression.
 *   @expr: Ref. The expression.
 *   @token: Ref. The current token.
 *   @env: The environment.
 *   &returns: Error.
 */
static char *parse_expr_app(struct ml_expr_t **expr, struct ml_token_t **token, struct ml_env_t *env)
{
#define onexit ml_expr_erase(*expr); *expr = NULL;
	struct ml_expr_t *next;

	*expr = NULL;
	chkfail(parse_expr_value(expr, token, env));
	if(*expr == NULL)
		return NULL;

	while(true) {
		chkfail(parse_expr_value(&next, token, env));
		if(next == NULL)
			return NULL;

		*expr = ml_expr_app(ml_app_new(*expr, next), ml_tag_copy((*expr)->tag));
	}

	return NULL;
#undef onexit
}

/**
 * Parse an value expression.
 *   @expr: Ref. The expression.
 *   @token: Ref. The current token.
 *   @env: The environment.
 *   &returns: Error.
 */
static char *parse_expr_value(struct ml_expr_t **expr, struct ml_token_t **token, struct ml_env_t *env)
{
#define onexit ml_expr_erase(*expr); *expr = NULL;
	struct ml_tag_t tag = (*token)->tag;

	*expr = NULL;

	if((*token)->id == ml_token_id_v) {
		*expr = ml_expr_var(strdup((*token)->data.str), ml_tag_copy(tag));
		*token = (*token)->next;
	}
	else if((*token)->id == ml_token_num_v) {
		struct ml_value_t *value;

		value = ml_value_num((*token)->data.num, ml_tag_copy(tag));
		*expr = ml_expr_value(value, ml_tag_copy(tag));
		*token = (*token)->next;
	}
	else if((*token)->id == ml_token_flt_v) {
		struct ml_value_t *value;

		value = ml_value_flt((*token)->data.flt, ml_tag_copy(tag));
		*expr = ml_expr_value(value, ml_tag_copy(tag));
		*token = (*token)->next;
	}
	else if((*token)->id == ml_token_str_v) {
		struct ml_value_t *value;

		value = ml_value_str(strdup((*token)->data.str), ml_tag_copy(tag));
		*expr = ml_expr_value(value, ml_tag_copy(tag));
		*token = (*token)->next;
	}
	else if((*token)->id == ml_token_nil_v) {
		*expr = ml_expr_value(ml_value_nil(ml_tag_copy(tag)), ml_tag_copy(tag));
		*token = (*token)->next;
	}
	else if((*token)->id == ml_token_empty_v) {
		*expr = ml_expr_value(ml_value_list(ml_list_new(), ml_tag_copy(tag)), ml_tag_copy(tag));
		*token = (*token)->next;
	}
	else if((*token)->id == ml_token_true_v) {
		*expr = ml_expr_value(ml_value_bool(true, ml_tag_copy(tag)), ml_tag_copy(tag));
		*token = (*token)->next;
	}
	else if((*token)->id == ml_token_false_v) {
		*expr = ml_expr_value(ml_value_bool(false, ml_tag_copy(tag)), ml_tag_copy(tag));
		*token = (*token)->next;
	}
	else if((*token)->id == '(')
		chkfail(parse_expr_tuple(expr, token, env));
	else if((*token)->id == '[')
		chkfail(parse_expr_list(expr, token, env));
	else
		*expr = NULL;

	return NULL;
#undef onexit
}

/**
 * Parse a tuple expression.
 *   @ret: Ref. The expression.
 *   @token: Ref. The current token.
 *   @env: The environment.
 *   &returns: Error.
 */
static char *parse_expr_tuple(struct ml_expr_t **ret, struct ml_token_t **token, struct ml_env_t *env)
{
#define onexit ml_expr_erase(*ret); ml_tuple_erase(tuple); *ret = NULL;
	struct ml_tuple_t *tuple = NULL;

	if((*token)->id != '(')
		return NULL;

	*token = (*token)->next;
	chkfail(parse_expr(ret, token, env));
	if(*ret == NULL)
		fail("%C: Missing expression.", ml_tag_chunk(&(*token)->tag));

	if((*token)->id == ',') {
		tuple = ml_tuple_new();
		ml_tuple_append(tuple, *ret);

		do {
			*ret = NULL;
			*token = (*token)->next;
			chkfail(parse_expr(ret, token, env));
			if(*ret == NULL)
				fail("%C: Missing expression in tuple.", ml_tag_chunk(&(*token)->tag));

			ml_tuple_append(tuple, *ret);
		} while((*token)->id == ',');

		if((*token)->id != ')')
			fail("%C: Missing ',' or ')'.", ml_tag_chunk(&(*token)->tag));

		*token = (*token)->next;
		*ret = ml_expr_tuple(tuple, ml_tag_copy(tuple->head->expr->tag));
	}
	else if((*token)->id == ')')
		*token = (*token)->next;
	else
		fail("%C: Missing ',' or ')'.", ml_tag_chunk(&(*token)->tag));

	return NULL;
#undef onexit
}

/**
 * Parse a list expression.
 *   @ret: Ref. The return expression.
 *   @token: Ref. The current token.
 *   @env: The environment.
 *   &returns: Error.
 */
static char *parse_expr_list(struct ml_expr_t **ret, struct ml_token_t **token, struct ml_env_t *env)
{
#define onexit ml_tuple_delete(tuple);
	struct ml_tag_t tag;
	struct ml_tuple_t *tuple;
	struct ml_expr_t *expr;

	if((*token)->id != '[')
		return NULL;

	tuple = ml_tuple_new();
	tag = (*token)->tag;

	while(true) {
		*token = (*token)->next;
		if((*token)->id == ']')
			break;

		chkfail(parse_expr(&expr, token, env));
		if(expr == NULL)
			fail("%C: Missing expression.", ml_tag_chunk(&(*token)->tag));

		ml_tuple_append(tuple, expr);

		if((*token)->id == ']')
			break;
		else if((*token)->id != ',')
			fail("%C: Expected ',' or ']'.", ml_tag_chunk(&(*token)->tag));
	}

	*token = (*token)->next;
	*ret = ml_expr_tuple(tuple, ml_tag_copy(tag));
	*ret = ml_expr_app(ml_app_new(ml_expr_value(ml_value_eval(ml_eval_list, ml_tag_copy(tag)), ml_tag_copy(tag)), *ret), ml_tag_copy(tag));
	return NULL;
#undef onexit
}
