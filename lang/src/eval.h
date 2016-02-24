#ifndef EVAL_H
#define EVAL_H

/*
 * evaluation declarations
 */

struct ml_value_t *ml_eval_neg(struct ml_value_t *value, struct ml_env_t *env, char **err);
struct ml_value_t *ml_eval_add(struct ml_value_t *value, struct ml_env_t *env, char **err);
struct ml_value_t *ml_eval_sub(struct ml_value_t *value, struct ml_env_t *env, char **err);
struct ml_value_t *ml_eval_mul(struct ml_value_t *value, struct ml_env_t *env, char **err);
struct ml_value_t *ml_eval_div(struct ml_value_t *value, struct ml_env_t *env, char **err);
struct ml_value_t *ml_eval_mod(struct ml_value_t *value, struct ml_env_t *env, char **err);
struct ml_value_t *ml_eval_gt(struct ml_value_t *value, struct ml_env_t *env, char **err);
struct ml_value_t *ml_eval_gte(struct ml_value_t *value, struct ml_env_t *env, char **err);
struct ml_value_t *ml_eval_lt(struct ml_value_t *value, struct ml_env_t *env, char **err);
struct ml_value_t *ml_eval_lte(struct ml_value_t *value, struct ml_env_t *env, char **err);
struct ml_value_t *ml_eval_list(struct ml_value_t *value, struct ml_env_t *env, char **err);
struct ml_value_t *ml_eval_cons(struct ml_value_t *value, struct ml_env_t *env, char **err);

#endif
