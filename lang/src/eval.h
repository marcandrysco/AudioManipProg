#ifndef EVAL_H
#define EVAL_H

struct ml_eval_t {
	const char *id;
	struct ml_value_t *(*func)(struct ml_value_t *, struct ml_env_t *, char **);
};

/*
 * evaluation declarations
 */

extern struct ml_eval_t ml_eval_arr[];

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
struct ml_value_t *ml_eval_concat(struct ml_value_t *value, struct ml_env_t *env, char **err);

struct ml_value_t *ml_eval_exp(struct ml_value_t *value, struct ml_env_t *env, char **err);
struct ml_value_t *ml_eval_log(struct ml_value_t *value, struct ml_env_t *env, char **err);
struct ml_value_t *ml_eval_floor(struct ml_value_t *value, struct ml_env_t *env, char **err);
struct ml_value_t *ml_eval_ceil(struct ml_value_t *value, struct ml_env_t *env, char **err);
struct ml_value_t *ml_eval_round(struct ml_value_t *value, struct ml_env_t *env, char **err);

struct ml_value_t *ml_eval_print(struct ml_value_t *value, struct ml_env_t *env, char **err);
struct ml_value_t *ml_eval_fail(struct ml_value_t *value, struct ml_env_t *env, char **err);

struct ml_value_t *ml_eval_i2str(struct ml_value_t *value, struct ml_env_t *env, char **err);
struct ml_value_t *ml_eval_strlen(struct ml_value_t *value, struct ml_env_t *env, char **err);

#endif
