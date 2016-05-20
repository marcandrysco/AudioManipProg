#ifndef EVAL_ARITH_H
#define EVAL_ARITH_H

/*
 * arithmetic evaluation declarations
 */
char *ml_eval_neg(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env);
char *ml_eval_add(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env);
char *ml_eval_sub(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env);
char *ml_eval_mul(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env);
char *ml_eval_div(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env);
char *ml_eval_mod(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env);

char *ml_eval_sqrt(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env);
char *ml_eval_exp(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env);
char *ml_eval_log(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env);
char *ml_eval_pow(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env);

char *ml_eval_lt(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env);
char *ml_eval_lte(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env);
char *ml_eval_gt(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env);
char *ml_eval_gte(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env);

#endif
