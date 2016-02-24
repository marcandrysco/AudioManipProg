#ifndef PARSE_H
#define PARSE_H

/*
 * parse declarations
 */

struct ml_env_t;
struct ml_token_t;

void ml_parse_top(struct ml_token_t *token, struct ml_env_t *env, char **err);
struct ml_expr_t *ml_parse_stmt(struct ml_token_t **token, char **err);
struct ml_expr_t *ml_parse_expr(struct ml_token_t **token, char **err);
struct ml_expr_t *ml_parse_set(struct ml_token_t **token, char **err);
struct ml_expr_t *ml_parse_list(struct ml_token_t **token, char **err);
struct ml_expr_t *ml_parse_cons(struct ml_token_t **token, char **err);
struct ml_expr_t *ml_parse_cmp(struct ml_token_t **token, char **err);
struct ml_expr_t *ml_parse_addsub(struct ml_token_t **token, char **err);
struct ml_expr_t *ml_parse_muldiv(struct ml_token_t **token, char **err);
struct ml_expr_t *ml_parse_unary(struct ml_token_t **token, char **err);
struct ml_expr_t *ml_parse_value(struct ml_token_t **token, char **err);

#endif
