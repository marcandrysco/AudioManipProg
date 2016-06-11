#ifndef EVAL_H
#define EVAL_H

/*
 * common evaluation declarations
 */
char *amp_eval_vel(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env);
char *amp_eval_amp2db(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env);
char *amp_eval_db2amp(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env);

#endif
