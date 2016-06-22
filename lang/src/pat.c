#include "common.h"


/*
 * local declarations
 */
static void pat_proc(struct io_file_t file, void *arg);


/**
 * Create a new pattern.
 *   @type: The type.
 *   @data: Consumed. The data.
 *   @tag: Consumed. The tag.
 *   &returns: The pattern.
 */
struct ml_pat_t *ml_pat_new(enum ml_pat_e type, union ml_pat_u data, struct ml_tag_t tag)
{
	struct ml_pat_t *pat;

	pat = malloc(sizeof(struct ml_pat_t));
	pat->type = type;
	pat->data = data;
	pat->tag = tag;
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
	struct ml_pat_t *head, **copy = &head;

	while(pat != NULL) {
		switch(pat->type) {
		case ml_pat_value_v:
			*copy = ml_pat_value(ml_value_copy(pat->data.value), ml_tag_copy(pat->tag));
			break;

		case ml_pat_var_v:
			*copy = ml_pat_var(strdup(pat->data.var), ml_tag_copy(pat->tag));
			break;

		case ml_pat_tuple_v:
			*copy = ml_pat_tuple(ml_pat_copy(pat->data.tuple), ml_tag_copy(pat->tag));
			break;

		case ml_pat_cons_v:
			*copy = ml_pat_cons(ml_pat_copy(pat->data.tuple), ml_tag_copy(pat->tag));
			break;
		}

		pat = pat->next;
		copy = &(*copy)->next;
	}

	*copy = NULL;

	return head;
}

/**
 * Delete an pattern.
 *   @pat: The pattern.
 */
void ml_pat_delete(struct ml_pat_t *pat)
{
	struct ml_pat_t *next;

	while(pat != NULL) {
		next = pat->next;

		switch(pat->type) {
		case ml_pat_value_v:
			ml_value_delete(pat->data.value);
			break;

		case ml_pat_var_v:
			free(pat->data.var);
			break;

		case ml_pat_tuple_v:
		case ml_pat_cons_v:
			ml_pat_delete(pat->data.tuple);
			break;
		}

		ml_tag_delete(pat->tag);
		free(pat);

		pat = next;
	}
}


/**
 * Create a value pattern.
 *   @value: Consumed. The value.
 *   @tag: Consumed. The tag.
 *   &returns: The pattern.
 */
struct ml_pat_t *ml_pat_value(struct ml_value_t *value, struct ml_tag_t tag)
{
	return ml_pat_new(ml_pat_value_v, (union ml_pat_u){ .value = value }, tag);
}

/**
 * Create a variable pattern.
 *   @var: Consumed. The variable identifier.
 *   @tag: Consumed. The tag.
 *   &returns: The pattern.
 */
struct ml_pat_t *ml_pat_var(char *id, struct ml_tag_t tag)
{
	return ml_pat_new(ml_pat_var_v, (union ml_pat_u){ .var = id }, tag);
}

/**
 * Create a tuple pattern.
 *   @tuple: Consumed. The tuple pattern.
 *   @tag: Consumed. The tag.
 *   &returns: The pattern.
 */
struct ml_pat_t *ml_pat_tuple(struct ml_pat_t *tuple, struct ml_tag_t tag)
{
	return ml_pat_new(ml_pat_tuple_v, (union ml_pat_u){ .tuple = tuple }, tag);
}

/**
 * Create an cons pattern.
 *   @pat: Consumed. The tuple.
 *   @tag: Consumed. The tag.
 *   &returns: The pattern.
 */
struct ml_pat_t *ml_pat_cons(struct ml_pat_t *tuple, struct ml_tag_t tag)
{
	return ml_pat_new(ml_pat_cons_v, (union ml_pat_u){ .tuple = tuple }, tag);
}


/**
 * Patterm match, adding values to the environment.
 *   @pat: The pattern.
 *   @value: The value.
 *   @env: The environment.
 *   &returns: True if matched, false otherwise.
 */
bool ml_pat_match(struct ml_pat_t *pat, struct ml_value_t *value, struct ml_env_t **env)
{
	switch(pat->type) {
	case ml_pat_value_v:
		return ml_value_cmp(pat->data.value, value) == 0;

	case ml_pat_var_v:
		ml_env_add(env, strdup(pat->data.var), ml_value_copy(value));

		return true;

	case ml_pat_tuple_v:
		if(value->type != ml_value_tuple_v)
			return false;

		{
			struct ml_link_t *link;

			pat = pat->data.tuple;
			link = value->data.list->head;

			while(pat != NULL) {
				if(link == NULL)
					return false;

				ml_pat_match(pat, link->value, env);

				pat = pat->next;
				link = link->next;
			}

			if(link != NULL)
				return false;
		}

		return true;

	case ml_pat_cons_v:
		if(value->type != ml_value_list_v)
			return false;
		else if(value->data.list->len == 0)
			return false;

		if(!ml_pat_match(pat->data.tuple, value->data.list->head->value, env))
			return false;

		{
			bool suc;
			struct ml_value_t *copy;

			copy = ml_value_copy(value);
			ml_list_remove(copy->data.list, copy->data.list->head);
			suc = ml_pat_match(pat->data.tuple->next, copy, env);
			ml_value_delete(copy);

			return suc;
		}
	}

	fatal("Invalid pattern type.");
}


/**
 * Print a pattern.
 *   @pat: The pattern.
 *   @file: The output ifle.
 */
void ml_pat_print(const struct ml_pat_t *pat, struct io_file_t file)
{
	while(true) {
		ml_pat_print1(pat, file);

		pat = pat->next;
		if(pat == NULL)
			break;

		hprintf(file, " ");
	}
}

/**
 * Print a single pattern.
 *   @pat: The pattern.
 *   @file: The output ifle.
 */
void ml_pat_print1(const struct ml_pat_t *pat, struct io_file_t file)
{
	switch(pat->type) {
	case ml_pat_value_v:
		ml_value_print(pat->data.value, file);
		break;

	case ml_pat_var_v:
		hprintf(file, "%s", pat->data.var);
		break;

	case ml_pat_tuple_v:
		{
			struct ml_pat_t *elem = pat->data.tuple;

			hprintf(file, "(");
			while(true) {
				ml_pat_print1(elem, file);

				elem = elem->next;
				if(elem == NULL)
					break;

				hprintf(file, ",");
			}

			hprintf(file, ")");
		}
		break;

	case ml_pat_cons_v:
		hprintf(file, "Cons(%C)", ml_pat_chunk(pat->data.tuple));
		break;
	}
}

/**
 * Retrieve a chunk for a pattern.
 *   @value: The pattern.
 *   &returns: The chunk.
 */
struct io_chunk_t ml_pat_chunk(const struct ml_pat_t *pat)
{
	return (struct io_chunk_t){ pat_proc, (void *)pat };
}
static void pat_proc(struct io_file_t file, void *arg)
{
	ml_pat_print(arg, file);
}
