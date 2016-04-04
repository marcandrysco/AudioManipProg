#ifndef EVAL_MATH_H
#define EVAL_MATH_H

/**
 * Evaluator structure.
 *   @id: The identifier.
 *   @func: The function.
 */
struct ml_eval0_t {
	const char *id;
	ml_eval_f func;
};

/*
 * math evaluator declarations
 */
extern struct ml_eval0_t ml_eval_table[];

char *ml_eval_randf(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env);
char *ml_eval_pow(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env);

#endif
