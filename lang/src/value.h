#ifndef VALUE_H
#define VALUE_H

/*
 * value declarations
 */
struct ml_value_t *ml_value_new(struct ml_tag_t tag, enum ml_value_e type, union ml_value_u data);
struct ml_value_t *ml_value_copy(struct ml_value_t *value);
void ml_value_delete(struct ml_value_t *value);

struct ml_value_t *ml_value_nil(struct ml_tag_t tag);
struct ml_value_t *ml_value_bool(bool flag);
struct ml_value_t *ml_value_num(double num);
struct ml_value_t *ml_value_str(char *str);
struct ml_value_t *ml_value_tuple(struct ml_tuple_t tuple);
struct ml_value_t *ml_value_list(struct ml_list_t list);
struct ml_value_t *ml_value_closure(struct ml_closure_t closure);
struct ml_value_t *ml_value_box(struct ml_box_t box);
struct ml_value_t *ml_value_impl(ml_impl_f impl);

int ml_value_cmp(struct ml_value_t *left, struct ml_value_t *right);

void ml_value_print(struct ml_value_t *value, FILE *file);

/**
 * Delete a value if non-null.
 *   @value: The value.
 */
static inline void ml_value_erase(struct ml_value_t *value)
{
	if(value != NULL)
		ml_value_delete(value);
}

/*
 * tuple declarations
 */
struct ml_tuple_t ml_tuple_new(void);
struct ml_tuple_t ml_tuple_new2(struct ml_value_t *left, struct ml_value_t *right);
struct ml_tuple_t ml_tuple_copy(struct ml_tuple_t tuple);
void ml_tuple_delete(struct ml_tuple_t tuple);
void ml_tuple_add(struct ml_tuple_t *tuple, struct ml_value_t *expr);

/*
 * list declaratons
 */
struct ml_list_t ml_list_new(void);
struct ml_list_t ml_list_copy(struct ml_list_t list);
void ml_list_delete(struct ml_list_t list);

void ml_list_prepend(struct ml_list_t *list, struct ml_value_t *value);
void ml_list_append(struct ml_list_t *list, struct ml_value_t *value);
struct ml_value_t *ml_list_remove(struct ml_list_t *list, struct ml_link_t *link);

/*
 * closure declarations
 */
struct ml_closure_t ml_closure_copy(struct ml_closure_t closure);
void ml_closure_delete(struct ml_closure_t closure);

static inline struct ml_closure_t ml_closure(struct ml_env_t *env, struct ml_pat_t *pat, char *rec, struct ml_expr_t *expr)
{
	return (struct ml_closure_t){ env, pat, rec, expr };
}

#endif
