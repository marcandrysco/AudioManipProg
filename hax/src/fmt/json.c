#include "../common.h"


/*
 * reder definitions
 */
enum read_e { null_v, true_v, false_v, str_v, num_v, eof_v, inv_v };

/**
 * Reader structure.
 *   @ch: The current character.
 *   @file: The file.
 *   @id: The current identifier.
 *   @str: The string.
 *   @num: The number.
 *   @path: The path.
 *   @line, col: The line and column information.
 */
struct read_t {
	int16_t ch;
	struct io_file_t file;

	uint16_t id;
	char *str;
	double num;

	char *path;
	unsigned int line, col;
};


/*
 * local function declarations
 */
static void json_proc(struct io_file_t file, void *arg);
static void arr_proc(struct io_file_t file, void *arg);
static void obj_proc(struct io_file_t file, void *arg);

static void prop_delete(void *ptr);

static struct read_t read_init(struct io_file_t file);
static void read_destroy(struct read_t *read);
static int16_t read_ch(struct read_t *read);
static char *read_next(struct read_t *read);


/**
 * Create a JSON value.
 *   @type: The type.
 *   @data: The data.
 *   &returns: The value.
 */
struct json_t *json_new(enum json_e type, union json_u data)
{
	struct json_t *json;

	json = malloc(sizeof(struct json_t));
	*json = (struct json_t){ type, data };

	return json;
}

/**
 * Delete a json value.
 *   @json: The value.
 */
void json_delete(struct json_t *json)
{
	switch(json->type) {
	case json_null_v: break;
	case json_true_v: break;
	case json_false_v: break;
	case json_num_v: break;
	case json_str_v: free(json->data.str); break;
	case json_arr_v: json_arr_delete(json->data.arr); break;
	case json_obj_v: json_obj_delete(json->data.obj); break;
	}

	free(json);
}


/**
 * Create a JSON null value.
 *   &returns: The JSON structure.
 */
struct json_t *json_null(void)
{
	return json_new(json_null_v, (union json_u){ });
}

/**
 * Create a JSON boolean.
 *   @num: The number.
 *   &returns: The JSON structure.
 */
struct json_t *json_bool(bool flag)
{
	return json_new(flag ? json_true_v : json_false_v, (union json_u){ });
}

/**
 * Create a JSON number.
 *   @num: The number.
 *   &returns: The JSON structure.
 */
struct json_t *json_num(double num)
{
	return json_new(json_num_v, (union json_u){ .num = num });
}

/**
 * Create a JSON string.
 *   @str: The string.
 *   &returns: The JSON structure.
 */
struct json_t *json_str(char *str)
{
	return json_new(json_str_v, (union json_u){ .str = str });
}

/**
 * Create a JSON array.
 *   @arr: Consumed. The array.
 *   &returns: The JSON structure.
 */
struct json_t *json_arr(struct json_arr_t *arr)
{
	return json_new(json_arr_v, (union json_u){ .arr = arr });
}

/**
 * Create a JSON object.
 *   @obj: Consumed. The object.
 *   &returns: The JSON structure.
 */
struct json_t *json_obj(struct json_obj_t *obj)
{
	return json_new(json_obj_v, (union json_u){ .obj = obj });
}


/**
 * Retrieve a chunk for the JSON value.
 *   @json: The JSON.
 *   &returns: The chunk.
 */
struct io_chunk_t json_chunk(const struct json_t *json)
{
	return (struct io_chunk_t){ json_proc, (void *)json };
}
static void json_proc(struct io_file_t file, void *arg)
{
	json_print(arg, file);
}

/**
 * Print a JSON value.
 *   @json: The JSON value.
 *   @file: The output file.
 */
void json_print(struct json_t *json, struct io_file_t file)
{
	switch(json->type) {
	case json_null_v: hprintf(file, "null"); break; 
	case json_true_v: hprintf(file, "true"); break; 
	case json_false_v: hprintf(file, "false"); break; 
	case json_num_v: hprintf(file, "%.17g", json->data.num); break; 
	case json_str_v: hprintf(file, "\"%s\"", json->data.str); break; 
	case json_arr_v: json_arr_print(json->data.arr, file); break;
	case json_obj_v: json_obj_print(json->data.obj, file); break;
	}
}


/**
 * Create a JSON array.
 *   &returns: The array.
 */
