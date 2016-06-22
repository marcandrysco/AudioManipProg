#ifndef EVAL_LIST_H
#define EVAL_LIST_H


/*
 * list evaluation declarations
 */
char *ml_eval_list(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env);
char *ml_eval_cons(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env);
char *ml_eval_concat(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env);

char *ml_eval_seq(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env);
char *ml_eval_seqf(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env);

char *ml_eval_map(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env);
char *ml_eval_foldr(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env);

#endif
