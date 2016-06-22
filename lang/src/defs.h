#ifndef DEFS_H
#define DEFS_H

/*
 * structure prototypes
 */
struct ml_env_t;
struct ml_value_t;

/**
 * Path structure.
 *   @str: The string.
 *   @refcnt: The reference count.
 */
struct ml_path_t {
	char *str;
	unsigned int refcnt;
};

/**
 * Tag structure.
 *   @path: The path.
 *   @line, col: The line and column information.
 */
struct ml_tag_t {
	struct ml_path_t *path;
	unsigned int line, col;
};


/**
 * Evaluator function.
 *   @ret: Ref. The returned value.
 *   @value: Consumed. The input value.
 *   @env: The environment.
 *   &returns: Error.
 */
typedef char *(*ml_eval_f)(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env);

#endif
