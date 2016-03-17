#include "common.h"


/**
 * Parse from the top.
 *   @token: The head token.
 *   @env: The environment.
 *   @path: Optional. The path for parsing imports.
 *   &returns: Error.
 */
char *ml_parse_top(struct ml_token_t *token, struct ml_env_t **env, const char *path)
{
#define onexit ml_pat_erase(pat); ml_expr_erase(expr); ml_value_erase(value);
	char *err;

	while(token->type != ml_token_end_e) {
		struct ml_value_t *value = NULL;
		struct ml_pat_t *pat = NULL;
		struct ml_expr_t *expr = NULL;

		if(token->type == ml_token_import_e) {
			token = token->next;
			expr = ml_parse_stmt(&token, &err);
			if(expr == NULL) {
				if(err)
					return err;
				else
					fail("Invalid let. Expected expression.");
			}

			value = ml_expr_eval(expr, *env, &err);
			if(value == NULL)
				return err;

			if(value->type != ml_value_str_e)
				fail("Import requries string value.");

			chkfail(ml_env_proc(value->data.str, env)); // TODO: relative to 'path'

			ml_value_delete(value);
			ml_expr_delete(expr);
		}
		else if(token->type == ml_token_let_e) {
			token = token->next;

			chkfail(ml_parse_pat(&pat, &token, false));
			if(pat == NULL)
				fail("Missin pattern.");

			if(token->type != ml_token_equal_e)
				fail("Invalid let. Expected '='.");

			token = token->next;
			expr = ml_parse_stmt(&token, &err);
			if(expr == NULL) {
				if(err)
					return err;
				else
					fail("Invalid let. Expected expression.");
			}

			if(pat->next != NULL) {
				struct ml_closure_t closure;

				if(pat->type != ml_pat_id_e)
					fail("Invalid function declaration.");

				closure = ml_closure(ml_env_copy(*env), ml_pat_copy(pat->next), strdup(pat->data.id), ml_expr_copy(expr));
				ml_env_add(env, strdup(pat->data.id), ml_value_closure(closure));
			}
			else {
				value = ml_expr_eval(expr, *env, &err);
				if(value == NULL)
					return err;

				if(!ml_pat_match(env, pat, value))
					fail("Pattern match failed.");

				ml_value_delete(value);
			}

			ml_pat_delete(pat);
			ml_expr_delete(expr);
		}
		else
			fail("Missing let.");
	}

	return NULL;
#undef onexit
}


/**
 * Parse a pattern.
 *   @dest: The destination.
 *   @token: The token.
 *   @comma: Comma separator flag.
 *   &returns: Error.
 */
char *ml_parse_pat(struct ml_pat_t **dest, struct ml_token_t **token, bool comma)
{
#define onexit ml_pat_delete(head);
	struct ml_pat_t *head = NULL, **pat = &head;

	while(true) {
		chkfail(ml_parse_pat_cons(pat, token));
		if(*pat == NULL) {
			if(comma)
				fail("Missing ')'.");
			else
				break;
		}
		else if(comma) {
			if((*token)->type != ml_token_comma_e)
				break;

			*token = (*token)->next;
		}

		pat = &(*pat)->next;
	}

	*dest = head;

	return NULL;
#undef onexit
}

/**
 * Parse a cons pattern.
 *   @pat: Ref. The pattern.
 *   @token: The token.
 *   &returns: Error.
 */
char *ml_parse_pat_cons(struct ml_pat_t **pat, struct ml_token_t **token)
{
#define onexit ml_pat_erase(*pat);
	struct ml_pat_t *right;

	chkfail(ml_parse_pat_value(pat, token));
	if(*pat == NULL)
		return NULL;

	if((*token)->type != ml_token_cons_e)
		return NULL;

	*token = (*token)->next;
	chkfail(ml_parse_pat_cons(&right, token));
	if(right == NULL)
		fail("Missing pattern after '::'.");

	*pat = ml_pat_list(*pat, right);

	return NULL;
#undef onexit
}

