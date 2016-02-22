#include "common.h"


void ml_parse_top(struct ml_token_t *token, struct ml_env_t *env, char **err);
struct ml_expr_t *ml_parse_expr(struct ml_token_t **token, char **err);
struct ml_expr_t *ml_parse_addsub(struct ml_token_t **token, char **err);
struct ml_expr_t *ml_parse_value(struct ml_token_t **token, char **err);


void ml_parse_top(struct ml_token_t *token, struct ml_env_t *env, char **err)
{
#undef fail
#define fail(str) do { if(expr != NULL) ml_expr_delete(expr); if(*err == NULL) ml_eprintf(err, "%s:%u:%u: %s", token->tag.file, token->tag.line, token->tag.col, str); return; } while(0)
	struct ml_expr_t *expr = NULL;

	while(token->type != ml_token_end_e) {
		const char *id;

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

		if(strcmp(id, "_") == 0) {
			ml_expr_print(expr, stdout);
			printf("\n");

			if(1) {
			struct ml_expr_t *eval;

			eval = ml_expr_eval(expr, env, err);
			if(eval == NULL)
				fail(*err);

			ml_expr_print(eval, stdout);
			printf("\n");

			ml_expr_delete(eval);
			}

			ml_expr_delete(expr);
		}
		else
			ml_env_add(env, strdup(id), expr);
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
 * Parse a tuple.
 *   @token: The token.
 *   @err: The error.
 *   &returns: The expression or null.
 */

struct ml_expr_t *ml_parse_tuple(struct ml_token_t **token, char **err)
{
#undef fail
#define fail(str, ...) do { if(expr != NULL) ml_expr_delete(expr); if(*err == NULL) ml_eprintf(err, "%s:%u:%u: " str, (*token)->tag.file, (*token)->tag.line, (*token)->tag.col, ##__VA_ARGS__); return NULL; } while(0)

	struct ml_tuple_t tuple;
	struct ml_expr_t *expr;

	expr = ml_parse_expr(token, err);
	if(expr == NULL)
		return NULL;

	if((*token)->type != ml_token_comma_e)
		return expr;

	tuple = ml_tuple_new();
	ml_tuple_add(&tuple, expr);

	do {
		*token = (*token)->next;
		expr = ml_parse_expr(token, err);
		if(expr == NULL)
			fail("Missing expression in tuple.");

		ml_tuple_add(&tuple, expr);
	} while((*token)->type == ml_token_comma_e);

	return ml_expr_tuple(tuple);
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
	struct ml_eval_t *eval;
	struct ml_expr_t *left, *right;

	left = ml_parse_value(token, err);
	if(left == NULL)
		return NULL;

	while(true) {
		if((*token)->type == ml_token_plus_e)
			eval = &ml_eval_add;
		else
			break;

		*token = (*token)->next;
		right = ml_parse_value(token, err);
		if(right == NULL)
			fail("Missing right-hand expression.");

		left = ml_expr_prim(eval, ml_expr_tuple(ml_tuple_new2(left, right)));
	}

	return left;
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

	if((*token)->type == ml_token_num_e)
		expr = ml_expr_num((*token)->data.flt);
	else if((*token)->type == ml_token_id_e)
		expr = ml_expr_id(strdup((*token)->data.str));
	else if((*token)->type == ml_token_lparen_e) {
		*token = (*token)->next;

		expr = ml_parse_tuple(token, err);

		if((*token)->type != ml_token_rparen_e)
			fail("Missing ')'.");

		if(expr == NULL)
			expr = ml_expr_nil();
	}
	else
		return NULL;

	*token = (*token)->next;

	return expr;
}


int DBG_memcnt = 0;
int DBG_rescnt = 0;

/**
 * Main entry function.
 *   @argc: The number of arguments.
 *   @argv: The argument array.
 *   &returns: Always zero.
 */

int main(int argc, char **argv)
{
	char *err = NULL;
	struct ml_token_t *token;
	struct ml_env_t *env;

	token = ml_token_load("test.ml", &err);
	if(err != NULL)
		fprintf(stderr, "%s\n", err), abort();

	env = ml_env_new();

	ml_parse_top(token, env, &err);
	if(err != NULL)
		fprintf(stderr, "%s\n", err), abort();

	ml_env_delete(env);
	ml_token_clean(token);

	if(DBG_memcnt != 0)
		printf("memcnt: %d\n", DBG_memcnt);

	return 0;
}
