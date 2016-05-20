#ifndef EVAL_IO_H
#define EVAL_IO_H

char *ml_eval_print(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env);
char *ml_eval_println(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env);

#endif