struct json_arr_t *json_arr_new(void)
{
	struct json_arr_t *arr;

	arr = malloc(sizeof(struct json_arr_t));
	arr->vec = malloc(100);
	arr->len = 0;

	return arr;
}

/**
 * Delete an array.
 *   @arr: The array.
 */
void json_arr_delete(struct json_arr_t *arr)
{
	unsigned int i;

	for(i = 0; i < arr->len; i++)
		json_delete(arr->vec[i]);

	free(arr->vec);
	free(arr);
}

/**
 * Append an element to the array.
 *   @arr: The array.
 *   @el: Consumed. The element.
 */
void json_arr_append(struct json_arr_t *arr, struct json_t *el)
{
	arr->vec = realloc(arr->vec, (arr->len + 1) * sizeof(struct json_t *));
	arr->vec[arr->len++] = el;
}


/**
 * Retrieve a chunk for the JSON array.
 *   @arr: The array.
 *   &returns: The chunk.
 */
struct io_chunk_t json_arr_chunk(const struct json_t *json)
{
	return (struct io_chunk_t){ arr_proc, (void *)json };
}
static void arr_proc(struct io_file_t file, void *arg)
{
	json_print(arg, file);
}

/**
 * Print a JSON array.
 *   @arr: The array.
 *   @file: The output file.
 */
void json_arr_print(struct json_arr_t *arr, struct io_file_t file)
{
	unsigned int i;

	hprintf(file, "[");

	for(i = 0; i < arr->len; i++)
		hprintf(file, "%s%C", (i > 0) ? "," : "", json_chunk(arr->vec[i]));

	hprintf(file, "]");
}


/**
 * Create a JSON object.
 *   &returns: The object.
 */
struct json_obj_t *json_obj_new(void)
{
	struct json_obj_t *obj;

	obj = malloc(sizeof(struct json_obj_t));
	obj->root = avltree_root_init(compare_str);

	return obj;
}

/**
 * Delete an object.
 *   @obj: The object.
 */
void json_obj_delete(struct json_obj_t *obj)
{
	avltree_root_destroy(&obj->root, offsetof(struct json_prop_t, node), prop_delete);
	free(obj);
}
static void prop_delete(void *ptr)
{
	struct json_prop_t *prop = ptr;

	json_delete(prop->value);
	free(prop->key);
	free(prop);
}


/**
 * Add a key-value pair to the object.
 *   @obj: The object.
 *   @key: Consumed. The key.
 *   @value: Consumed. The value.
 *   &returns: The property.
 */
struct json_prop_t *json_obj_add(struct json_obj_t *obj, char *key, struct json_t *value)
{
	struct json_prop_t *prop;

	prop = malloc(sizeof(struct json_prop_t));
	prop->key = key;
	prop->value = value;
	prop->node.ref = key;
	avltree_root_insert(&obj->root, &prop->node);

	return prop;
}

/**
 * Retrieve a key-value pair from the object by key.
 *   @obj: The object.
 *   @key: The key.
 *   &returns: The property or null.
 */
struct json_prop_t *json_obj_get(struct json_obj_t *obj, const char *key)
{
	struct avltree_node_t *node;

	node = avltree_root_lookup(&obj->root, key);
	return node ? getparent(node, struct json_prop_t, node) : NULL;
}

/**
 * Retrieve a value from the object by key.
 *   @obj: The object.
 *   @key: The key.
 *   &returns: The value or null.
 */
struct json_t *json_obj_getval(struct json_obj_t *obj, const char *key)
{
	struct json_prop_t *prop;

	prop = json_obj_get(obj, key);
	return prop ? prop->value : NULL;
}


/**
 * Retrieve the first property from the object.
 *   @obj: The object.
 *   &returns: The property or null.
 */
struct json_prop_t *json_obj_first(struct json_obj_t *obj)
{
	struct avltree_node_t *node;

	node = avltree_root_first(&obj->root);
	return node ? getparent(node, struct json_prop_t, node) : NULL;
}

/**
 * Retrieve the last property from the object.
 *   @obj: The object.
 *   &returns: The property or null.
 */
struct json_prop_t *json_obj_last(struct json_obj_t *obj)
{
	struct avltree_node_t *node;

	node = avltree_root_last(&obj->root);
	return node ? getparent(node, struct json_prop_t, node) : NULL;
}

