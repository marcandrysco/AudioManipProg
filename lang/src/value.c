#include "common.h"


/*
 * local declarations
 */
static void value_proc(struct io_file_t file, void *arg);


/**
 * Create a new value.
 *   @type: The type.
 *   @data: Consumed. The data.
 *   @tag: Consumed. The tag.
 */
struct ml_value_t *ml_value_new(enum ml_value_e type, union ml_value_u data, struct ml_tag_t tag)
{
	struct ml_value_t *value;

	value = malloc(sizeof(struct ml_value_t));
	value->type = type;
	value->data = data;
	value->tag = tag;

	return value;
}

/**
 * Copy a value.
 *   @value: The original value.
 *   &returns: The copy.
 */
struct ml_value_t *ml_value_copy(struct ml_value_t *value)
{
	struct ml_tag_t tag;

	tag = ml_tag_copy(value->tag);

	switch(value->type) {
	case ml_value_nil_v: return ml_value_nil(tag);
	case ml_value_bool_v: return ml_value_bool(value->data.flag, tag);
	case ml_value_num_v: return ml_value_num(value->data.num, tag);
	case ml_value_flt_v: return ml_value_flt(value->data.flt, tag);
	case ml_value_str_v: return ml_value_str(strdup(value->data.str), tag);
	case ml_value_tuple_v: return ml_value_tuple(ml_list_copy(value->data.list), tag);
	case ml_value_list_v: return ml_value_list(ml_list_copy(value->data.list), tag);
	case ml_value_closure_v: return ml_value_closure(ml_closure_copy(value->data.closure), tag);
	case ml_value_box_v: return ml_value_box(ml_box_copy(value->data.box), tag);
	case ml_value_eval_v: return ml_value_eval(value->data.eval, tag);
	}

	fatal("Invalid value type.");
}

/**
 * Delete a value.
 *   @value: The value.
 */
void ml_value_delete(struct ml_value_t *value)
{
	switch(value->type) {
	case ml_value_nil_v: break;
	case ml_value_bool_v: break;
	case ml_value_num_v: break;
	case ml_value_flt_v: break;
	case ml_value_str_v: free(value->data.str); break;
	case ml_value_tuple_v: ml_list_delete(value->data.list); break;
	case ml_value_list_v: ml_list_delete(value->data.list); break;
	case ml_value_closure_v: ml_closure_delete(value->data.closure); break;
	case ml_value_box_v: ml_box_delete(value->data.box); break;
	case ml_value_eval_v: break;
	}

	ml_tag_delete(value->tag);
	free(value);
}


/**
 * Create a nil value.
 *   @tag: Consumed. The tag.
 *   &returns: The value.
 */
struct ml_value_t *ml_value_nil(struct ml_tag_t tag)
{
	return ml_value_new(ml_value_nil_v, (union ml_value_u){}, tag);
}

/**
 * Create a boolean value.
 *   @flag: The boolean flag.
 *   @tag: Consumed. The tag.
 *   &returns: The value.
 */
struct ml_value_t *ml_value_bool(bool flag, struct ml_tag_t tag)
{
	return ml_value_new(ml_value_bool_v, (union ml_value_u){ .flag = flag }, tag);
}

/**
 * Create a number value.
 *   @num: The number.
 *   @tag: Consumed. The tag.
 *   &returns: The value.
 */
struct ml_value_t *ml_value_num(int num, struct ml_tag_t tag)
{
	return ml_value_new(ml_value_num_v, (union ml_value_u){ .num = num }, tag);
}

/**
 * Create a floating-point value.
 *   @flt: The floating-point number.
 *   @tag: Consumed. The tag.
 *   &returns: The value.
 */
struct ml_value_t *ml_value_flt(double flt, struct ml_tag_t tag)
{
	return ml_value_new(ml_value_flt_v, (union ml_value_u){ .flt = flt }, tag);
}

/**
 * Create a string value.
 *   @str: Consumed. The string.
 *   @tag: Consumed. The tag.
 *   &returns: The value.
 */
