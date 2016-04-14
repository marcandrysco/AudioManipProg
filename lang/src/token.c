#include "common.h"


/*
 * local declarations
 */
static void tag_proc(struct io_file_t file, void *arg);

/*
 * global variables
 */
struct ml_tag_t ml_tag_null = { NULL, 0, 0 };


/**
 * Create a chunk for a tag.
 *   @tag: The tag.
 *   &returns: The chunk.
 */
struct io_chunk_t ml_tag_chunk(const struct ml_tag_t *tag)
{
	return (struct io_chunk_t){ tag_proc, (void *)tag };
}
static void tag_proc(struct io_file_t file, void *arg)
{
	const struct ml_tag_t *tag = arg;

	if(memcmp(&ml_tag_null, tag, sizeof(struct ml_tag_t)) == 0)
		hprintf(file, "(unk)");
	else
		hprintf(file, "%s:%u:%u", tag->file ?: "-", tag->line, tag->col);
}

bool issym(char ch)
{
	return ((ch >= 33) && (ch <= 47)) || ((ch >= 58) && (ch <= 64)) || ((ch >= 91) && (ch <= 96)) || ((ch >= 123) && (ch <= 126));
}


/**
 * Reader structure.
 *   @path: The path.
 *   @file: The file.
 *   @tag: The tag.
 */
struct reader_t {
	char *path;
	FILE *file;

	struct ml_tag_t tag;
};


/**
 * Parse the next symbol from the file.
 *   @file: The file.
 *   @byte: The byte.
 *   @tag: The tag.
 *   &returns: The token or end.
 */
enum ml_token_e ml_token_symbol(FILE *file, int *byte, struct ml_tag_t *tag)
{
	enum ml_token_e token;

	switch(*byte) {
	case '(':
		*byte = fgetc(file), tag->off++, tag->col++;
		switch(*byte) {
		case '*':
			token = ml_token_comment_e;

			{
				int nest = 1;

				*byte = fgetc(file), tag->off++, tag->col++;

				while(true) {
					if(*byte == '*') {
						do
							*byte = fgetc(file), tag->off++, tag->col++;
						while(*byte == '*');

						if(*byte == ')') {
							if(--nest == 0)
								break;
						}
					}
					else if(*byte == '(') {
						*byte = fgetc(file), tag->off++, tag->col++;
						if(*byte == '*')
							nest++;
					}
					else if(*byte == EOF)
						break;

					*byte = fgetc(file), tag->off++, tag->col++;
				}

				if(*byte == EOF)
					return ml_token_end_e;
			}

			break;

		case ')': token = ml_token_nil_e; break;
		default: return ml_token_lparen_e;
		}

		break;

	case '+':
		*byte = fgetc(file), tag->off++, tag->col++;
		switch(*byte) {
		case '+': token = ml_token_concat_e; break;
		default: return ml_token_plus_e;
		}

		break;
	case '-':
		*byte = fgetc(file), tag->off++, tag->col++;
		switch(*byte) {
		case '>': token = ml_token_arrow_e; break;
		default: return ml_token_minus_e;
		}

		break;

	case '=':
		*byte = fgetc(file), tag->off++, tag->col++;
		switch(*byte) {
		case '=': token = ml_token_equal_e; break;
		default: return ml_token_assign_e;
		}

		break;

	case '>':
		*byte = fgetc(file), tag->off++, tag->col++;
		switch(*byte) {
		case '=': token = ml_token_gte_e; break;
		default: return ml_token_gt_e;
		}

		break;

	case '<':
		*byte = fgetc(file), tag->off++, tag->col++;
		switch(*byte) {
		case '=': token = ml_token_lte_e; break;
		default: return ml_token_lt_e;
		}

		break;

	case ')': token = ml_token_rparen_e; break;
	case '[': token = ml_token_lbrace_e; break;
	case ']': token = ml_token_rbrace_e; break;
	case ',': token = ml_token_comma_e; break;
	case ';': token = ml_token_semicolon_e; break;

	case ':':
		*byte = fgetc(file), tag->off++, tag->col++;
		switch(*byte) {
		case ':': token = ml_token_cons_e; break;
		default: return ml_token_colon_e;
		}

		break;

	case '*': token = ml_token_star_e; break;
	case '/': token = ml_token_slash_e; break;
	case '%': token = ml_token_mod_e; break;
	case '|': token = ml_token_pipe_e; break;
	default: return ml_token_end_e;
	}