/**
 * Retrieve the previous property from the object.
 *   @prop: The current property..
 *   &returns: The previous property or null.
 */
struct json_prop_t *json_obj_prev(struct json_prop_t *prop)
{
	struct avltree_node_t *node;

	node = avltree_node_prev(&prop->node);
	return node ? getparent(node, struct json_prop_t, node) : NULL;
}

/**
 * Retrieve the next property from the object.
 *   @prop: The current property..
 *   &returns: The next property or null.
 */
struct json_prop_t *json_obj_next(struct json_prop_t *prop)
{
	struct avltree_node_t *node;

	node = avltree_node_next(&prop->node);
	return node ? getparent(node, struct json_prop_t, node) : NULL;
}


/**
 * Retrieve a chunk for the JSON object.
 *   @obj: The object.
 *   &returns: The chunk.
 */
struct io_chunk_t json_obj_chunk(const struct json_obj_t *json)
{
	return (struct io_chunk_t){ obj_proc, (void *)json };
}
static void obj_proc(struct io_file_t file, void *arg)
{
	json_obj_print(arg, file);
}

/**
 * Print a JSON object.
 *   @obj: The object.
 *   @file: The output file.
 */
void json_obj_print(struct json_obj_t *obj, struct io_file_t file)
{
	bool comma = false;
	struct json_prop_t *prop;

	hprintf(file, "{");

	for(prop = json_obj_first(obj); prop != NULL; prop = json_obj_next(prop)) {
		if(comma)
			hprintf(file, ",");

		comma = true;
		hprintf(file, "%s:%C", prop->key, json_chunk(prop->value));
	}

	hprintf(file, "}");
}


/**
 * Read a byte from a file.
 *   @file: The file.
 *   &returns: The byte, or negative if end-of-file.
 */
int16_t io_read_byte(struct io_file_t file)
{
	uint8_t ch;

	if(io_file_read(file, &ch, 1) < 1)
		return -1;

	return ch;
}


/**
 * Initialize the reader.
 *   @file: The file.
 *   &returns: The reader.
 */
static struct read_t read_init(struct io_file_t file)
{
	struct read_t read;

	read.file = file;
	read.str = NULL;
	read_ch(&read);

	return read;
}

/**
 * Destroy a reader.
 *   @read: The reader.
 */
static void read_destroy(struct read_t *read)
{
	erase(read->str);
}

/**
 * Read a character.
 *   @read: The reader.
 *   &returns: The current character.
 */
static int16_t read_ch(struct read_t *read)
{
	read->ch = io_read_byte(read->file);

	return read->ch;
}

/**
 * Read the next token.
 *   @read: The eader.
 *   &returns: Error.
 */
static char *read_next(struct read_t *read)
{
	strset(&read->str, NULL);

	while(isspace(read->ch))
		read_ch(read);

	if(read->ch < 0)
		read->id = eof_v;
	else if(strchr("[]{},:", read->ch)) {
		read->id = read->ch;
		read_ch(read);
	}
	else if(isalpha(read->ch) || (read->ch == '_')) {
		struct strbuf_t buf;

		buf = strbuf_init(32);

		do
			strbuf_addch(&buf, read->ch), read_ch(read);
		while(isalnum(read->ch) || (read->ch == '_'));

		read->str = strbuf_done(&buf);
		if(strcmp(read->str, "null") == 0)
			read->id = null_v;
		else if(strcmp(read->str, "true") == 0)
			read->id = true_v;
		else if(strcmp(read->str, "false") == 0)
			read->id = false_v;
		else
			read->id = str_v;
	}
	else if(read->ch == '"') {
#define onexit strbuf_destroy(&buf);
		struct strbuf_t buf;

		buf = strbuf_init(32);
		read_ch(read);

		while(true) {
			if(read->ch == '\0')
				fail("Unterminated quote.");
			else if(read->ch == '"')
				break;
			else if(read->ch == '\\') {
				char ch;

				read_ch(read);

				switch(read->ch) {
				case 'b': ch = '\b'; break;
				case 'f': ch = '\f'; break;
				case 'n': ch = '\n'; break;
				case 'r': ch = '\r'; break;
				case 't': ch = '\t'; break;
				case '\\': ch = '\\'; break;

				default:
					strbuf_addch(&buf, '\\');
					ch = read->ch;
					break;
				}

				strbuf_addch(&buf, ch);
			}
			else
				strbuf_addch(&buf, read->ch);

			read_ch(read);
		}

		read_ch(read);
		read->str = strbuf_done(&buf);
		read->id = str_v;
#undef onexit
	}
	else if(isdigit(read->ch) || (read->ch == '-')) {
		bool valid;
		char *endptr;
		struct strbuf_t buf;

		buf = strbuf_init(32);

		do
			strbuf_addch(&buf, read->ch), read_ch(read);
		while(isalnum(read->ch) || (read->ch == '_') || (read->ch == '.'));

		strbuf_addch(&buf, '\0');
		read->num = strtod(buf.arr, &endptr);
		read->id = num_v;
		valid = *endptr == '\0';
		strbuf_destroy(&buf);

		if(!valid)
			return mprintf("Invalid number.");
	}
	else
		read->id = inv_v;

	return NULL;
}


