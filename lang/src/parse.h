#ifndef PARSe_H
#define PARSE_H

/*
 * parse declarations
 */
struct ml_token_t;

char *ml_parse_file(struct ml_env_t **env, const char *path);
char *ml_parse_top(struct ml_env_t **env, struct ml_token_t *token, const char *path);

#endif