	*byte = fgetc(file), tag->off++, tag->col++;

	return token;
}

/**
 * Find the keyword from a string.
 *   @str: The string.
 *   &returns: The token or negative.
 */
enum ml_token_e ml_token_keyword(const char *str)
{
	if(strcmp(str, "let") == 0)
		return ml_token_let_e;
	else if(strcmp(str, "import") == 0)
		return ml_token_import_e;
	else if(strcmp(str, "in") == 0)
		return ml_token_in_e;
	else if(strcmp(str, "match") == 0)
		return ml_token_match_e;
	else if(strcmp(str, "with") == 0)
		return ml_token_with_e;
	else if(strcmp(str, "if") == 0)
		return ml_token_if_e;
	else if(strcmp(str, "then") == 0)
		return ml_token_then_e;
	else if(strcmp(str, "else") == 0)
		return ml_token_else_e;
	else if(strcmp(str, "fun") == 0)
		return ml_token_func_e;
	else
		return -1;
}


/**
 * Create a simple token.
 *   @id: The identifier string.
 *   &returns: The token.
 */
struct ml_token_t *ml_token_new(enum ml_token_e type)
{
	struct ml_token_t *token;

	token = malloc(sizeof(struct ml_token_t));
	token->next = NULL;
	token->type = type;

	return token;
}

/**
 * Create an identifier token.
 *   @id: The identifier string.
 *   &returns: The token.
 */

struct ml_token_t *ml_token_id(const char *id)
{
	struct ml_token_t *token;

	token = ml_token_new(ml_token_id_e);
	token->data.str = strdup(id);

	return token;
}

/**
 * Create a string.
 *   @str: The identifier string.
 *   &returns: The token.
 */
struct ml_token_t *ml_token_str(const char *str)
{
	struct ml_token_t *token;

	token = ml_token_new(ml_token_str_e);
	token->data.str = strdup(str);

	return token;
}

/**
 * Create a number token.
 *   @id: The identifier string.
 *   &returns: The token.
 */
struct ml_token_t *ml_token_num(double flt)
{
	struct ml_token_t *token;

	token = ml_token_new(ml_token_num_e);
	token->data.flt = flt;

	return token;
}

/**
 * Delete a token.
 *   @token: The token.
 */
void ml_token_delete(struct ml_token_t *token)
{
	if((token->type == ml_token_id_e) || (token->type == ml_token_str_e))
		free(token->data.str);

	free(token);
}

/**
 * Parse a list of tokens from a file.
 *   @file: The file.
 *   @path: The path.
 *   @err: The error.
 *   &returns: The token list.
 */
struct ml_token_t *ml_token_parse(FILE *file, const char *path, char **err)
{
#undef fail
#define fail(str, ...) do { if(*err == NULL) ml_eprintf(err, "%s:%u:%u:" str, tag.file, tag.line, tag.col); return NULL; } while(0)

	int byte;
	enum ml_token_e type;
	struct ml_tag_t tag = { path, 0, 1, 0 };
	struct ml_token_t *head, **token;

	head = NULL;
	token = &head;

