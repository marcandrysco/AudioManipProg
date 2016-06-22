#ifndef EFX_CONV_H
#define EFX_CONV_H

/*
 * conversion declarations
 */
char *ml_eval_val2str(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env);
char *ml_eval_flt2int(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env);

char *ml_eval_isint(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env);
char *ml_eval_isflt(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env);
char *ml_eval_isnum(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env);
char *ml_eval_islist(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env);

#endif
