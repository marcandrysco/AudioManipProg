#include "common.h"


bool ml_match_cons(struct ml_value_t *value, struct ml_value_t *head, struct ml_value_t *tail)
{
	if(value->type != ml_value_list_e)
		return false;

	return true;
}


/**
 * Create a new pattern.
 *   @type: The type.
 *   @data: The data.
 *   &returns: The pattern.
 */

struct ml_pat_t *ml_pat_new(enum ml_pat_e type, union ml_pat_u data)
{
	struct ml_pat_t *pat;

	pat = malloc(sizeof(struct ml_pat_t));
	pat->type = type;
	pat->data = data;
	pat->next = NULL;

	return pat;
}

/**
 * Copy a pattern.
 *   @pat: The pattern.
 *   &returns: The copy.
 */

struct ml_pat_t *ml_pat_copy(struct ml_pat_t *pat)
{
	struct ml_pat_t *head = NULL, **copy = &head;

	while(pat != NULL) {
		*copy = malloc(sizeof(struct ml_pat_t));
		(*copy)->type = pat->type;

		switch(pat->type) {
		case ml_pat_id_e:
			(*copy)->data.id = strdup(pat->data.id);
			break;

		case ml_pat_tuple_e:
			(*copy)->data.tuple = ml_pat_copy(pat->data.tuple);
			break;

		case ml_pat_list_e:
			(*copy)->data.list[0] = ml_pat_copy(pat->data.list[0]);
			(*copy)->data.list[1] = ml_pat_copy(pat->data.list[1]);
			break;
		}

		pat = pat->next;
		copy = &(*copy)->next;
	}

	*copy = NULL;

	return head;
}

/**
 * Delete a pattern.
 *   @pat: The pattern.
 */

void ml_pat_delete(struct ml_pat_t *pat)
{
	struct ml_pat_t *next;

	while(pat != NULL) {
		next = pat->next;

		switch(pat->type) {
		case ml_pat_id_e:
			free(pat->data.id);
			break;

		case ml_pat_tuple_e:
			ml_pat_delete(pat->data.tuple);
			break;

		case ml_pat_list_e:
			ml_pat_delete(pat->data.list[0]);
			ml_pat_delete(pat->data.list[1]);
			break;
		}

		free(pat);
		pat = next;
	}
}


/**
 * Create an identifier pattern.
 *   @id: Consumed. The identifier.
 *   &returns: The pattern.
 */

struct ml_pat_t *ml_pat_id(char *id)
{
	return ml_pat_new(ml_pat_id_e, (union ml_pat_u){ .id = id });
}

/**
 * Create a tuple pattern.
 *   @tuple: Consumed. The tuple pattern.
 *   &returns: The pattern.
 */

struct ml_pat_t *ml_pat_tuple(struct ml_pat_t *tuple)
{
	return ml_pat_new(ml_pat_tuple_e, (union ml_pat_u){ .tuple = tuple });
}

/**
 * Create a list pattern.
 *   @head: Consumed. The head pattern.
 *   @tail: Consumed. The tail pattern.
 *   &returns: The pattern.
 */

struct ml_pat_t *ml_pat_list(struct ml_pat_t *head, struct ml_pat_t *tail)
{
	return ml_pat_new(ml_pat_list_e, (union ml_pat_u){ .list = { head, tail } });
}


/**
 * Print a pattern.
 *   @pat: The pattern.
 *   @file: The file.
 */

void ml_pat_print(struct ml_pat_t *pat, FILE *file)
{
	while(pat != NULL) {
		switch(pat->type) {
		case ml_pat_id_e:
			fprintf(file, "id(%s)", pat->data.id);
			break;

		case ml_pat_tuple_e:
			ml_fprintf(file, "tuple(%Mp)", pat->data.tuple);
			break;

		case ml_pat_list_e:
			ml_fprintf(file, "list(%Mp,%Mp)", pat->data.list[0], pat->data.list[1]);
			break;
		}

		pat = pat->next;

		if(pat != NULL)
			printf(",");
	}
}


