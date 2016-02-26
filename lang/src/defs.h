#ifndef DEFS_H
#define DEFS_H

/*
 * structure prototypes
 */

struct ml_env_t;


/**
 * Tag structure.
 *   @file: The file.
 *   @off, line, col: The offset, line, and column info.
 */

struct ml_tag_t {
	const char *file;
	unsigned int off, line, col;
};


/**
 * Native implementation function.
 *   @value: The input value.
 *   @env: The environment.
 *   @err: The error.
 *   &returns: The processed value.
 */

typedef struct ml_value_t *(*ml_impl_f)(struct ml_value_t *, struct ml_env_t *, char **);


/**
 * Box interface.
 *   @id: The identifier.
 *   @copy: Copy.
 *   @delete: Deletion.
 */

struct ml_box_i {
	const char *id;
	void *(*copy)(void *);
	void (*delete)(void *);
};

/**
 * Box structure.
 *   @ref: The reference.
 *   @iface: The interface.
 */

struct ml_box_t {
	void *ref;
	const struct ml_box_i *iface;
};

/**
 * Tuple structure.
 *   @len: The length.
 *   @value; The value array.
 */

struct ml_tuple_t {
	unsigned int len;
	struct ml_value_t **value;
};

/**
 * Closure structure.
 *   @env: The environment.
 *   @pat: Patterned argument.
 *   @rec: The recursive name.
 *   @expr: The expression.
 */

struct ml_closure_t {
	struct ml_env_t *env;
	struct ml_pat_t *pat;
	char *rec;
	struct ml_expr_t *expr;
};

/**
 * Link structure.
 *   @value: The value.
 *   @prev, next: Previous or next link.
 */

struct ml_link_t {
	struct ml_value_t *value;
	struct ml_link_t *prev, *next;
};

/**
 * List structure.
 *   @head, tail: The head and tail links.
 */

struct ml_list_t {
	struct ml_link_t *head, *tail;
};


/**
 * Value type enumerator.
 *   @ml_value_num_e: Number.
 *   @ml_value_bool_e: Boolean.
 *   @ml_value_str_e: String.
 *   @ml_value_tuple_e: Tuple.
 *   @ml_value_list_e: List.
 *   @ml_value_closure_e: Closure.
 *   @ml_value_box_e: Boxed type.
 *   @ml_value_impl_e: Native implementation.
 */

enum ml_value_e {
	ml_value_nil_e,
	ml_value_bool_e,
	ml_value_num_e,
	ml_value_str_e,
	ml_value_tuple_e,
	ml_value_list_e,
	ml_value_closure_e,
	ml_value_box_e,
	ml_value_impl_e
};

/**
 * Value data union.
 *   @flag: Boolean flag.
 *   @num: Number.
 *   @str: The string.
 *   @tuple: Tuple.
 *   @list: List.
 *   @closure: Closure.
 *   @box: Bosed value.
 *   @impl: Native imlementation.
 */

union ml_value_u {
	bool flag;
	double num;
	char *str;
	struct ml_tuple_t tuple;
	struct ml_list_t list;
	struct ml_closure_t closure;
	struct ml_box_t box;
	ml_impl_f impl;
};

/**
 * Value structure.
 *   @type: The type.
 *   @data: The data.
 */

struct ml_value_t {
	enum ml_value_e type;
	union ml_value_u data;
};


/**
 * Set structure.
 *   @len: The length.
 *   @expr; The expression array.
 */

struct ml_set_t {
	unsigned int len;
	struct ml_expr_t **expr;
};

/**
 * Function structure.
 *   @pat: The pattern.
 *   @expr: The pattern and expression.
 */

struct ml_func_t {
	struct ml_pat_t *pat;
	struct ml_expr_t *expr;
};

/**
 * Application structure.
 *   @func, value: The function and value.
 */

struct ml_app_t {
	struct ml_expr_t *func, *value;
};

/**
 * Let structure.
 *   @pat: The pattern.
 *   @value, expr: The value and expression.
 */

struct ml_let_t {
	struct ml_pat_t *pat;
	struct ml_expr_t *value, *expr;
};

/**
 * Condition structure.
 *   @eval, ontrue, onfalse: The evaluation, true, and false expressions.
 */

struct ml_cond_t {
	struct ml_expr_t *eval, *ontrue, *onfalse;
};

/**
 * Expression type enumerator.
 *   @ml_expr_id_e: Identifier.
 *   @ml_expr_set_e: Set.
 *   @ml_expr_func_e: Function.
 *   @ml_expr_app_e: Application.
 *   @ml_expr_let_e: Let.
 *   @ml_expr_cone_e: Conditional.
 *   @ml_expr_value_e: Constant value.
 */

enum ml_expr_e {
	ml_expr_id_e,
	ml_expr_set_e,
	ml_expr_func_e,
	ml_expr_app_e,
	ml_expr_let_e,
	ml_expr_cond_e,
	ml_expr_value_e
};

/**
 * Expression data union.
 *   @id: The identifier.
 *   @set: Set.
 *   @box: Boxed data.
 *   @func: Function.
 *   @impl: Native implementation.
 *   @closure: Closure.
 *   @app: Application.
 *   @let: Let.
 *   @cond: Conditional.
 *   @value: Constant value.
 */

union ml_expr_u {
	char *id;
	struct ml_set_t set;
	struct ml_func_t func;
	struct ml_app_t app;
	struct ml_let_t let;
	struct ml_cond_t cond;
	struct ml_value_t *value;
};

/**
 * Expression structure.
 *   @tag: The tag.
 *   @type: The type.
 *   @data: The data.
 */

struct ml_expr_t {
	struct ml_tag_t tag;
	enum ml_expr_e type;
	union ml_expr_u data;
};


#endif