/**
 * Parse a value pattern.
 *   @pat: Ref. The pattern.
 *   @token: The token.
 *   &returns: Error.
 */
char *ml_parse_pat_value(struct ml_pat_t **pat, struct ml_token_t **token)
{
#define onexit
	if((*token)->type == ml_token_id_e) {
		*pat = ml_pat_id(strdup((*token)->data.str));
		*token = (*token)->next;
	}
	else if((*token)->type == ml_token_num_e) {
		*pat = ml_pat_value(ml_value_num((*token)->data.flt));
		*token = (*token)->next;
	}
	else if((*token)->type == ml_token_str_e) {
		*pat = ml_pat_value(ml_value_str(strdup((*token)->data.str)));
		*token = (*token)->next;
	}
	else if((*token)->type == ml_token_lparen_e) {
		struct ml_pat_t *sub;

		*token = (*token)->next;
		chkfail(ml_parse_pat(&sub, token, true));
		if((*token)->type != ml_token_rparen_e)
			fail("Missing ')'.");

		*token = (*token)->next;
		*pat = ml_pat_tuple(sub);
	}
	else
		return NULL;

	return NULL;
#undef onexit
}


/**
 * Parse a statement.
 *   @token: The token.
 *   @err: The error.
 *   &returns: The expression or null.
 */

struct ml_expr_t *ml_parse_stmt(struct ml_token_t **token, char **err)
{
#undef fail
#define fail(str, ...) do { ml_pat_delete(pat); ml_expr_erase(left); ml_expr_erase(value); if(*err == NULL) ml_eprintf(err, "%s:%u:%u: " str, (*token)->tag.file, (*token)->tag.line, (*token)->tag.col, ##__VA_ARGS__); return NULL; } while(0)

	struct ml_pat_t *pat = NULL;
	struct ml_expr_t *left = NULL, *value = NULL, *expr;

	*err = NULL;

	if((*token)->type == ml_token_func_e) {
		*token = (*token)->next;
		*err = ml_parse_pat(&pat, token, false);
		if(*err != NULL)
			return NULL;
		else if(pat == NULL)
			fail("Missing function parameters.");

		if((*token)->type != ml_token_arrow_e)
			fail("Missing '->'.");

		*token = (*token)->next;
		expr = ml_parse_stmt(token, err);
		if(expr == NULL)
			fail("Missing function expression.", (*token)->type);

		return ml_expr_func(pat, expr);
	}
	else if((*token)->type == ml_token_let_e) {
		*token = (*token)->next;
		*err = ml_parse_pat(&pat, token, false);
		if(*err != NULL)
			return NULL;
		else if(pat == NULL)
			fail("Missing function parameters.");

		if((*token)->type != ml_token_equal_e)
			fail("Expected '='.");

		*token = (*token)->next;
		value = ml_parse_stmt(token, err);
		if(value == NULL)
			fail("Missing expression in 'let'.");

		if((*token)->type != ml_token_in_e)
			fail("Expected 'in'.");

		*token = (*token)->next;
		expr = ml_parse_stmt(token, err);
		if(expr == NULL)
			fail("Missing expression in 'let'.");

		return ml_expr_let(pat, value, expr);
	}
	else if((*token)->type == ml_token_if_e) {
		*token = (*token)->next;
		left = ml_parse_expr(token, err);
		if(left == NULL)
			return NULL;

		if((*token)->type != ml_token_then_e)
			fail("Expected 'then'.");

		*token = (*token)->next;
		value = ml_parse_stmt(token, err);
		if(value == NULL)
			fail("Missing ontrue expression in 'if'.");

		if((*token)->type != ml_token_else_e)
			fail("Expected 'else'.");

		*token = (*token)->next;
		expr = ml_parse_stmt(token, err);
		if(expr == NULL)
			fail("Missing onfalse expression in 'if'.");

		return ml_expr_cond(left, value, expr);
	}
	else if((*token)->type == ml_token_match_e) {
		struct ml_match_t *match;

		*token = (*token)->next;
		expr = ml_parse_expr(token, err);
		if(expr == NULL)
			return NULL;

		if((*token)->type != ml_token_with_e)
			fail("Expected 'with'.");

		match = ml_match_new(expr);

		*token = (*token)->next;
		while((*token)->type == ml_token_pipe_e) {
			*token = (*token)->next;
			*err = ml_parse_pat(&pat, token, false);
			if(*err != NULL)
				return NULL;
			else if(pat == NULL)
				fail("Missing pattern.");

			if((*token)->type != ml_token_arrow_e)
				fail("Expected '->'.");

			*token = (*token)->next;
			expr = ml_parse_stmt(token, err);
			if(expr == NULL)
				fail("Expected expresion.");

			ml_match_append(match, pat, expr);
		}

		return ml_expr_match(match);
	}
	else
		return ml_parse_expr(token, err);
}

