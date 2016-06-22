#ifndef EVAL_ARITH_H
#define EVAL_ARITH_H

bool ml_get_flt(double *flt, struct ml_value_t *value, unsigned int n);

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

char *ml_eval_round(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env);

char *ml_eval_min(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env);
char *ml_eval_max(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env);
char *ml_eval_bound(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env);

#endif