/**
 * Peform a pattern match, adding to an environment.
 *   @env: The environment.
 *   @pat: The pattern.
 *   @value: The value.
 *   &returns: True if successful, false otherwise.
 */

bool ml_pat_match(struct ml_env_t *env, struct ml_pat_t *pat, struct ml_value_t *value)
{
	switch(pat->type) {
	case ml_pat_id_e:
		ml_env_add(env, strdup(pat->data.id), ml_value_copy(value));
		return true;

	case ml_pat_tuple_e:
		if(value->type != ml_value_tuple_e)
			return false;

		{
			unsigned int i;
			struct ml_pat_t *sub = pat->data.tuple;
			struct ml_tuple_t tuple = value->data.tuple;

			for(i = 0; i < tuple.len; i++) {
				if(sub == NULL)
					return false;

				if(!ml_pat_match(env, sub, tuple.value[i]))
					return false;

				sub = sub->next;
			}

			return sub == NULL;
		}

		return false;

	case ml_pat_list_e:
		if(value->type != ml_value_list_e)
			return false;

		if(value->data.list.head == NULL)
			return false;

		if(!ml_pat_match(env, pat->data.list[0], value->data.list.head->value))
			return false;

		{
			bool suc;
			struct ml_list_t list;

			list = ml_list_copy(value->data.list);
			ml_value_delete(ml_list_remove(&list, list.head));

			value = ml_value_list(list);
			suc = ml_pat_match(env, pat->data.list[1], value);
			ml_value_delete(value);

			return suc;
		}
	}

	fprintf(stderr, "Invalid pattern type.\n"), abort();
}


/**
 * Peform a pattern match, adding to an environment.
 *   @env: The environment.
 *   @pat: The pattern.
 *   @value: The value.
 *   &returns: True if successful, false otherwise.
 */

bool ml_match_pat(struct ml_env_t *env, struct ml_expr_t *pat, struct ml_value_t *value)
{
	switch(pat->type) {
	case ml_expr_id_e:
		ml_env_add(env, strdup(pat->data.id), ml_value_copy(value));
		return true;

	case ml_expr_set_e:
		if(value->type != ml_value_tuple_e)
			return false;

		{
			unsigned int i;
			struct ml_tuple_t tuple = value->data.tuple;

			if(tuple.len != pat->data.set.len)
				return false;

			for(i = 0; i < tuple.len; i++) {
				if(!ml_match_pat(env, pat->data.set.expr[i], tuple.value[i]))
					return false;
			}

			return true;
		}

	case ml_expr_app_e:
		if(pat->data.app.func->type != ml_expr_value_e)
			return false;

		{
			struct ml_value_t *func = pat->data.app.func->data.value;

			if(func->type == ml_value_impl_e) {
				if(func->data.impl == ml_eval_cons) {
					struct ml_set_t set;

					if(value->type != ml_value_list_e)
						return false;

					if(value->data.list.head == NULL)
						return false;

					if(pat->data.app.value->type != ml_expr_set_e)
						return false;

					set = pat->data.app.value->data.set;
					if(set.len != 2)
						return false;

					if(!ml_match_pat(env, set.expr[0], value->data.list.head->value))
						return false;

					{
						bool suc;
						struct ml_list_t tail;
						struct ml_value_t *sub;

						tail = ml_list_copy(value->data.list);
						ml_value_delete(ml_list_remove(&tail, tail.tail));

						sub = ml_value_list(tail);
						suc = ml_match_pat(env, set.expr[1], sub);
						ml_value_delete(sub);

						return suc;
					}
				}
				else
					return false;
			}
			else
				return false;
		}
		break;

	case ml_expr_func_e:
	case ml_expr_cond_e:
	case ml_expr_value_e:
	case ml_expr_let_e:
		return false;
	}

	fprintf(stderr, "invalid expression type.\n"), abort();
}