/**
 * Parse a value.
 *   @value: The value.
 *   @read: The reader.
 */
static char *parse_value(struct json_t **value, struct read_t *read)
{
	if(read->id == null_v) {
		*value = json_null();
		read_next(read);
	}
	else if(read->id == true_v) {
		*value = json_bool(true);
		read_next(read);
	}
	else if(read->id == false_v) {
		*value = json_bool(false);
		read_next(read);
	}
	else if(read->id == str_v) {
		*value = json_str(strdup(read->str));
		read_next(read);
	}
	else if(read->id == num_v) {
		*value = json_num(read->num);
		read_next(read);
	}
	else if(read->id == '[') {
#define onexit json_arr_delete(arr);
		struct json_t *el;
		struct json_arr_t *arr;

		read_next(read);
		arr = json_arr_new();

		while(read->id != ']') {
			chkfail(parse_value(&el, read));
			json_arr_append(arr, el);

			if(read->id == ',')
				read_next(read);
			else if(read->id != ']')
				fail("Missing ',' or ']' in array.");
		}

		read_next(read);
		*value = json_arr(arr);
#undef onexit
	}
	else if(read->id == '{') {
#define onexit json_obj_delete(obj); erase(key);
		struct json_t *el;
		struct json_obj_t *obj;

		read_next(read);
		obj = json_obj_new();

		while(read->id != '}') {
			char *key = NULL;

			if(read->id != str_v)
				fail("Expected key string.");

			key = strdup(read->str);
			read_next(read);
			if(read->id != ':')
				fail("Expected ':'. %c %d", read->id, read->id);

			read_next(read);
			chkfail(parse_value(&el, read));
			json_obj_add(obj, key, el);

			if(read->id == ',')
				read_next(read);
			else if(read->id != '}')
				fail("Missing ',' or '}' in object.");
		}

		read_next(read);
		*value = json_obj(obj);
#undef onexit
	}
	else
		return mprintf("Expected value.");

	return NULL;
}


/**
 * Parse a JSON value from a file.
 *   @json: Ref. The output JSON value.
 *   @file: The file.
 *   &returns: Error.
 */
char *json_parse_file(struct json_t **json, struct io_file_t file)
{
#define onexit read_destroy(&read);
	struct read_t read;

	read = read_init(file);
	chkfail(read_next(&read));
	chkfail(parse_value(json, &read));
	read_destroy(&read);

	return NULL;
#undef onexit
}

/**
 * Parse a JSON value from a string.
 *   @json: Ref. The output JSON value.
 *   @str: The string.
 *   &returns: Error.
 */
char *json_parse_str(struct json_t **json, const char *str)
{
#define onexit read_destroy(&read); io_file_close(file);
	struct read_t read;
	struct io_file_t file;

	*json = NULL;
	file = io_file_str(str);

	read = read_init(file);
	chkfail(read_next(&read));
	chkfail(parse_value(json, &read));
	read_destroy(&read);

	io_file_close(file);

	return NULL;
#undef onexit
}

/**
 * Parse a JSON value from a file path.
 *   @json: Ref. The output JSON value.
 *   @str: The string.
 *   &returns: Error.
 */