/**
 * Parse an expression.
 *   @token: The token.
 *   @err: The error.
 *   &returns: The expression or null.
 */

struct ml_expr_t *ml_parse_expr(struct ml_token_t **token, char **err)
{
	return ml_parse_concat(token, err);

#undef fail
#define fail(str, ...) do { ml_expr_erase(left); ml_expr_erase(value); if(*err == NULL) ml_eprintf(err, "%s:%u:%u: " str, (*token)->tag.file, (*token)->tag.line, (*token)->tag.col, ##__VA_ARGS__); return NULL; } while(0)
	struct ml_expr_t *expr, *value;

	expr = ml_parse_concat(token, err);
	if(expr == NULL)
		return expr;

	while(true) {
		value = ml_parse_concat(token, err);
		if(value == NULL)
			return expr;

		expr = ml_expr_app(expr, value);
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

	expr = ml_parse_stmt(token, err);
	if(expr == NULL)
		return NULL;

	if((*token)->type != ml_token_comma_e)
		return expr;

	set = ml_set_new();
	ml_set_add(&set, expr);

	do {
		*token = (*token)->next;
		expr = ml_parse_stmt(token, err);
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
 * Parse an concat expression.
 *   @token: The token.
 *   @err: The error.
 *   &returns: The expression or null.
 */

struct ml_expr_t *ml_parse_concat(struct ml_token_t **token, char **err)
{
#undef fail
#define fail(str) do { if(left != NULL) ml_expr_delete(left); if(*err == NULL) ml_eprintf(err, "%s:%u:%u: %s", (*token)->tag.file, (*token)->tag.line, (*token)->tag.col, str); return NULL; } while(0)
	ml_impl_f impl;
	struct ml_expr_t *left, *right;

	left = ml_parse_cons(token, err);
	if(left == NULL)
		return NULL;

	while(true) {
		if((*token)->type == ml_token_concat_e)
			impl = ml_eval_concat;
		else
			break;

		*token = (*token)->next;
		right = ml_parse_cons(token, err);
		if(right == NULL)
			fail("Missing right-hand expression.");

		left = ml_expr_app(ml_expr_value(ml_value_impl(impl)), ml_expr_set(ml_set_new2(left, right)));
	}

	return left;
}

/**
 * Parse an cons expression.
 *   @token: The token.
 *   @err: The error.
 *   &returns: The expression or null.
 */

struct ml_expr_t *ml_parse_cons(struct ml_token_t **token, char **err)
{
#undef fail
#define fail(str) do { if(left != NULL) ml_expr_delete(left); if(*err == NULL) ml_eprintf(err, "%s:%u:%u: %s", (*token)->tag.file, (*token)->tag.line, (*token)->tag.col, str); return NULL; } while(0)
	ml_impl_f impl;
	struct ml_expr_t *left, *right;

	left = ml_parse_cmp(token, err);
	if(left == NULL)
		return NULL;

	while(true) {
		if((*token)->type == ml_token_cons_e)
			impl = ml_eval_cons;
		else
			break;

		*token = (*token)->next;
		right = ml_parse_cons(token, err);
		if(right == NULL)
			fail("Missing right-hand expression.");

		left = ml_expr_app(ml_expr_value(ml_value_impl(impl)), ml_expr_set(ml_set_new2(left, right)));
	}

	return left;
}

/**
 * Parse an comparison expression.
 *   @token: The token.
 *   @err: The error.
 *   &returns: The expression or null.
 */

struct ml_expr_t *ml_parse_cmp(struct ml_token_t **token, char **err)
{
#undef fail
#define fail(str) do { if(left != NULL) ml_expr_delete(left); if(*err == NULL) ml_eprintf(err, "%s:%u:%u: %s", (*token)->tag.file, (*token)->tag.line, (*token)->tag.col, str); return NULL; } while(0)
	ml_impl_f impl;
	struct ml_expr_t *left, *right;

	left = ml_parse_addsub(token, err);
	if(left == NULL)
		return NULL;

	while(true) {
		if((*token)->type == ml_token_gt_e)
			impl = ml_eval_gt;
		else if((*token)->type == ml_token_gte_e)
			impl = ml_eval_gte;
		else if((*token)->type == ml_token_lt_e)
			impl = ml_eval_lt;
		else if((*token)->type == ml_token_lte_e)
			impl = ml_eval_lte;
		else
			break;

		*token = (*token)->next;
		right = ml_parse_addsub(token, err);
		if(right == NULL)
			fail("Missing right-hand expression.");

		left = ml_expr_app(ml_expr_value(ml_value_impl(impl)), ml_expr_set(ml_set_new2(left, right)));
	}

	return left;
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
		expr = ml_parse_app(token, err);

		return expr ? ml_expr_app(ml_expr_value(ml_value_impl(ml_eval_neg)), expr) : NULL;
	}
	else if((*token)->type == ml_token_plus_e) {
		*token = (*token)->next;

		return ml_parse_app(token, err);
	}
	else
		return ml_parse_app(token, err);
}

/**
 * Parse an applicationn.
 *   @token: The token.
 *   @err: The error.
 *   &returns: The expression or null.
 */

struct ml_expr_t *ml_parse_app(struct ml_token_t **token, char **err)
{
	struct ml_expr_t *expr, *value;

	expr = ml_parse_value(token, err);
	if(expr == NULL)
		return expr;

	while(true) {
		value = ml_parse_value(token, err);
		if(value == NULL)
			return expr;

		expr = ml_expr_app(expr, value);
	}
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
		expr = ml_expr_value(ml_value_str(strdup((*token)->data.str)));
	else if((*token)->type == ml_token_id_e) {
		if(strcmp((*token)->data.str, "true") == 0)
			expr = ml_expr_value(ml_value_bool(true));
		else if(strcmp((*token)->data.str, "false") == 0)
			expr = ml_expr_value(ml_value_bool(false));
		else if(strcmp((*token)->data.str, "exp") == 0)
			expr = ml_expr_value(ml_value_impl(ml_eval_exp));
		else if(strcmp((*token)->data.str, "log") == 0)
			expr = ml_expr_value(ml_value_impl(ml_eval_log));
		else if(strcmp((*token)->data.str, "floor") == 0)
			expr = ml_expr_value(ml_value_impl(ml_eval_floor));
		else if(strcmp((*token)->data.str, "ceil") == 0)
			expr = ml_expr_value(ml_value_impl(ml_eval_ceil));
		else if(strcmp((*token)->data.str, "round") == 0)
			expr = ml_expr_value(ml_value_impl(ml_eval_round));
		else {
			struct ml_eval_t *eval;

			for(eval = ml_eval_arr; eval->id != NULL; eval++) {
				if(strcmp(eval->id, (*token)->data.str) == 0)
					break;
			}

			if(eval->id != NULL)
				expr = ml_expr_value(ml_value_impl(eval->func));
			else
				expr = ml_expr_id(strdup((*token)->data.str), (*token)->tag);
		}
	}
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
