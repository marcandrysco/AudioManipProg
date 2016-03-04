#include "common.h"


/**
 * Create a value.
 *   @type: The type.
 *   @data: The data.
 *   &returns: The value.
 */

struct ml_value_t *ml_value_new(enum ml_value_e type, union ml_value_u data)
{
	struct ml_value_t *value;

	value = malloc(sizeof(struct ml_value_t));
	value->type = type;
	value->data = data;

	return value;
}

/**
 * Copy a value.
 *   @value: The value.
 *   &returns: The copy.
 */

struct ml_value_t *ml_value_copy(struct ml_value_t *value)
{
	switch(value->type) {
	case ml_value_nil_e:
		return ml_value_nil();

	case ml_value_bool_e:
		return ml_value_bool(value->data.flag);

	case ml_value_num_e:
		return ml_value_num(value->data.num);

	case ml_value_str_e:
		return ml_value_str(strdup(value->data.str));

	case ml_value_tuple_e:
		return ml_value_tuple(ml_tuple_copy(value->data.tuple));

	case ml_value_list_e:
		return ml_value_list(ml_list_copy(value->data.list));

	case ml_value_closure_e:
		return ml_value_closure(ml_closure_copy(value->data.closure));

	case ml_value_box_e:
		return ml_value_box(ml_box_copy(value->data.box));

	case ml_value_impl_e:
		return ml_value_impl(value->data.impl);
	}

	fprintf(stderr, "Invalid value.\n"), abort();
}

/**
 * Delete a value.
 *   @value: The value.
 */

void ml_value_delete(struct ml_value_t *value)
{
	switch(value->type) {
	case ml_value_nil_e:
	case ml_value_bool_e:
	case ml_value_num_e:
	case ml_value_impl_e:
		break;

	case ml_value_str_e:
		free(value->data.str);
		break;

	case ml_value_tuple_e:
		ml_tuple_delete(value->data.tuple);
		break;

	case ml_value_list_e:
		ml_list_delete(value->data.list);
		break;

	case ml_value_closure_e:
		ml_closure_delete(value->data.closure);
		break;

	case ml_value_box_e:
		ml_box_delete(value->data.box);
		break;
	}

	free(value);
}


/**
 * Create a nil value.
 *   &returns: The value.
 */

struct ml_value_t *ml_value_nil(void)
{
	return ml_value_new(ml_value_nil_e, (union ml_value_u){  });
}

/**
 * Create a boolean value.
 *   @num: The number.
 *   &returns: The value.
 */

struct ml_value_t *ml_value_bool(bool flag)
{
	return ml_value_new(ml_value_bool_e, (union ml_value_u){ .flag = flag });
}

/**
 * Create a number value.
 *   @num: The number.
 *   &returns: The value.
 */

struct ml_value_t *ml_value_num(double num)
{
	return ml_value_new(ml_value_num_e, (union ml_value_u){ .num = num });
}

/**
 * Create a string value.
 *   @str: Consumed. The string.
 *   &returns: The value.
 */

struct ml_value_t *ml_value_str(char *str)
{
	return ml_value_new(ml_value_str_e, (union ml_value_u){ .str = str });
}

/**
 * Create a tuple value.
 *   @tuple: Consumed. The tuple.
 *   &returns: The value.
 */

struct ml_value_t *ml_value_tuple(struct ml_tuple_t tuple)
{
	return ml_value_new(ml_value_tuple_e, (union ml_value_u){ .tuple = tuple });
}

/**
 * Create a list value.
 *   @list: Consumed. The list.
 *   &returns: The value.
 */

struct ml_value_t *ml_value_list(struct ml_list_t list)
{
	return ml_value_new(ml_value_list_e, (union ml_value_u){ .list = list });
}

/**
 * Create a closure.
 *   @box: Consumed. The closure.
 *   &returns: The value.
 */

struct ml_value_t *ml_value_closure(struct ml_closure_t closure)
{
	return ml_value_new(ml_value_closure_e, (union ml_value_u){ .closure = closure });
}