char *json_parse_path(struct json_t **json, const char *path)
{
#define onexit io_file_close(file); fclose(fp);
	FILE *fp;
	struct read_t read;
	struct io_file_t file;

	fp = fopen(path, "r");
	if(fp == NULL)
		return mprintf("File '%s' does not exist.", path);

	file = io_file_wrap(fp);

	read = read_init(file);
	chkfail(read_next(&read));
	chkfail(parse_value(json, &read));
	read_destroy(&read);

	io_file_close(file);
	fclose(fp);

	return NULL;
#undef onexit
}


/**
 * Check if a value is a double that falls in a range.
 *   @json: The JSON value.
 *   @low: The low end of the range.
 *   @high: The high end of the range.
 *   &returns: The number if valid, NAN otherwise.
 */
double json_num_range(struct json_t *json, double low, double high)
{
	double val;

	if(json->type != json_num_v)
		return NAN;

	val = json->data.num;
	if((val < low) || (val > high))
		return NAN;

	return val;
}


/**
 * Check if a value is a string.
 *   @json: The JSON value.
 *   @out: Ref. Optional. The output string.
 *   &returns: True if success.
 */
bool json_str_get(struct json_t *json, const char **out)
{
	if(json->type != json_str_v)
		return false;

	if(out != NULL)
		*out = json->data.str;

	return true;
}

/**
 * Check if a value is a string.
 *   @json: The JSON value.
 *   @out: Ref. Optional. The output string.
 *   &returns: True if success.
 */
bool json_str_objget(struct json_obj_t *obj, const char *id, const char **out)
{
	struct json_t *json;

	json = json_obj_getval(obj, id);
	return json ? json_str_get(json, out) : false;
}


/**
 * Check if a value is an integer that falls in a range.
 *   @json: The JSON value.
 *   @low: The low end of the range.
 *   @high: The high end of the range.
 *   @out: Ref. Optional. The output number.
 *   &returns: True if success.
 */
bool json_int_range(struct json_t *json, int low, int high, int *out)
{
	int val;

	if(!json_get_int(json, &val))
		return false;
	else if((val < low) || (val > high))
		return false;

	if(out != NULL)
		*out = val;

	return true;
}


/**
 * Check if a value is an double.
 *   @json: The JSON value.
 *   @out: Ref. Optional. The output number.
 *   &returns: True if success.
 */
bool json_get_double(struct json_t *json, double *out)
{
	if(json->type != json_num_v)
		return false;

	if(out != NULL)
		*out = json->data.num;

	return true;
}

/**
 * Check if a value is an integer.
 *   @json: The JSON value.
 *   @out: Ref. Optional. The output number.
 *   &returns: True if success.
 */
bool json_get_int(struct json_t *json, int *out)
{
	int val;

	if(json->type != json_num_v)
		return false;

	val = json->data.num;
	if(val != json->data.num)
		return false;

	if(out != NULL)
		*out = val;

	return true;
}

/**
 * Check if a value is a 16-bit unsigned integer.
 *   @json: The JSON value.
 *   @out: Ref. Optional. The output number.
 *   &returns: True if success.
 */
bool json_get_uint16(struct json_t *json, uint16_t *out)
{
	uint16_t val;

	if(json->type != json_num_v)
		return false;

	val = json->data.num;
	if(val != json->data.num)
		return false;

	if(out != NULL)
		*out = val;

	return true;
}


/**
 * Verify a JSON value is an array with the proper length.
 *   @json: The JSON value.
 *   @len: The length. Negative indicates any length is acceptable.
 *   &returns: The array if valid, null otherwise.
 */
struct json_arr_t *json_chk_arr(struct json_t *json, int len)
{
	struct json_arr_t *arr;

	if(json->type != json_arr_v)
		return NULL;

	arr = json->data.arr;
	if((len >= 0) && (arr->len != len))
		return NULL;

	return arr;
}

/**
 * Verify a JSON value is an object with the requested members.
 *   @json: The JSON value.
 *   @...: Null-delimited member list.
 *   &returns: The object if valid, null otherwise.
 */
struct json_obj_t *json_chk_obj(struct json_t *json, ...)
{
	va_list args;
	const char *id;
	struct json_obj_t *obj;
	unsigned int cnt = 0;

	if(json->type != json_obj_v)
		return NULL;

	obj = json->data.obj;
	va_start(args, json);

	while((id = va_arg(args, const char *)) != NULL) {
		if(json_obj_get(obj, id) == NULL)
			break;

		cnt++;
	}

	va_end(args);

	return ((id == NULL) && (obj->root.count == cnt)) ? obj : NULL;
}


