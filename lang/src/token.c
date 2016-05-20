#include "common.h"


/**
 * Reader structure.
 *   @ch: Current character.
 *   @file: The file.
 *   @tag: The tag.
 */
struct read_t {
	int ch;
	FILE *file;
	struct ml_tag_t tag;
};


/*
 * local declarations
 */
static void tag_proc(struct io_file_t file, void *arg);
static void token_proc(struct io_file_t file, void *arg);

static struct read_t *read_open(const char *path);
static void read_close(struct read_t *read);
static int read_next(struct read_t *read);

/*
 * global variables
 */
struct ml_tag_t ml_tag_null = { &(struct ml_path_t){ "-", 1 }, 0, 0 };


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

	erase(path->str);
	free(path);
}


/**
 * Create a tag.
 *   @path: Consumed. The path.
 *   @line: The line.
 *   @col: The column.
 *   &returns: The tag.
 */
struct ml_tag_t ml_tag_new(struct ml_path_t *path, unsigned int line, unsigned int col)
{
	return (struct ml_tag_t){ path, line, col };
}

/**
 * Copy a tag.
 *   @tag: The original tag.
 *   &returns: The copied tag.
 */
struct ml_tag_t ml_tag_copy(struct ml_tag_t tag)
{
	return ml_tag_new(ml_path_copy(tag.path), tag.line, tag.col);
}

/**
 * Delete a tag.
 *   @tag: The tag.
 */
void ml_tag_delete(struct ml_tag_t tag)
{
	ml_path_delete(tag.path);
}


/**
 * Retrieve a chunk for a tag.
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

	hprintf(file, "%s:%u:%u", tag->path ? tag->path->str : "-", tag->line, tag->col);
}


/**
 * Create a new token.
 *   @id: The indetifier.
 *   @data: Consumed. The data.
 *   @tag: Consumed. The tag.
 *   &returns: The token.
 */
struct ml_token_t *ml_token_new(uint16_t id, union ml_token_u data, struct ml_tag_t tag)
{
	struct ml_token_t *token;

	token = malloc(sizeof(struct ml_token_t));
	token->id = id;
	token->data = data;
	token->tag = tag;
	token->next = NULL;

	return token;
}

/**
 * Delete a token list.
 *   @token: The token list.
 */
void ml_token_delete(struct ml_token_t *token)
{
	struct ml_token_t *cur;

	while(token != NULL) {
		cur = token;
		token = token->next;

		switch(cur->id) {
		case ml_token_id_v: free(cur->data.str); break;
		case ml_token_str_v: free(cur->data.str); break;
		}

		ml_tag_delete(cur->tag);
		free(cur);
	}
}


/**
 * Create a simple token.
 *   @id: The identifier.
 *   @tag: The tag.
 *   &returns: The token.
 */
struct ml_token_t *ml_token_simple(enum ml_token_e id, struct ml_tag_t tag)
{
	return ml_token_new(id, (union ml_token_u){ }, tag);
}

/**
 * Create an identifier token.
 *   @str: The string.
 *   @tag: The tag.
 *   &returns: The token.
 */
struct ml_token_t *ml_token_id(char *str, struct ml_tag_t tag)
{
	return ml_token_new(ml_token_id_v, (union ml_token_u){ .str = str }, tag);
}

/**
 * Create a number token.
 *   @num: The value.
 *   @tag: The tag.
 *   &returns: The token.
 */
struct ml_token_t *ml_token_num(double num, struct ml_tag_t tag)
{
	return ml_token_new(ml_token_num_v, (union ml_token_u){ .num = num }, tag);
}

/**
 * Create a floating-point token.
 *   @flt: The floating-point value.
 *   @tag: The tag.
 *   &returns: The token.
 */
struct ml_token_t *ml_token_flt(double flt, struct ml_tag_t tag)
{
	return ml_token_new(ml_token_flt_v, (union ml_token_u){ .flt = flt }, tag);
}

/**
 * Create an string token.
 *   @str: The string.
 *   @tag: The tag.
 *   &returns: The token.
 */
struct ml_token_t *ml_token_str(char *str, struct ml_tag_t tag)
{
	return ml_token_new(ml_token_str_v, (union ml_token_u){ .str = str }, tag);
}


/**
 * Load a token list from a path.
 *   @res: Ref: The result.
 *   @path: The path.
 *   &returns: Error.
 */