struct ml_value_t *ml_value_str(char *str, struct ml_tag_t tag)
{
	return ml_value_new(ml_value_str_v, (union ml_value_u){ .str = str }, tag);
}

/**
 * Create a tuple value.
 *   @list: Consumed. The list.
 *   @tag: Consumed. The tag.
 *   &returns: The value.
 */
struct ml_value_t *ml_value_tuple(struct ml_list_t *list, struct ml_tag_t tag)
{
	return ml_value_new(ml_value_tuple_v, (union ml_value_u){ .list = list }, tag);
}

/**
 * Create a list value.
 *   @list: Consumed. The list.
 *   @tag: Consumed. The tag.
 *   &returns: The value.
 */
struct ml_value_t *ml_value_list(struct ml_list_t *list, struct ml_tag_t tag)
{
	return ml_value_new(ml_value_list_v, (union ml_value_u){ .list = list }, tag);
}

/**
 * Create a closure value.
 *   @closure: Consumed. The closure.
 *   @tag: Consumed. The tag.
 *   &returns: The value.
 */
struct ml_value_t *ml_value_closure(struct ml_closure_t *closure, struct ml_tag_t tag)
{
	return ml_value_new(ml_value_closure_v, (union ml_value_u){ .closure = closure }, tag);
}

/**
 * Create a boxed value.
 *   @boxed: Consumed. The boxed value.
 *   @tag: Consumed. The tag.
 *   &returns: The value.
 */
struct ml_value_t *ml_value_box(struct ml_box_t box, struct ml_tag_t tag)
{
	return ml_value_new(ml_value_box_v, (union ml_value_u){ .box = box }, tag);
}

/**
 * Create an evaluator value.
 *   @eval: The evaluator.
 *   @tag: Consumed. The tag.
 *   &returns: The value.
 */
struct ml_value_t *ml_value_eval(ml_eval_f eval, struct ml_tag_t tag)
{
	return ml_value_new(ml_value_eval_v, (union ml_value_u){ .eval = eval }, tag);
}


/**
 * Compare two values.
 *   @left: The left value.
 *   @right: The right value.
 *   &returns: Their order.
 */
int ml_value_cmp(const struct ml_value_t *left, const struct ml_value_t *right)
{
	if(left->type < right->type)
		return 1;
	else if(left->type > right->type)
		return -1;

	switch(left->type) {
	case ml_value_nil_v:
		return 0;

	case ml_value_bool_v:
		if(left->data.flag < right->data.flag)
			return 1;
		else if(left->data.flag > right->data.flag)
			return -1;
		else
			return 0;

	case ml_value_num_v:
		if(left->data.num < right->data.num)
			return 1;
		else if(left->data.num > right->data.num)
			return -1;
		else
			return 0;

	case ml_value_flt_v:
		if(left->data.flt < right->data.flt)
			return 1;
		else if(left->data.flt > right->data.flt)
			return -1;
		else
			return 0;

	case ml_value_str_v:
		return strcmp(left->data.str, right->data.str);

	case ml_value_tuple_v:
	case ml_value_list_v:
		{
			int cmp;
			struct ml_link_t *a, *b;

			for(a = left->data.list->head, b = right->data.list->head; a && b; a = a->next, b = b->next) {
				cmp = ml_value_cmp(a->value, b->value);
				if(cmp != 0)
					return cmp;
			}

			if(a != NULL)
				return 1;
			else if(b != NULL)
				return -1;
			else
				return 0;
		}

	case ml_value_closure_v:
		return 1; // TODO

	case ml_value_box_v:
		if(left->data.box.iface < right->data.box.iface)
			return 1;
		else if(right->data.box.iface > left->data.box.iface)
			return -1;
		else if(left->data.box.ref < right->data.box.ref)
			return 1;
		else if(right->data.box.ref > left->data.box.ref)
			return -1;
		else
			return 0;

	case ml_value_eval_v:
		if(left->data.eval < right->data.eval)
			return 1;
		else if(left->data.eval > right->data.eval)
			return -1;
		else
			return 0;

	}

	fatal("Invalid value type.");
}


