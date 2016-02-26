#ifndef MATCH_H
#define MATCH_H

/**
 * Pattern type enumerator.
 *   @ml_pat_id_e: Identifier.
 *   @ml_pat_tuple_e: Tuple.
 *   @ml_pat_list_e: List.
 */

enum ml_pat_e {
	ml_pat_id_e,
	ml_pat_tuple_e,
	ml_pat_list_e,
};

/**
 * Pattern data union.
 *   @id: The identifier.
 *   @tuple: Patterned tuple.
 *   @list: List pair.
 */

union ml_pat_u {
	char *id;
	struct ml_pat_t *tuple;
	struct ml_pat_t *list[2];
};

/**
 * Pattern structure.
 *   @type: The type.
 *   @data: The data.
 *   @next: The next pattern.
 */

struct ml_pat_t {
	enum ml_pat_e type;
	union ml_pat_u data;

	struct ml_pat_t *next;
};


/*
 * pattern declarations
 */

struct ml_pat_t *ml_pat_new(enum ml_pat_e type, union ml_pat_u data);
struct ml_pat_t *ml_pat_copy(struct ml_pat_t *pat);
void ml_pat_delete(struct ml_pat_t *pat);

struct ml_pat_t *ml_pat_id(char *id);
struct ml_pat_t *ml_pat_tuple(struct ml_pat_t *tuple);
struct ml_pat_t *ml_pat_list(struct ml_pat_t *head, struct ml_pat_t *tail);

void ml_pat_print(struct ml_pat_t *pat, FILE *file);

bool ml_pat_match(struct ml_env_t *env, struct ml_pat_t *pat, struct ml_value_t *value);

/*
 * match declarations
 */

bool ml_match_pat(struct ml_env_t *env, struct ml_expr_t *pat, struct ml_value_t *value);

#endif