char *ml_token_load(struct ml_token_t **res, const char *path)
{
#define onexit ml_token_delete(*res); *res = NULL; read_close(read);
	struct read_t *read;
	struct ml_token_t **token = res;

	*res = NULL;

	read = read_open(path);
	if(read == NULL)
		return mprintf("Failed to open '%s' -- %s (%d).", path, strerror(errno), errno);

	while(true) {
		while(isspace(read->ch))
			read_next(read);

		if(read->ch == EOF)
			break;

		if(isalpha(read->ch) || (read->ch == '_')) {
			char *str;
			struct strbuf_t buf;
			struct ml_tag_t tag = ml_tag_copy(read->tag);

			buf = strbuf_init(32);
			do
				strbuf_addch(&buf, read->ch), read_next(read);
			while(isalnum(read->ch) || (read->ch == '_') || (read->ch == '.') || (read->ch == '\''));

			str = strbuf_done(&buf);

			if(strcmp(str, "true") == 0)
				*token = ml_token_simple(ml_token_true_v, tag);
			else if(strcmp(str, "false") == 0)
				*token = ml_token_simple(ml_token_false_v, tag);
			else if(strcmp(str, "let") == 0)
				*token = ml_token_simple(ml_token_let_v, tag);
			else if(strcmp(str, "in") == 0)
				*token = ml_token_simple(ml_token_in_v, tag);
			else if(strcmp(str, "if") == 0)
				*token = ml_token_simple(ml_token_if_v, tag);
			else if(strcmp(str, "then") == 0)
				*token = ml_token_simple(ml_token_then_v, tag);
			else if(strcmp(str, "else") == 0)
				*token = ml_token_simple(ml_token_else_v, tag);
			else if(strcmp(str, "match") == 0)
				*token = ml_token_simple(ml_token_match_v, tag);
			else if(strcmp(str, "with") == 0)
				*token = ml_token_simple(ml_token_with_v, tag);
			else if(strcmp(str, "import") == 0)
				*token = ml_token_simple(ml_token_import_v, tag);
			else
				*token = ml_token_id(str, tag);

			if((*token)->id != ml_token_id_v)
				free(str);

			token = &(*token)->next;
		}
		else if(read->ch == '"') {
			struct strbuf_t buf;
			struct ml_tag_t tag = read->tag;

			buf = strbuf_init(32);

			while(true) {
				read_next(read);

				if((read->ch == '"') || (read->ch == EOF) || (read->ch == '\n'))
					break;
				else if(read->ch == '\\') {
					read_next(read);
					switch(read->ch) {
					case 't': strbuf_addch(&buf, '\t'); break;
					case 'n': strbuf_addch(&buf, '\n'); break;
					default: fail("%C: Unknown escape sequencer '\\%c'.", ml_tag_chunk(&read->tag), read->ch);
					}
				}
				else
					strbuf_addch(&buf, read->ch);
			}

			*token = ml_token_str(strbuf_done(&buf), ml_tag_copy(tag));
			token = &(*token)->next;

			if((read->ch == EOF) || (read->ch == '\n'))
				fail("%C: Unterminated quote.", ml_tag_chunk(&read->tag));

			read_next(read);
		}
		else if(isdigit(read->ch) || (read->ch == '.')) {
			char *str;
			struct strbuf_t buf;
			struct ml_tag_t tag = ml_tag_copy(read->tag);

			buf = strbuf_init(32);

			while(isdigit(read->ch))
				strbuf_addch(&buf, read->ch), read_next(read);

			if(read->ch == '.') {
				strbuf_addch(&buf, read->ch), read_next(read);

				while(isdigit(read->ch))
					strbuf_addch(&buf, read->ch), read_next(read);

				if(read->ch == 'e') {
					strbuf_addch(&buf, read->ch), read_next(read);

					while(isdigit(read->ch))
						strbuf_addch(&buf, read->ch), read_next(read);
				}

				str = strbuf_done(&buf);
				*token = ml_token_flt(strtod(str, NULL), tag);
				free(str);
			}
			else {
				str = strbuf_done(&buf);
				*token = ml_token_num(strtol(str, NULL, 0), tag);
				free(str);
			}

			token = &(*token)->next;
		}
		else if(strchr("<=>+-*/%^&|()[].,?:", read->ch) != NULL) {
			struct map_t {
				const char *str;
				uint16_t id;
			};

			static const struct map_t map[] = {
				{ "::",  ml_token_cons_v },
				{ "++",  ml_token_concat_v },
				{ "==",  ml_token_eq_v },
				{ "<=",  ml_token_lte_v },
				{ ">=",  ml_token_gte_v },
				{ "->",  ml_token_arrow_v },
				{ "()",  ml_token_nil_v },
				{ "(*",  ml_token_comment_v },
				{ "(+)",  ml_token_id_v },
				{ "(-)",  ml_token_id_v },
				{ "(*)",  ml_token_id_v },
				{ "(/)",  ml_token_id_v },
				{ "(%)",  ml_token_id_v },
				{ NULL,  0 }
			};

			uint16_t id;
			struct ml_tag_t tag = ml_tag_copy(read->tag);
			char tmp[4];
			unsigned int i, k = 2;

			tmp[0] = id = read->ch;
			tmp[1] = read_next(read);
			tmp[2] = '\0';

			while(true) {
				for(i = 0; map[i].str != NULL; i++) {
					if(memcmp(tmp, map[i].str, k) != 0)
						continue;

					id = (map[i].str[k] == '\0') ? map[i].id : 0;
					break;
				}

				if(map[i].str == NULL)
					break;

				tmp[k++] = read_next(read);
				tmp[k] = '\0';
			}

			if(id == ml_token_comment_v) {
				unsigned int nest = 1;

				read_next(read);

				while(true) {
					if(read->ch == '*') {
						do
							read_next(read);
						while(read->ch == '*');

						if(read->ch == ')') {
							if(--nest == 0)
								break;
						}
					}
					else if(read->ch == '(') {
						read_next(read);
						if(read->ch == '*')
							nest++;
					}
					else if(read->ch == EOF)
						break;

					read_next(read);
				}

				read_next(read);
			}

			if(id == ml_token_id_v) {
				tmp[k-1] = '\0';
				*token = ml_token_id(strdup(tmp), tag);
				token = &(*token)->next;
			}
			else if(id == 0) {
				for(i = 0; i < k-1; i++) {
					*token = ml_token_simple(tmp[i], ml_tag_copy(tag));
					token = &(*token)->next;
					tag.col++;
				}

				ml_tag_delete(tag);
			}
			else {
				*token = ml_token_simple(id, tag);
				token = &(*token)->next;
			}
		}
		else
			fail("Unexpected token '%c'.", read->ch);
	}

	*token = ml_token_simple(0, ml_tag_copy(read->tag));
	read_close(read);

	return NULL;
#undef onexit
}

