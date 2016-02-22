#include "common.h"


/**
 * Parse from the top.
 *   @token: The head token.
 *   @env: The environment.
 *   @err: The error.
 */

void ml_parse_top(struct ml_token_t *token, struct ml_env_t *env, char **err)
{
#undef fail
#define fail(str) do { if(expr != NULL) ml_expr_delete(expr); if(*err == NULL) ml_eprintf(err, "%s:%u:%u: %s", token->tag.file, token->tag.line, token->tag.col, str); return; } while(0)

	while(token->type != ml_token_end_e) {
		const char *id;
		struct ml_value_t *value;
		struct ml_expr_t *expr = NULL;

		if(token->type != ml_token_let_e)
			fail("Missing let.");

		token = token->next;
		if(token->type != ml_token_id_e)
			fail("Invalid let. Expected identifier.");

		id = token->data.str;
		token = token->next;
		if(token->type != ml_token_equal_e)
			fail("Invalid let. Expected '='.");

		token = token->next;
		expr = ml_parse_expr(&token, err);
		if(expr == NULL)
			fail("Invalid let. Expected expression.");

		value = ml_expr_eval(expr, env, err);
		if(value != NULL)
			ml_env_add(env, strdup(id), value);

		ml_expr_delete(expr);

		if(value == NULL)
			return;
	}
}


/**
 * Parse an expression.
 *   @token: The token.
 *   @err: The error.
 *   &returns: The expression or null.
 */

struct ml_expr_t *ml_parse_expr(struct ml_token_t **token, char **err)
{
#undef fail
#define fail(str, ...) do { if(*err == NULL) ml_eprintf(err, "%s:%u:%u: " str, (*token)->tag.file, (*token)->tag.line, (*token)->tag.col, ##__VA_ARGS__); return NULL; } while(0)

	if((*token)->type == ml_token_func_e) {
		const char *id;
		struct ml_expr_t *expr;

		*token = (*token)->next;
		if((*token)->type != ml_token_id_e)
			fail("Missing function variable. %u", (*token)->type);

		id = (*token)->data.str;
		*token = (*token)->next;
		if((*token)->type != ml_token_arrow_e)
			fail("Missing '->'.");

		*token = (*token)->next;
		expr = ml_parse_expr(token, err);
		if(expr == NULL)
			fail("Missing function expression.", (*token)->type);

		return ml_expr_func(strdup(id), expr);
	}
	else {
		struct ml_expr_t *expr, *value;

		expr = ml_parse_addsub(token, err);
		if(expr == NULL)
			return expr;

		while(true) {
			value = ml_parse_addsub(token, err);
			if(value == NULL)
				return expr;

			expr = ml_expr_app(expr, value);
		}
	}
}

/**
 * Parse a set.
 *   @token: The token.
 *   @err: The error.
 *   &returns: The expression or null.
 */

struct ml_expr_t *ml_parse_set(struct ml_token_t **token, char **err)
{
#undef fail
#define fail(str, ...) do { ml_set_delete(set); if(*err == NULL) ml_eprintf(err, "%s:%u:%u: " str, (*token)->tag.file, (*token)->tag.line, (*token)->tag.col, ##__VA_ARGS__); return NULL; } while(0)

	struct ml_set_t set;
	struct ml_expr_t *expr;

	expr = ml_parse_expr(token, err);
	if(expr == NULL)
		return NULL;

	if((*token)->type != ml_token_comma_e)
		return expr;

	set = ml_set_new();
	ml_set_add(&set, expr);

	do {
		*token = (*token)->next;
		expr = ml_parse_expr(token, err);
		if(expr == NULL)
			fail("Missing expression in set.");

		ml_set_add(&set, expr);
	} while((*token)->type == ml_token_comma_e);

	return ml_expr_set(set);
}

/**
 * Parse a list.
 *   @token: The token.
 *   @err: The error.
 *   &returns: The expression or null.
 */

struct ml_expr_t *ml_parse_list(struct ml_token_t **token, char **err)
{
#undef fail
#define fail(str, ...) do { ml_set_delete(set); if(*err == NULL) ml_eprintf(err, "%s:%u:%u: " str, (*token)->tag.file, (*token)->tag.line, (*token)->tag.col, ##__VA_ARGS__); return NULL; } while(0)

	struct ml_set_t set;
	struct ml_expr_t *expr;

	set = ml_set_new();

	while((*token)->type != ml_token_rbrace_e) {
		expr = ml_parse_expr(token, err);
		if(expr == NULL)
			fail("Missing expression in list.");

		ml_set_add(&set, expr);

		if((*token)->type != ml_token_comma_e)
			break;

		*token = (*token)->next;
	} while((*token)->type == ml_token_comma_e);

	return ml_expr_app(ml_expr_value(ml_value_impl(ml_eval_list)), ml_expr_set(set));
}

