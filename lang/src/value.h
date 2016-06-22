#ifndef VALUE_H
#define VALUE_H

/**
 * Box interface.
 *   @copy: Copy.
 *   @delete: Deletion.
 */

struct ml_box_i {
	copy_f copy;
	delete_f delete;
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
 * Value enumerator.
 *   @ml_value_nil_v: Nil.
 *   @ml_value_bool_v: Boolean.
 *   @ml_value_num_v: Integer number.
 *   @ml_value_flt_v: Floating-point number.
 *   @ml_value_str_v: String.
 *   @ml_value_list_v: List.
 *   @ml_value_tuple_v: Tuple.
 *   @ml_value_closure_v: Closure.
 *   @ml_value_box_v: Boxed value.
 *   @ml_value_eval_v: Evaluator.
 */
enum ml_value_e {
	ml_value_nil_v,
	ml_value_bool_v,
	ml_value_num_v,
	ml_value_flt_v,
	ml_value_str_v,
	ml_value_list_v,
	ml_value_tuple_v,
	ml_value_closure_v,
	ml_value_box_v,
	ml_value_eval_v
};

/**
 * Value data union.
 *   @flag: Boolean flag.
 *   @num: Integer number.
 *   @flt: Floating-point number.
 *   @str: String.
 *   @list: List.
 *   @closure: Closure.
 *   @box: Boxed value.
 *   @eval: Native evaluator.
 */
union ml_value_u {
	bool flag;
	int num;
	double flt;
	char *str;
	struct ml_list_t *list;
	struct ml_closure_t *closure;
	struct ml_box_t box;
	ml_eval_f eval;
};

/**
 * Value structure.
 *   @tag: The tag.
 *   @type: The type.
 *   @data: The data.
 */
struct ml_value_t {
	struct ml_tag_t tag;

	enum ml_value_e type;
	union ml_value_u data;
};


/**
 * List structure.
 *   @head, tail: The head and tail links.
 *   @len: The length.
 */
struct ml_list_t {
	struct ml_link_t *head, *tail;
	unsigned int len;
};

/**
 * List link structure.
 *   @value: The value.
 *   @prev, next: The previous and next links.
 */
struct ml_link_t {
	struct ml_value_t *value;
	struct ml_link_t *prev, *next;
};

/**
 * Closure structure.
 *   @rec: The recursive name.
 *   @env: The environment.
 *   @pat: The pattern.
 *   @expr: The expression.
 */
struct ml_closure_t {
	char *rec;
	struct ml_env_t *env;
	struct ml_pat_t *pat;
	struct ml_expr_t *expr;
};


/*
 * value declarations
 */
struct ml_value_t *ml_value_new(enum ml_value_e type, union ml_value_u data, struct ml_tag_t tag);
struct ml_value_t *ml_value_copy(struct ml_value_t *value);
void ml_value_delete(struct ml_value_t *value);

struct ml_value_t *ml_value_nil(struct ml_tag_t tag);
struct ml_value_t *ml_value_bool(bool flag, struct ml_tag_t tag);
struct ml_value_t *ml_value_num(int num, struct ml_tag_t tag);
struct ml_value_t *ml_value_flt(double flt, struct ml_tag_t tag);
struct ml_value_t *ml_value_str(char *str, struct ml_tag_t tag);
struct ml_value_t *ml_value_list(struct ml_list_t *list, struct ml_tag_t tag);
struct ml_value_t *ml_value_tuple(struct ml_list_t *list, struct ml_tag_t tag);
struct ml_value_t *ml_value_closure(struct ml_closure_t *closure, struct ml_tag_t tag);
struct ml_value_t *ml_value_box(struct ml_box_t box, struct ml_tag_t tag);
struct ml_value_t *ml_value_eval(ml_eval_f eval, struct ml_tag_t tag);

int ml_value_cmp(const struct ml_value_t *left, const struct ml_value_t *right);

void ml_value_print(const struct ml_value_t *value, struct io_file_t file);
struct io_chunk_t ml_value_chunk(const struct ml_value_t *value);

/*
 * list declarations
 */
struct ml_list_t *ml_list_new(void);
struct ml_list_t *ml_list_newl(struct ml_value_t *value, ...);
struct ml_list_t *ml_list_copy(struct ml_list_t *list);
struct ml_list_t *ml_list_merge(struct ml_list_t *left, struct ml_list_t *right);
void ml_list_delete(struct ml_list_t *list);

struct ml_link_t *ml_list_get(struct ml_list_t *list, unsigned int idx);
struct ml_value_t *ml_list_getv(struct ml_list_t *list, unsigned int idx);

void ml_list_prepend(struct ml_list_t *list, struct ml_value_t *value);
void ml_list_append(struct ml_list_t *list, struct ml_value_t *value);
void ml_list_remove(struct ml_list_t *list, struct ml_link_t *link);

/*
 * closure declarations
 */
struct ml_closure_t *ml_closure_new(char *rec, struct ml_env_t *env, struct ml_pat_t *pat, struct ml_expr_t *expr);
struct ml_closure_t *ml_closure_copy(struct ml_closure_t *closure);
void ml_closure_delete(struct ml_closure_t *closure);

/*
 * box declarations
 */
struct ml_box_t ml_box_new(void *ref, const struct ml_box_i *iface);
struct ml_box_t ml_box_copy(struct ml_box_t box);
void ml_box_delete(struct ml_box_t box);


/**
 * Delete a value if not null.
 *   @value: The value.
 */
static inline void ml_value_erase(struct ml_value_t *value)
{
	if(value != NULL)
		ml_value_delete(value);
}


/**
 * Check if a value is numeric (int or float).
 *   @value: The value.
 *   &returns: True if numeric, false otherwise.
 */
static inline bool ml_value_isnum(struct ml_value_t *value)
{
	return (value->type == ml_value_num_v) || (value->type == ml_value_flt_v);
}

/**
 * Retrieve the floating-point value from a numeric value.
 *   @value: The value. Must be numeric.
 *   &returns: The value as a double-precision float.
 */
static inline double ml_value_getflt(struct ml_value_t *value)
{
	if(value->type == ml_value_num_v)
		return value->data.num;
	else if(value->type == ml_value_flt_v)
		return value->data.flt;
	else
		fatal("Not a numeric type.");
}

#endif