/**
 * Print a value.
 *   @value: The value.
 *   @file: The output ifle.
 */
void ml_value_print(const struct ml_value_t *value, struct io_file_t file)
{
	switch(value->type) {
	case ml_value_nil_v:
		hprintf(file, "()");
		break;

	case ml_value_bool_v:
		hprintf(file, "%s", value->data.flag ? "true" : "false");
		break;

	case ml_value_num_v:
		hprintf(file, "%d", value->data.num);
		break;

	case ml_value_flt_v:
		hprintf(file, "%g", value->data.flt);
		break;

	case ml_value_str_v:
		hprintf(file, "\"%s\"", value->data.str);
		break;

	case ml_value_tuple_v:
	case ml_value_list_v:
		{
			struct ml_link_t *link;

			hprintf(file, (value->type == ml_value_tuple_v) ? "(" : "[");

			for(link = value->data.list->head; link != NULL; link = link->next)
				hprintf(file, "%s%C", (link != value->data.list->head) ? "," : "", ml_value_chunk(link->value));

			hprintf(file, (value->type == ml_value_tuple_v) ? ")" : "]");
		}
		break;

	case ml_value_closure_v:
		hprintf(file, "closure");
		//if(value->data.closure->rec != NULL)
			//hprintf(file, "closure(\"%s\",%C,%C)", value->data.closure->rec, ml_pat_chunk(value->data.closure->pat), ml_expr_chunk(value->data.closure->expr));

		break;

	case ml_value_box_v:
		hprintf(file, "box");
		break;

	case ml_value_eval_v:
		hprintf(file, "eval");
		break;
	}
}

/**
 * Retrieve a chunk for a value.
 *   @value: The value.
 *   &returns: The chunk.
 */
struct io_chunk_t ml_value_chunk(const struct ml_value_t *value)
{
	return (struct io_chunk_t){ value_proc, (void *)value };
}
static void value_proc(struct io_file_t file, void *arg)
{
	ml_value_print(arg, file);
}


/**
 * Create a new list.
 *   &returns: The list.
 */
struct ml_list_t *ml_list_new(void)
{
	struct ml_list_t *list;

	list = malloc(sizeof(struct ml_list_t));
	list->head = list->tail = NULL;
	list->len = 0;

	return list;
}

/**
 * Create a list from an argument list.
 *   @value: The value.
 *   @...: The null-terminated list of values.
 *   &returns: The list.
 */
struct ml_list_t *ml_list_newl(struct ml_value_t *value, ...)
{
	va_list args;
	struct ml_list_t *list;

	list = ml_list_new();

	va_start(args, value);

	while(value != NULL) {
		ml_list_append(list, value);
		value = va_arg(args, struct ml_value_t *);
	}

	va_end(args);

	return list;
}

/**
 * Copy a list.
 *   @list: The list.
 *   &returns: The copy.
 */
struct ml_list_t *ml_list_copy(struct ml_list_t *list)
{
	struct ml_list_t *copy;
	struct ml_link_t *link;

	copy = ml_list_new();

	for(link = list->head; link != NULL; link = link->next)
		ml_list_append(copy, ml_value_copy(link->value));

	return copy;
}

/**
 * Merge two lists.
 *   @left: Consumed. The left list.
 *   @right: Consumed. The right list.
 *   &returns: The merged list.
 */
struct ml_list_t *ml_list_merge(struct ml_list_t *left, struct ml_list_t *right)
{
	struct ml_list_t *list;

	list = malloc(sizeof(struct ml_list_t));

	if(left->head == NULL) {
		list->head = right->head;
		list->tail = right->tail;
		list->len = right->len;
	}
	else if(right->head == NULL) {
		list->head = left->head;
		list->tail = left->tail;
		list->len = left->len;
	}
	else {
		left->tail->next = right->head;
		right->head->prev = left->tail;

		list->head = left->head;
		list->tail = right->tail;
		list->len = left->len + right->len;
	}

	free(left);
	free(right);

	return list;
}

/**
 * Delete a list.
 *   @list: The list.
 */