/**
 * Parse an addition or subtraction expression.
 *   @token: The token.
 *   @err: The error.
 *   &returns: The expression or null.
 */

struct ml_expr_t *ml_parse_addsub(struct ml_token_t **token, char **err)
{
#undef fail
#define fail(str) do { if(left != NULL) ml_expr_delete(left); if(*err == NULL) ml_eprintf(err, "%s:%u:%u: %s", (*token)->tag.file, (*token)->tag.line, (*token)->tag.col, str); return NULL; } while(0)
	ml_impl_f impl;
	struct ml_expr_t *left, *right;

	left = ml_parse_muldiv(token, err);
	if(left == NULL)
		return NULL;

	while(true) {
		if((*token)->type == ml_token_plus_e)
			impl = ml_eval_add;
		else if((*token)->type == ml_token_minus_e)
			impl = ml_eval_sub;
		else
			break;

		*token = (*token)->next;
		right = ml_parse_muldiv(token, err);
		if(right == NULL)
			fail("Missing right-hand expression.");

		left = ml_expr_app(ml_expr_value(ml_value_impl(impl)), ml_expr_set(ml_set_new2(left, right)));
	}

	return left;
}

/**
 * Parse a multiplication or division expression.
 *   @token: The token.
 *   @err: The error.
 *   &returns: The expression or null.
 */

struct ml_expr_t *ml_parse_muldiv(struct ml_token_t **token, char **err)
{
#undef fail
#define fail(str) do { if(left != NULL) ml_expr_delete(left); if(*err == NULL) ml_eprintf(err, "%s:%u:%u: %s", (*token)->tag.file, (*token)->tag.line, (*token)->tag.col, str); return NULL; } while(0)
	ml_impl_f impl;
	struct ml_expr_t *left, *right;

	left = ml_parse_unary(token, err);
	if(left == NULL)
		return NULL;

	while(true) {
		if((*token)->type == ml_token_star_e)
			impl = ml_eval_mul;
		else if((*token)->type == ml_token_slash_e)
			impl = ml_eval_div;
		else if((*token)->type == ml_token_mod_e)
			impl = ml_eval_mod;
		else
			break;

		*token = (*token)->next;
		right = ml_parse_unary(token, err);
		if(right == NULL)
			fail("Missing right-hand expression.");

		left = ml_expr_app(ml_expr_value(ml_value_impl(impl)), ml_expr_set(ml_set_new2(left, right)));
	}

	return left;
}

/**
 * Parse an unary expression.
 *   @token: The token.
 *   @err: The error.
 *   &returns: The expression or null.
 */

struct ml_expr_t *ml_parse_unary(struct ml_token_t **token, char **err)
{
	if((*token)->type == ml_token_minus_e) {
		struct ml_expr_t *expr;

		*token = (*token)->next;
		expr = ml_parse_value(token, err);

		return expr ? ml_expr_app(ml_expr_value(ml_value_impl(ml_eval_neg)), expr) : NULL;
	}
	else if((*token)->type == ml_token_plus_e) {
		*token = (*token)->next;

		return ml_parse_value(token, err);
	}
	else
		return ml_parse_value(token, err);
}

/**
 * Parse a value.
 *   @token: The token.
 *   @err: The error.
 *   &returns: The expression or null.
 */

struct ml_expr_t *ml_parse_value(struct ml_token_t **token, char **err)
{
#undef fail
#define fail(str) do { if(expr != NULL) ml_expr_delete(expr); if(*err == NULL) ml_eprintf(err, "%s:%u:%u: %s", (*token)->tag.file, (*token)->tag.line, (*token)->tag.col, str); return NULL; } while(0)

	struct ml_expr_t *expr;

	if((*token)->type == ml_token_nil_e)
		expr = ml_expr_value(ml_value_nil());
	else if((*token)->type == ml_token_num_e)
		expr = ml_expr_value(ml_value_num((*token)->data.flt));
	else if((*token)->type == ml_token_str_e)
		expr = ml_expr_value(ml_value_str((*token)->data.str));
	else if((*token)->type == ml_token_id_e)
		expr = ml_expr_id(strdup((*token)->data.str));
	else if((*token)->type == ml_token_lbrace_e) {
		*token = (*token)->next;

		expr = ml_parse_list(token, err);

		if((*token)->type != ml_token_rbrace_e)
			fail("Missing ']'.");
	}
	else if((*token)->type == ml_token_lparen_e) {
		*token = (*token)->next;

		expr = ml_parse_set(token, err);
		if(expr == NULL)
			fail("Missing expresion.");

		if((*token)->type != ml_token_rparen_e)
			fail("Missing ')'.");
	}
	else
		return NULL;

	*token = (*token)->next;

	return expr;
}