/**
 * Create a boxed value.
 *   @box: Consumed. The boxed value.
 *   &returns: The value.
 */

struct ml_value_t *ml_value_box(struct ml_box_t box)
{
	return ml_value_new(ml_value_box_e, (union ml_value_u){ .box = box });
}

/**
 * Create a native implementation value.
 *   @impl: The native implementation.
 *   &returns: The value.
 */

struct ml_value_t *ml_value_impl(ml_impl_f impl)
{
	return ml_value_new(ml_value_impl_e, (union ml_value_u){ .impl = impl });
}


/**
 * Compare two value for their order.
 *   @left: The left value.
 *   @right: The right value.
 *   &returns: Their order.
 */

int ml_value_cmp(struct ml_value_t *left, struct ml_value_t *right)
{
	if(left->type > right->type)
		return 1;
	else if(left->type < right->type)
		return -1;

	switch(left->type) {
	case ml_value_nil_e:
		return 0;

	case ml_value_bool_e:
		if(left->data.flag > right->data.flag)
			return 1;
		else if(left->data.flag < right->data.flag)
			return -1;
		else
			return 0;

	case ml_value_num_e:
		if(left->data.num > right->data.num)
			return 1;
		else if(left->data.num < right->data.num)
			return -1;
		else
			return 0;

	case ml_value_str_e:
		return strcmp(left->data.str, right->data.str);

	case ml_value_tuple_e:
		return 0;

	case ml_value_list_e:
		return 0;

	case ml_value_closure_e:
		return 0;

	case ml_value_box_e:
		return 0;

	case ml_value_impl_e:
		return 0;
	}

	fprintf(stderr, "Invalid value type.\n"), abort();
}


/**
 * Print a value.
 *   @value: The value.
 *   @file: The file.
 */

void ml_value_print(struct ml_value_t *value, FILE *file)
{
	switch(value->type) {
	case ml_value_nil_e:
		fprintf(file, "nil");
		break;

	case ml_value_bool_e:
		fprintf(file, "bool(%s)", value->data.flag ? "true" : "false");
		break;

	case ml_value_num_e:
		fprintf(file, "num(%g)", value->data.num);
		break;

	case ml_value_str_e:
		fprintf(file, "str(%s)", value->data.str);
		break;

	case ml_value_tuple_e:
		{
			unsigned int i;

			fprintf(file, "tuple(");

			for(i = 0; i < value->data.tuple.len; i++) {
				if(i > 0)
					fprintf(file, ",");

				ml_value_print(value->data.tuple.value[i], file);
			}

			fprintf(file, ")");
		}
		break;

	case ml_value_list_e:
		{
			struct ml_link_t *link;

			fprintf(file, "list(");

			for(link = value->data.list.head; link != NULL; link = link->next) {
				if(link != value->data.list.head)
					fprintf(file, ",");

				ml_value_print(link->value, file);
			}

			fprintf(file, ")");
		}
		break;

	case ml_value_closure_e:
		fprintf(file, "closure()");
		break;

	case ml_value_box_e:
		fprintf(file, "box");
		break;

	case ml_value_impl_e:
		fprintf(file, "impl");
		break;
	}
}


/**
 * Create a tuple.
 *   &returns: The tuple.
 */

struct ml_tuple_t ml_tuple_new(void)
{
	return (struct ml_tuple_t){ 0, malloc(0) };
}

/**
 * Create a tuple with two parameters.
 *   @left: The left expression.
 *   @right: The right expression.
 *   &returns: The tuple.
 */

struct ml_tuple_t ml_tuple_new2(struct ml_value_t *left, struct ml_value_t *right)
{
	struct ml_tuple_t tuple;

	tuple.len = 2;
	tuple.value = malloc(2 * sizeof(void *));
	tuple.value[0] = left;
	tuple.value[1] = right;

	return tuple;
}

/**
 * Copy a tuple.
 *   @tuple: The original tuple.
 *   &returns: The copied tuple.
 */

struct ml_tuple_t ml_tuple_copy(struct ml_tuple_t tuple)
{
	unsigned int i;
	struct ml_tuple_t copy;