/**
 * Strip a token list of all comments.
 *   @token: The token list reference.
 */
void ml_token_strip(struct ml_token_t **token)
{
	while(*token != NULL) {
		if((*token)->id == ml_token_comment_v) {
			struct ml_token_t *tmp = *token;

			*token = tmp->next;
			tmp->next = NULL;
			ml_token_delete(tmp);
		}
		else
			token = &(*token)->next;
	}
}


/**
 * Print a token.
 *   @token: The token.
 *   @file: The output ifle.
 */
void ml_token_print(const struct ml_token_t *token, struct io_file_t file)
{
	const char *str;

	switch(token->id) {
	case ml_token_id_v:
		hprintf(file, "%s", token->data.str);
		return;

	case ml_token_str_v:
		hprintf(file, "\"%s\"", token->data.str);
		return;

	case ml_token_num_v:
		hprintf(file, "%d", token->data.num);
		return;

	case ml_token_flt_v:
		hprintf(file, "%g", token->data.flt);
		return;

	case ml_token_nil_v: str = "()"; break;
	case ml_token_true_v: str = "true"; break;
	case ml_token_false_v: str = "false"; break;
	case ml_token_let_v: str = "let"; break;
	case ml_token_import_v: str = "import"; break;
	case ml_token_cons_v: str = "::"; break;
	case ml_token_eq_v: str = "=="; break;
	case ml_token_lte_v: str = "<="; break;
	case ml_token_gte_v: str = ">="; break;
	default: str = NULL; break;
	}

	if(str != NULL)
		hprintf(file, "%s", str);
	else
		hprintf(file, "%c", token->id);
}

/**
 * Retrieve a chunk for a token.
 *   @token: The token.
 *   &returns: The chunk.
 */
struct io_chunk_t ml_token_chunk(const struct ml_token_t *token)
{
	return (struct io_chunk_t){ token_proc, (void *)token };
}
static void token_proc(struct io_file_t file, void *arg)
{
	ml_token_print(arg, file);
}


/**
 * Open a reader.
 *   @path: The path.
 *   &returns: The reader or null.
 */
static struct read_t *read_open(const char *path)
{
	FILE *file;
	struct read_t *read;

	file = fopen(path, "r");
	if(file == NULL)
		return NULL;

	read = malloc(sizeof(struct read_t));
	read->file = file;
	read->tag = ml_tag_new(ml_path_new(strdup(path)), 1, 0);
	read_next(read);

	return read;
}

/**
 * Close a reader.
 *   @read: The reader.
 */
static void read_close(struct read_t *read)
{
	ml_tag_delete(read->tag);
	fclose(read->file);
	free(read);
}

/**
 * Read the next character from the reader.
 *   @read: The reader.
 *   &returns: The next character.
 */
static int read_next(struct read_t *read)
{
	read->ch = fgetc(read->file);

	if(read->ch == '\n') {
		read->tag.line++;
		read->tag.col = 0;
	}
	else
		read->tag.col++;

	return read->ch;
}
