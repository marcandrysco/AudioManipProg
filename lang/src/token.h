#ifndef TOKEN_H
#define TOKEN_H

/**
 * Token enumerator.
 *   @ml_token_equal_e: Equal.
 *   @ml_token_gt_e: Greater than.
 *   @ml_token_gte_e: Greater than or equal.
 *   @ml_token_lt_e: Less than.
 *   @ml_token_lte_e: Less than or equal.
 *   @ml_token_lparen_e: Left parenthesis.
 *   @ml_token_rparen_e: Right parenthesis.
 *   @ml_token_lbrace_e: Left brace.
 *   @ml_token_rbrace_e: Right brace.
 *   @ml_token_nil_e: Nil value.
 *   @ml_token_comma_e: Comma.
 *   @ml_token_colon_e: Colon.
 *   @ml_token_semicolon_e: Semicolon.
 *   @ml_token_plus_e: Plus.
 *   @ml_token_minus_e: Minus.
 *   @ml_token_star_e: Star/multiply.
 *   @ml_token_slash_e: Slash/divide.
 *   @ml_token_mod_e: Modulus.
 *   @ml_token_cons_e: Cons.
 *   @ml_token_arrow_e: Arrow.
 *   @ml_token_pipe_e: Pipe.
 *   @ml_token_concat_e: List concatenation.
 *   @ml_token_id_e: Identifier.
 *   @ml_token_str_e: String.
 *   @ml_token_num_e: Number.
 *   @ml_token_let_e: Let.
 *   @ml_token_in_e: In.
 *   @ml_token_match_e: Match.
 *   @ml_token_with_e: With.
 *   @ml_token_if_e: If.
 *   @ml_token_then_e: Then.
 *   @ml_token_else_e: Else.
 *   @ml_token_func_e: Function.
 *   @ml_token_comment_e: Comment.
 *   @ml_token_end_e: End-of-input.
 */

enum ml_token_e {
	ml_token_equal_e,
	ml_token_gt_e,
	ml_token_gte_e,
	ml_token_lt_e,
	ml_token_lte_e,
	ml_token_lparen_e,
	ml_token_rparen_e,
	ml_token_lbrace_e,
	ml_token_rbrace_e,
	ml_token_nil_e,
	ml_token_comma_e,
	ml_token_colon_e,
	ml_token_semicolon_e,
	ml_token_plus_e,
	ml_token_minus_e,
	ml_token_star_e,
	ml_token_slash_e,
	ml_token_mod_e,
	ml_token_cons_e,
	ml_token_concat_e,
	ml_token_arrow_e,
	ml_token_pipe_e,
	ml_token_id_e,
	ml_token_str_e,
	ml_token_num_e,
	ml_token_let_e,
	ml_token_in_e,
	ml_token_match_e,
	ml_token_with_e,
	ml_token_if_e,
	ml_token_then_e,
	ml_token_else_e,
	ml_token_func_e,
	ml_token_comment_e,
	ml_token_end_e
};

/**
 * Token data union.
 *   @flt: Floating-point number.
 *   @str: The string.
 */

union ml_token_u {
	double flt;
	char *str;
};

/**
 * Token structure.
 *   @type: The type.
 *   @data: The data.
 *   @tag: The tag.
 *   @next: The next.
 */

struct ml_token_t {
	enum ml_token_e type;
	union ml_token_u data;

	struct ml_tag_t tag;

	struct ml_token_t *next;
};


/*
 * token declarations
 */

enum ml_token_e ml_token_symbol(FILE *file, int *byte, struct ml_tag_t *tag);
enum ml_token_e ml_token_keyword(const char *str);

struct ml_token_t *ml_token_new(enum ml_token_e type);
struct ml_token_t *ml_token_id(const char *id);
struct ml_token_t *ml_token_str(const char *str);
struct ml_token_t *ml_token_num(double flt);
void ml_token_delete(struct ml_token_t *token);

struct ml_token_t *ml_token_load(const char *path, char **err);
struct ml_token_t *ml_token_parse(FILE *file, const char *path, char **err);
void ml_token_clean(struct ml_token_t *token);

#endif
