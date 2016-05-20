#ifndef TOKEN_H
#define TOKEN_H

/**
 * Symbol structure.
 *   @str: The string.
 *   @id; The identifier.
 */
struct ml_symbol_t {
	const char *str;
	uint16_t id;
};


/**
 * Token enumerator.
 *   @ml_token_id_v: Identifier.
 *   @ml_token_num_v: Integer value.
 *   @ml_token_flt_v: Floating-point value.
 *   @ml_token_str_v: String value.
 *   @ml_token_nil_v: Nil.
 *   @ml_token_true_v: True.
 *   @ml_token_false_v: False.
 *   @ml_token_let_v: Let.
 *   @ml_token_in_v: In.
 *   @ml_token_if_v: If.
 *   @ml_token_then_v: Then.
 *   @ml_token_else_v: Else.
 *   @ml_token_match_v: Match.
 *   @ml_token_with_v: With.
 *   @ml_token_import_v: Import.
 *   @ml_token_eq_v: Equality.
 *   @ml_token_lte_v: Less than or equal.
 *   @ml_token_gte_v: Greater than or equal.
 *   @ml_token_cons_v: List cons.
 *   @ml_token_concat_v: Concatonate.
 *   @ml_token_arrow_v: Arrow.
 *   @ml_token_comment_v: Comment.
 */
enum ml_token_e {
	ml_token_id_v = 0x100,
	ml_token_str_v = 0x101,
	ml_token_num_v = 0x102,
	ml_token_flt_v = 0x103,
	ml_token_nil_v = 0x0200,
	ml_token_true_v = 0x0201,
	ml_token_false_v = 0x0202,
	ml_token_let_v = 0x203,
	ml_token_in_v = 0x204,
	ml_token_if_v = 0x205,
	ml_token_then_v = 0x206,
	ml_token_else_v = 0x207,
	ml_token_match_v = 0x208,
	ml_token_with_v = 0x209,
	ml_token_import_v = 0x20a,
	ml_token_eq_v = 0x300,
	ml_token_lte_v = 0x301,
	ml_token_gte_v = 0x302,
	ml_token_cons_v = 0x400,
	ml_token_concat_v = 0x401,
	ml_token_arrow_v = 0x500,
	ml_token_comment_v = 0x600
};

/**
 * Token data union.
 *   @num: Integer value.
 *   @flt: Floating-point value.
 *   @str: String.
 */
union ml_token_u {
	int num;
	double flt;
	char *str;
	bool flag;
};

/**
 * Token structure.
 *   @tag: The tag.
 *   @id: The token identifier.
 *   @data: The token data.
 *   @next: The next token.
 */
struct ml_token_t {
	struct ml_tag_t tag;

	uint16_t id;
	union ml_token_u data;

	struct ml_token_t *next;
};


/*
 * path declarations
 */
struct ml_path_t *ml_path_new(char *str);
struct ml_path_t *ml_path_copy(struct ml_path_t *path);
void ml_path_delete(struct ml_path_t *path);

/*
 * tag declarations
 */
extern struct ml_tag_t ml_tag_null;

struct ml_tag_t ml_tag_new(struct ml_path_t *path, unsigned int line, unsigned int col);
struct ml_tag_t ml_tag_copy(struct ml_tag_t tag);
void ml_tag_delete(struct ml_tag_t tag);

struct io_chunk_t ml_tag_chunk(const struct ml_tag_t *tag);

/*
 * token declarations
 */
struct ml_token_t *ml_token_new(uint16_t id, union ml_token_u data, struct ml_tag_t tag);
void ml_token_delete(struct ml_token_t *token);

struct ml_token_t *ml_token_simple(enum ml_token_e id, struct ml_tag_t tag);
struct ml_token_t *ml_token_id(char *str, struct ml_tag_t tag);
struct ml_token_t *ml_token_flt(double flt, struct ml_tag_t tag);
struct ml_token_t *ml_token_str(char *str, struct ml_tag_t tag);

char *ml_token_load(struct ml_token_t **res, const char *path);
void ml_token_strip(struct ml_token_t **token);

void ml_token_print(const struct ml_token_t *token, struct io_file_t file);
struct io_chunk_t ml_token_chunk(const struct ml_token_t *token);


/**
 * Replace a tag.
 *   @dest: The destination.
 *   @src: The source.
 */
static inline void ml_tag_replace(struct ml_tag_t *dest, struct ml_tag_t src)
{
	ml_tag_delete(*dest);
	*dest = src;
}

#endif
