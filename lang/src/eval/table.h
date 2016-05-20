#ifndef EVAL_TABLE_H
#define EVAL_TABLE_H

/**
 * Evaluator structure.
 *   @id: The identifier.
 *   @func: The function.
 */
struct ml_eval_t {
	const char *id;
	ml_eval_f func;
};

/*
 * table declarations
 */
extern struct ml_eval_t ml_eval_table[];

ml_eval_f ml_eval_find(const char *id);

struct ml_value_t *ml_eval_closure(const char *id);
struct ml_value_t *ml_eval_value(ml_eval_f func, unsigned int n);

#endif
