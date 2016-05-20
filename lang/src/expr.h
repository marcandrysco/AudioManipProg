#ifndef EXPR_H
#define EXPR_H

/**
 * Expression type enumerator.
 *   @ml_expr_value_v: Value.
 *   @ml_expr_var_v: Variable.
 *   @ml_expr_app_v: Function application.
 *   @ml_expr_let_v: Let.
 *   @ml_expr_cond_v: Conditional.
 *   @ml_expr_match_v: Match.
 *   @ml_expr_tuple_v: Tuple.
 */
enum ml_expr_e {
	ml_expr_value_v,
	ml_expr_var_v,
	ml_expr_app_v,
	ml_expr_let_v,
	ml_expr_cond_v,
	ml_expr_match_v,
	ml_expr_tuple_v
};

/**
 * Expression data union.
 *   @value: Constant value.
 *   @var: Variable identifier.
 *   @app: Function application.
 *   @let: Let.
 *   @cond: Conditional.
 *   @match: Match.
 *   @tuple: Tuple expression.
 */
union ml_expr_u {
	struct ml_value_t *value;
	char *var;
	struct ml_app_t *app;
	struct ml_let_t *let;
	struct ml_cond_t *cond;
	struct ml_match_t *match;
	struct ml_tuple_t *tuple;
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


/**
 * Function application structure.
 *   @left, right: The left and right expressions.
 */
struct ml_app_t {
	struct ml_expr_t *left, *right;
};

/**
 * Condition structure.
 *   @eval, ontrue, onfalse: The evaluation, ontrue, and onfalse expressions.
 */
struct ml_cond_t {
	struct ml_expr_t *eval, *ontrue, *onfalse;
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
 * Match structure.
 *   @expr: The expression.
 *   @with: The with list.
 */
struct ml_match_t {
	struct ml_expr_t *expr;
	struct ml_with_t *with;
};

/**
 * With structure.
 *   @pat: The pattern.
 *   @expr: The expression.
 *   @next: The next with.
 */
struct ml_with_t {
	struct ml_pat_t *pat;
	struct ml_expr_t *expr;
	struct ml_with_t *next;
};


/**
 * Tuple structure.
 *   @head, tail: The head and tail elements.
 *   @len: The length.
 */
struct ml_tuple_t {
	struct ml_elem_t *head, *tail;
	unsigned int len;
};

/**
 * Tuple element structure.
 *   @expr: The expression.
 *   @prev, next: The previous and next elements.
 */
struct ml_elem_t {
	struct ml_expr_t *expr;
	struct ml_elem_t *prev, *next;
};


/*
 * expression declarations
 */
struct ml_expr_t *ml_expr_new(enum ml_expr_e type, union ml_expr_u data, struct ml_tag_t tag);
struct ml_tuple_t *ml_tuple_newl(struct ml_expr_t *expr, ...);
struct ml_expr_t *ml_expr_copy(struct ml_expr_t *expr);
void ml_expr_delete(struct ml_expr_t *expr);

struct ml_expr_t *ml_expr_value(struct ml_value_t *value, struct ml_tag_t tag);
struct ml_expr_t *ml_expr_var(char *var, struct ml_tag_t tag);
struct ml_expr_t *ml_expr_app(struct ml_app_t *app, struct ml_tag_t tag);
struct ml_expr_t *ml_expr_let(struct ml_let_t *let, struct ml_tag_t tag);
struct ml_expr_t *ml_expr_cond(struct ml_cond_t *cond, struct ml_tag_t tag);
struct ml_expr_t *ml_expr_match(struct ml_match_t *match, struct ml_tag_t tag);
struct ml_expr_t *ml_expr_tuple(struct ml_tuple_t *tuple, struct ml_tag_t tag);

char *ml_expr_eval(struct ml_value_t **ret, struct ml_expr_t *expr, struct ml_env_t *env);

/*
 * application declarations
 */
struct ml_app_t *ml_app_new(struct ml_expr_t *left, struct ml_expr_t *right);
struct ml_app_t *ml_app_copy(struct ml_app_t *app);
void ml_app_delete(struct ml_app_t *app);

/*
 * conditional declarations
 */
struct ml_cond_t *ml_cond_new(struct ml_expr_t *eval, struct ml_expr_t *ontrue, struct ml_expr_t *onfalse);
struct ml_cond_t *ml_cond_copy(struct ml_cond_t *cond);
void ml_cond_delete(struct ml_cond_t *cond);

/*
 * let declarations
 */
struct ml_let_t *ml_let_new(struct ml_pat_t *pat, struct ml_expr_t *value, struct ml_expr_t *expr);
struct ml_let_t *ml_let_copy(struct ml_let_t *let);
void ml_let_delete(struct ml_let_t *let);

/*
 * match declarations
 */
struct ml_match_t *ml_match_new(struct ml_expr_t *expr);
struct ml_match_t *ml_match_copy(struct ml_match_t *match);
void ml_match_delete(struct ml_match_t *match);

struct ml_with_t *ml_with_new(struct ml_pat_t *pat, struct ml_expr_t *expr);

/*
 * tuple declarations
 */
struct ml_tuple_t *ml_tuple_new(void);
struct ml_tuple_t *ml_tuple_copy(struct ml_tuple_t *tuple);
void ml_tuple_delete(struct ml_tuple_t *tuple);

void ml_tuple_prepend(struct ml_tuple_t *tuple, struct ml_expr_t *expr);
void ml_tuple_append(struct ml_tuple_t *tuple, struct ml_expr_t *expr);


/**
 * Delete an expression if not null.
 *   @expr: The expression.
 */
static inline void ml_expr_erase(struct ml_expr_t *expr)
{
	if(expr != NULL)
		ml_expr_delete(expr);
}

/**
 * Delete a tuple if not null.
 *   @tuple: The tuple.
 */
static inline void ml_tuple_erase(struct ml_tuple_t *tuple)
{
	if(tuple != NULL)
		ml_tuple_delete(tuple);
}

#endif