	copy.len = tuple.len;
	copy.value = malloc(tuple.len * sizeof(void *));
	for(i = 0; i < tuple.len; i++)
		copy.value[i] = ml_value_copy(tuple.value[i]);

	return copy;
}

/**
 * Delete a tuple.
 *   @tuple: The tuple.
 */

void ml_tuple_delete(struct ml_tuple_t tuple)
{
	unsigned int i;

	for(i = 0; i < tuple.len; i++)
		ml_value_delete(tuple.value[i]);

	free(tuple.value);
}

/**
 * Add an value to the tuple.
 *   @tuple: The tuple.
 *   @value: Consume The value.
 */

void ml_tuple_add(struct ml_tuple_t *tuple, struct ml_value_t *value)
{
	tuple->value = realloc(tuple->value, (tuple->len + 1) * sizeof(void *));
	tuple->value[tuple->len++] = value;
}


/**
 * Create a new list.
 *   &returns: The list.
 */

struct ml_list_t ml_list_new(void)
{
	return (struct ml_list_t){ NULL, NULL };
}

/**
 * Copy a list.
 *   @list: The original list.
 *   &returns: The copied list.
 */

struct ml_list_t ml_list_copy(struct ml_list_t list)
{
	struct ml_list_t copy;
	struct ml_link_t *link;

	copy = ml_list_new();

	for(link = list.head; link != NULL; link = link->next)
		ml_list_append(&copy, ml_value_copy(link->value));

	return copy;
}

/**
 * Delete a list.
 *   @list: The list.
 */

void ml_list_delete(struct ml_list_t list)
{
	struct ml_link_t *cur, *next;

	for(cur = list.head; cur != NULL; cur = next) {
		next = cur->next;

		ml_value_delete(cur->value);
		free(cur);
	}
}


/**
 * Prepend an value onto the list.
 *   @list: The list.
 *   @value: The value.
 */

void ml_list_prepend(struct ml_list_t *list, struct ml_value_t *value)
{
	struct ml_link_t *link;

	link = malloc(sizeof(struct ml_link_t));
	link->value = value;
	link->next = list->head;
	link->prev = NULL;
	*(list->head ? &list->head->prev : &list->tail) = link;
	list->head = link;
}

/**
 * Append an value onto the list.
 *   @list: The list.
 *   @value: The value.
 */

void ml_list_append(struct ml_list_t *list, struct ml_value_t *value)
{
	struct ml_link_t *link;

	link = malloc(sizeof(struct ml_link_t));
	link->value = value;
	link->prev = list->tail;
	link->next = NULL;
	*(list->tail ? &list->tail->next : &list->head) = link;
	list->tail = link;
}

/**
 * Remove a link from the list.
 *   @list: The list.
 *   @link: The link.
 *   &returns: The removed value.
 */

struct ml_value_t *ml_list_remove(struct ml_list_t *list, struct ml_link_t *link)
{
	struct ml_value_t *value = link->value;

	if(link->next == NULL)
		list->tail = link->prev;
	else
		link->next->prev = link->prev;

	if(link->prev == NULL)
		list->head = link->next;
	else
		link->prev->next = link->next;

	free(link);

	return value;
}


/**
 * Copy a closure.
 *   @closure: The closure.
 *   &returns: The copy.
 */

struct ml_closure_t ml_closure_copy(struct ml_closure_t closure)
{
	char *rec;
	struct ml_env_t *env;
	struct ml_pat_t *pat;
	struct ml_expr_t *expr;

	env = ml_env_copy(closure.env);
	pat = ml_pat_copy(closure.pat);
	rec = closure.rec ? strdup(closure.rec) : NULL;
	expr = ml_expr_copy(closure.expr);

	return (struct ml_closure_t){ env, pat, rec, expr };
}

/**
 * Delete a closure.
 *   @closure: The closure.
 */

void ml_closure_delete(struct ml_closure_t closure)
{
	ml_env_delete(closure.env);
	ml_pat_delete(closure.pat);
	ml_expr_delete(closure.expr);

	if(closure.rec)
		free(closure.rec);
}
