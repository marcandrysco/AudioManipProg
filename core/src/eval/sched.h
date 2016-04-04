#ifndef EVAL_SCHED_H
#define EVAL_SCHED_H

/*
 * scheduler declarations
 */
char *amp_eval_skyline(struct ml_value_t **result, struct ml_value_t *value, struct ml_env_t *env);
struct ml_value_t *amp_eval_skyline0(struct ml_value_t *value, struct ml_env_t *env, char **err);

#endif
