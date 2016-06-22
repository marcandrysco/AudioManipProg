#ifndef PAT_H
#define PAT_H

/**
 * Pattern evaluation structure.
 *   @func: The function.
 *   @pat: The child patterns.
 */
struct ml_pat_eval_t {
	ml_eval_f func;
	struct ml_pat_t *pat;
};

/**
 * Pattern type enumerator.
 *   @ml_pat_value_v: Value.
 *   @ml_pat_var_v: Variable.
 *   @ml_pat_tuple_v: Tuple.
 *   @ml_pat_cons_v: Cons.
 */
enum ml_pat_e {
	ml_pat_value_v,
	ml_pat_var_v,
	ml_pat_tuple_v,
	ml_pat_cons_v
};

/**
 * Pattern value union.
 *   @value: Value.
 *   @var: The variable ID.
 *   @tuple: The tuple list.
 */
union ml_pat_u {
	struct ml_value_t *value;
	char *var;
	struct ml_pat_t *tuple;
};

/**
 * Pattern structure.
 *   @tag: The tag.
 *   @type: The type.
 *   @data: The data.
 *   @next: The next pattern.
 */
struct ml_pat_t {
	struct ml_tag_t tag;

	enum ml_pat_e type;
	union ml_pat_u data;

	struct ml_pat_t *next;
};


/*
 * pattern declarations
 */
struct ml_pat_t *ml_pat_new(enum ml_pat_e type, union ml_pat_u data, struct ml_tag_t tag);
struct ml_pat_t *ml_pat_copy(struct ml_pat_t *pat);
void ml_pat_delete(struct ml_pat_t *pat);

struct ml_pat_t *ml_pat_value(struct ml_value_t *value, struct ml_tag_t tag);
struct ml_pat_t *ml_pat_var(char *id, struct ml_tag_t tag);
struct ml_pat_t *ml_pat_tuple(struct ml_pat_t *tuple, struct ml_tag_t tag);
struct ml_pat_t *ml_pat_cons(struct ml_pat_t *pat, struct ml_tag_t tag);

bool ml_pat_match(struct ml_pat_t *pat, struct ml_value_t *value, struct ml_env_t **env);

void ml_pat_print(const struct ml_pat_t *pat, struct io_file_t file);
void ml_pat_print1(const struct ml_pat_t *pat, struct io_file_t file);
struct io_chunk_t ml_pat_chunk(const struct ml_pat_t *pat);


/**
 * Delete a pattern if not null.
 *   @pat: The pattern.
 */
static inline void ml_pat_erase(struct ml_pat_t *pat)
{
	if(pat != NULL)
		ml_pat_delete(pat);
}

#endif