char *json_getf(struct json_t *json, const char *restrict fmt, ...)
{
	char *err;
	va_list args;

	va_start(args, fmt);
	err = json_vgetf(json, fmt, args);
	va_end(args);

	return err;
}

char *json_vgetf(struct json_t *json, const char *restrict fmt, va_list args)
{
	char *err;
	struct arglist_t arglist;

	va_copy(arglist.args, args);
	err = json_vgetfptr(json, &fmt, &arglist);
	va_end(arglist.args);

	return err;
}

char *json_vgetfptr(struct json_t *json, const char *restrict *restrict fmt, struct arglist_t *arglist)
{
	//const char *orig = *fmt;

	while(isspace(**fmt))
		(*fmt)++;

	switch(**fmt) {
	case '{':
		{
			size_t len;
			char *endptr;
			struct json_obj_t *obj;
			unsigned int cnt = 0;

			if(json->type != json_obj_v)
				return mprintf("Expected object.");

			(*fmt)++;
			obj = json->data.obj;

			while(true) {
				while(isspace(**fmt))
					(*fmt)++;

				if(**fmt == '\0')
					fatal("Invalid format string.");
				else if(**fmt == '}')
					break;

				endptr = strchr(*fmt, ':');
				if(endptr == NULL)
					fatal("Invalid format string.");

				len = endptr - *fmt;

				{
					char name[len + 1];

					memcpy(name, *fmt, len);
					name[len] = '\0';

					json = json_obj_getval(obj, name);
					if(json == NULL)
						return mprintf("Missing field '%s'.", name);

					cnt++;
					*fmt = endptr + 1;
					chkret(json_vgetfptr(json, fmt, arglist));
				}

				while(isspace(**fmt))
					(*fmt)++;

				if(**fmt == '}')
					break;
				else if(**fmt == '$') {
					if(obj->root.count != cnt)
						return mprintf("Too many fields.");

					(*fmt)++;
					while(isspace(**fmt))
						(*fmt)++;

					if(**fmt != '}')
						fatal("Invalid format string.");

					break;
				}
				else if(**fmt != ',')
					fatal("Invalid format string.");

				(*fmt)++;
			}

			(*fmt)++;
		}
		break;

	case 'b':
		{
			bool flag;

			if(json->type == json_true_v)
				flag = true;
			else if(json->type == json_false_v)
				flag = false;
			else
				return mprintf("Expected boolean.");

			(*fmt)++;
			*va_arg(arglist->args, bool *) = flag;
		}
		break;

	case 's':
		if(json->type != json_str_v)
			return mprintf("Expected string.");

		(*fmt)++;
		*va_arg(arglist->args, const char **) = json->data.str;
		break;

	case 'd':
		{
			double flt;
			int val;

			if(json->type != json_num_v)
				return mprintf("Expected integer.");

			val = flt = json->data.num;
			if(val != flt)
				return mprintf("Expected integer.");

			(*fmt)++;
			if(((*fmt)[0] == '1')  && ((*fmt)[1] == '6')) {
				if(val > INT16_MAX)
					return mprintf("Integer too large.");
				else if(val < INT16_MIN)
					return mprintf("Integer too small.");

				*fmt += 2;
				*va_arg(arglist->args, int16_t *) = val;
			}
			else
				*va_arg(arglist->args, int *) = val;

			break;
		}

	case 'u':
		{
			double flt;
			unsigned int val;

			if(json->type != json_num_v)
				return mprintf("Expected integer.");

			val = flt = json->data.num;
			if(flt < 0)
				return mprintf("Expected positive number.");
			else if(val != flt)
				return mprintf("Expected integer.");

			(*fmt)++;
			if(((*fmt)[0] == '1')  && ((*fmt)[1] == '6')) {
				if(val > UINT16_MAX)
					return mprintf("Integer too large.");

				*fmt += 2;
				*va_arg(arglist->args, uint16_t *) = val;
			}
			else
				*va_arg(arglist->args, unsigned int *) = val;

			break;
		}

	case 'f':
		if(json->type != json_num_v)
			return mprintf("Expected number.");

		(*fmt)++;
		*va_arg(arglist->args, double *) = json->data.num;
		break;

	default:
		fatal("Invalid format character '%c'.", **fmt);
	}

	return NULL;
}