	byte = fgetc(file), tag.off++, tag.col++;
	while(byte != EOF) {
		if(isalpha(byte) || (byte == '_')) {
			char id[256];
			unsigned int i = 0;

			do {
				if(i == sizeof(id))
					fail("Identifier too long.");

				id[i++] = byte;
				byte = fgetc(file), tag.off++, tag.col++;
			} while(isalnum(byte) || (byte == '_') || (byte == '\'') || (byte == '.'));

			id[i] = '\0';
			type  = ml_token_keyword(id);
			if((int)type < 0)
				*token = ml_token_id(id);
			else
				*token = ml_token_new(type);

			(*token)->tag = tag;
			token = &(*token)->next;
		}
		else if(isdigit(byte)) {
			char num[256], *endptr;
			double flt;
			unsigned int i = 0;

			do {
				if(i == sizeof(num))
					fail("Number too long.");

				num[i++] = byte;
				byte = fgetc(file), tag.off++, tag.col++;
			} while(isalnum(byte) || (byte == '.') || (byte == 'e') || (byte == '-' && num[i-1] == 'e'));

			errno = 0;
			num[i] = '\0';
			flt = strtod(num, &endptr);
			if((errno != 0) || (*endptr != '\0'))
				fail("Invalid number.");

			*token = ml_token_num(flt);
			(*token)->tag = tag;
			token = &(*token)->next;
		}
		else if(byte == '"') {
			char str[256];
			unsigned int i = 0;

			byte = fgetc(file), tag.off++, tag.col++;

			while((byte != '"') && (byte != EOF)) {
				if(byte == '\\') {
					byte = fgetc(file), tag.off++, tag.col++;
					switch(byte) {
					case '\\': break;
					case 't': byte = '\t'; break;
					case 'n': byte = '\n'; break;
					default: fail("Invalid escaped sequence '\\%c'.", byte);
					}
				}

				if(i == sizeof(str))
					fail("Identifier too long.");

				str[i++] = byte;
				byte = fgetc(file), tag.off++, tag.col++;
			}

			if(byte == EOF)
				fail("Unterminated quote.");

			str[i] = '\0';
			byte = fgetc(file), tag.off++, tag.col++;

			*token = ml_token_str(str);
			(*token)->tag = tag;
			token = &(*token)->next;
		}
		else if((type = ml_token_symbol(file, &byte, &tag)) != ml_token_end_e) {
			if(type != ml_token_comment_e) {
				*token = ml_token_new(type);
				(*token)->tag = tag;
				token = &(*token)->next;
			}
		}
		else if(isspace(byte)) {
			if(byte == '\n')
				tag.line++, tag.col = 0;

			byte = fgetc(file), tag.off++, tag.col++;
		}
		else
			fail("Unknown token '%c'.", byte);
	}

	*token = ml_token_new(ml_token_end_e);
	(*token)->tag = tag;

	return head;
}

/**
 * Load a token list from a path.
 *   @path: The path.
 *   @err: The error message pointer.
 *   &returns: The token list.
 */
struct ml_token_t *ml_token_load(const char *path, char **err)
{
	FILE *file;
	struct ml_token_t *token;

	file = fopen(path, "r");
	if(file == NULL)
		return ml_eprintf(err, "Unknown file '%s'.", path);

	token = ml_token_parse(file, path, err);
	fclose(file);

	return token;
}

/**
 * Clean up a list of tokens.
 *   @token: The token.
 */
void ml_token_clean(struct ml_token_t *token)
{
	struct ml_token_t *next;

	while(token != NULL) {
		next = token->next;
		ml_token_delete(token);
		token = next;
	}
}


/**
 * Lexer sructure.
 */
struct lex_t {
};

/**
 * Create a new path.
 *   @str: Consumed. The string value.
 *   &returns: The path.
 */
struct ml_path_t *ml_path_new(char *str)
{
	struct ml_path_t *path;

	path = malloc(sizeof(struct ml_path_t));
	path->str = str;
	path->refcnt = 1;

	return path;
}

/**
 * Copy a path.
 *   @path: The original path.
 *   &returns: The copied path.
 */
struct ml_path_t *ml_path_copy(struct ml_path_t *path)
{
	path->refcnt++;

	return path;
}

/**
 * Delete a path.
 *   @path: The path.
 */
void ml_path_delete(struct ml_path_t *path)
{
	if(--path->refcnt > 0)
		return;

	erase(path);
	free(path);
}


/**
 * Create a tag.
 *   @path: Consumed. The path.
 *   @line: The line.
 *   @col: The column.
 *   &returns: The tag.
 */
struct ml_tag0_t ml_tag0_new(struct ml_path_t *path, unsigned int line, unsigned int col)
{
	return (struct ml_tag0_t){ path, line, col };
}

/**
 * Copy a tag.
 *   @tag: The original tag.
 *   &returns: The copied tag.
 */
struct ml_tag0_t ml_tag0_copy(struct ml_tag0_t tag)
{
	return ml_tag0_new(ml_path_copy(tag.path), tag.line, tag.col);
}

/**
 * Delete a tag.
 *   @tag: The tag.
 */
void ml_tag0_delete(struct ml_tag0_t tag)
{
	ml_path_delete(tag.path);
}