void ml_list_delete(struct ml_list_t *list)
{
	struct ml_link_t *cur, *next;

	for(cur = list->head; cur != NULL; cur = next) {
		next = cur->next;

		ml_value_delete(cur->value);
		free(cur);
	}

	free(list);
}


/**
 * Retrieve a link at a given index.
 *   @list: The list.
 *   @idx: The index.
 *   &returns: The link or null.
 */
struct ml_link_t *ml_list_get(struct ml_list_t *list, unsigned int idx)
{
	struct ml_link_t *link;

	link = list->head;
	while(idx-- > 0) {
		link = link->next;
		if(link == NULL)
			return NULL;
	}

	return link;
}

/**
 * Retrieve a value at a given index.
 *   @list: The list.
 *   @idx: The index.
 *   &returns: The value or null.
 */
struct ml_value_t *ml_list_getv(struct ml_list_t *list, unsigned int idx)
{
	struct ml_link_t *link;

	link = list->head;
	while(idx-- > 0) {
		link = link->next;
		if(link == NULL)
			return NULL;
	}

	return link->value;
}


/**
 * Prepend a value onto the list.
 *   @list: The list.
 *   @value: Consumed. The value.
 */
void ml_list_prepend(struct ml_list_t *list, struct ml_value_t *value)
{
	struct ml_link_t *link;

	link = malloc(sizeof(struct ml_link_t));
	link->value = value;
	link->next = list->head;
	link->prev = NULL;
	*(list->head ? &list->head->prev : &list->tail) = link;

	list->len++;
	list->head = link;
}

/**
 * Append a value onto the list.
 *   @list: The list.
 *   @value: Consumed. The value.
 */
void ml_list_append(struct ml_list_t *list, struct ml_value_t *value)
{
	struct ml_link_t *link;

	link = malloc(sizeof(struct ml_link_t));
	link->value = value;
	link->prev = list->tail;
	link->next = NULL;
	*(list->tail ? &list->tail->next : &list->head) = link;

	list->len++;
	list->tail = link;
}


/**
 * Create a closure.
 *   @rec: Optional. The recursive name.
 *   @env: The environment.
 *   @pat: The pattern.
 *   @expr: The expression.
 */
struct ml_closure_t *ml_closure_new(char *rec, struct ml_env_t *env, struct ml_pat_t *pat, struct ml_expr_t *expr)
{
	struct ml_closure_t *closure;

	closure = malloc(sizeof(struct ml_closure_t));
	closure->rec = rec;
	closure->env = env;
	closure->pat = pat;
	closure->expr = expr;

	return closure;
}

/**
 * Copy a closure.
 *   @closure: The closure.
 *   &returns: The copy.
 */
struct ml_closure_t *ml_closure_copy(struct ml_closure_t *closure)
{
	return ml_closure_new(closure->rec ? strdup(closure->rec) : NULL, ml_env_copy(closure->env), ml_pat_copy(closure->pat), ml_expr_copy(closure->expr));
}

/**
 * Delete a closure.
 *   @closure: The closure.
 */
void ml_closure_delete(struct ml_closure_t *closure)
{
	erase(closure->rec);
	ml_env_delete(closure->env);
	ml_pat_delete(closure->pat);
	ml_expr_delete(closure->expr);
	free(closure);
}


/**
 * Create a new boxed value.
 *   @ref: The reference.
 *   @iface: The interface.
 *   &returns: The boxed value.
 */
struct ml_box_t ml_box_new(void *ref, const struct ml_box_i *iface)
{
	return (struct ml_box_t){ ref, iface };
}

/**
 * Copy a boxed value.
 *   @box: The original boxed value.
 *   &returns: The copied value.
 */
struct ml_box_t ml_box_copy(struct ml_box_t box)
{
	return (struct ml_box_t){ box.iface->copy(box.ref), box.iface };
}

/**
 * Delete a boxed value.
 *   @box: The boxed value.
 */
void ml_box_delete(struct ml_box_t box)
{
	box.iface->delete(box.ref);
}
