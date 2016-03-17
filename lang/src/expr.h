#ifndef EXPR_H
#define EXPR_H

/**
 * Match structure.
 *   @expr: The expression.
 *   @head, tail: The head and tail wht clauses.
 */

struct ml_match_t {
	struct ml_expr_t *expr;
	struct ml_with_t *head, *tail;
};

/**
 * Match with structure.
 *   @pat: The pattern.
 *   @expr: The expression.
 *   @prev, next: The previous and next withs.
 */

struct ml_with_t {
	struct ml_pat_t *pat;
	struct ml_expr_t *expr;

	struct ml_with_t *prev, *next;
};


/*
 * expression declarations
 */

struct ml_expr_t *ml_expr_new(enum ml_expr_e type, union ml_expr_u data, struct ml_tag_t tag);
struct ml_expr_t *ml_expr_copy(struct ml_expr_t *expr);
void ml_expr_delete(struct ml_expr_t *expr);

struct ml_expr_t *ml_expr_id(char *id, struct ml_tag_t tag);
struct ml_expr_t *ml_expr_set(struct ml_set_t set);
struct ml_expr_t *ml_expr_func(struct ml_pat_t *pat, struct ml_expr_t *expr);
struct ml_expr_t *ml_expr_app(struct ml_expr_t *func, struct ml_expr_t *value);
struct ml_expr_t *ml_expr_let(struct ml_pat_t *pat, struct ml_expr_t *value, struct ml_expr_t *expr);
struct ml_expr_t *ml_expr_cond(struct ml_expr_t *eval, struct ml_expr_t *onfalse, struct ml_expr_t *ontrue);
struct ml_expr_t *ml_expr_match(struct ml_match_t *match);
struct ml_expr_t *ml_expr_value(struct ml_value_t *value);

struct ml_value_t *ml_expr_eval(struct ml_expr_t *expr, struct ml_env_t *env, char **err);

void ml_expr_print(struct ml_expr_t *expr, FILE *file);


/**
 * Delete an expression if non-null.
 *   @expr: The expression
 */

static inline void ml_expr_erase(struct ml_expr_t *expr)
{
	if(expr != NULL)
		ml_expr_delete(expr);
}


/*
 * set declarations
 */

struct ml_set_t ml_set_new(void);
struct ml_set_t ml_set_new2(struct ml_expr_t *left, struct ml_expr_t *right);
struct ml_set_t ml_set_copy(struct ml_set_t set);
void ml_set_delete(struct ml_set_t set);
void ml_set_add(struct ml_set_t *set, struct ml_expr_t *expr);

/*
 * match declaratons
 */

struct ml_match_t *ml_match_new(struct ml_expr_t *expr);
struct ml_match_t *ml_match_copy(struct ml_match_t *match);
void ml_match_delete(struct ml_match_t *match);

void ml_match_append(struct ml_match_t *match, struct ml_pat_t *pat, struct ml_expr_t *expr);


/**
 * Copy a boxed value.
 *   @box: The boxed value.
 *   &returns: The copy.
 */

static inline struct ml_box_t ml_box_copy(struct ml_box_t box)
{
	return (struct ml_box_t){ box.iface->copy(box.ref), box.iface };
}

/**
 * Delete a boxed value.
 *   @box: The boxed value.
 */

static inline void ml_box_delete(struct ml_box_t box)
{
	box.iface->delete(box.ref);
}

#endif
