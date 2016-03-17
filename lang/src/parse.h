#ifndef PARSE_H
#define PARSE_H

/*
 * parse declarations
 */

struct ml_env_t;
struct ml_token_t;

char *ml_parse_top(struct ml_token_t *token, struct ml_env_t **env, const char *path);
struct ml_expr_t *ml_parse_stmt(struct ml_token_t **token, char **err);
struct ml_expr_t *ml_parse_expr(struct ml_token_t **token, char **err);
struct ml_expr_t *ml_parse_set(struct ml_token_t **token, char **err);
struct ml_expr_t *ml_parse_list(struct ml_token_t **token, char **err);
struct ml_expr_t *ml_parse_concat(struct ml_token_t **token, char **err);
struct ml_expr_t *ml_parse_cons(struct ml_token_t **token, char **err);
struct ml_expr_t *ml_parse_cmp(struct ml_token_t **token, char **err);
struct ml_expr_t *ml_parse_addsub(struct ml_token_t **token, char **err);
struct ml_expr_t *ml_parse_muldiv(struct ml_token_t **token, char **err);
struct ml_expr_t *ml_parse_unary(struct ml_token_t **token, char **err);
struct ml_expr_t *ml_parse_app(struct ml_token_t **token, char **err);
struct ml_expr_t *ml_parse_value(struct ml_token_t **token, char **err);

char *ml_parse_pat(struct ml_pat_t **dest, struct ml_token_t **token, bool comma);
char *ml_parse_pat_cons(struct ml_pat_t **pat, struct ml_token_t **token);
char *ml_parse_pat_value(struct ml_pat_t **pat, struct ml_token_t **token);

#endif
