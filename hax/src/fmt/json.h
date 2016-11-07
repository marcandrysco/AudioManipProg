#ifndef JSON_H
#define JSON_H


/**
 * JSON type enumerator.
 *   @json_null_v: Null.
 *   @json_true_v: True boolean.
 *   @json_false_v: False boolean.
 *   @json_num_v: Number.
 *   @json_str_v: String.
 *   @json_arr_v: Array.
 *   @json_obj_v: Object.
 */
enum json_e {
	json_null_v,
	json_true_v,
	json_false_v,
	json_num_v,
	json_str_v,
	json_obj_v,
	json_arr_v
};

/**
 * JSON data union.
 *   @num: The number.
 *   @str: The string.
 *   @arr: The array.
 *   @obj: The object.
 */
union json_u {
	double num;
	char *str;
	struct json_arr_t *arr;
	struct json_obj_t *obj;
};

/**
 * JSON value structure.
 *   @type: The type.
 *   @data: The data.
 */
struct json_t {
	enum json_e type;
	union json_u data;
};


/**
 * JSON array structure.
 *   @vec: The vector.
 *   @len: The length.
 */
struct json_arr_t {
	struct json_t **vec;
	unsigned int len;
};

/**
 * JSON object structure.
 *   @root: Property tree root.
 */
struct json_obj_t {
	struct avltree_root_t root;
};

/**
 * JSON property structure.
 *   @key: The key.
 *   @value: The value.
 *   @node: The tree node.
 */
struct json_prop_t {
	char *key;
	struct json_t *value;

	struct avltree_node_t node;
};

/*
 * json declarations
 */
struct json_t *json_new(enum json_e type, union json_u data);
void json_delete(struct json_t *json);

struct json_t *json_null(void);
struct json_t *json_bool(bool flag);
struct json_t *json_num(double num);
struct json_t *json_str(char *str);
struct json_t *json_arr(struct json_arr_t *arr);

struct io_chunk_t json_chunk(const struct json_t *json);
void json_print(struct json_t *json, struct io_file_t file);

/*
 * array declarations
 */
struct json_arr_t *json_arr_new(void);
void json_arr_delete(struct json_arr_t *arr);

void json_arr_append(struct json_arr_t *arr, struct json_t *el);

struct io_chunk_t json_arr_chunk(const struct json_t *json);
void json_arr_print(struct json_arr_t *arr, struct io_file_t file);

/*
 * object declarations
 */
struct json_obj_t *json_obj_new(void);
void json_obj_delete(struct json_obj_t *obj);

struct json_prop_t *json_obj_add(struct json_obj_t *obj, char *key, struct json_t *value);
struct json_prop_t *json_obj_get(struct json_obj_t *obj, const char *key);
struct json_t *json_obj_getval(struct json_obj_t *obj, const char *key);

struct json_prop_t *json_obj_first(struct json_obj_t *obj);
struct json_prop_t *json_obj_last(struct json_obj_t *obj);
struct json_prop_t *json_obj_prev(struct json_prop_t *prop);
struct json_prop_t *json_obj_next(struct json_prop_t *prop);

struct io_chunk_t json_obj_chunk(const struct json_obj_t *json);
void json_obj_print(struct json_obj_t *obj, struct io_file_t file);

char *json_parse_file(struct json_t **json, struct io_file_t file);
char *json_parse_str(struct json_t **json, const char *str);
char *json_parse_path(struct json_t **json, const char *path);

/**
 * Retrieve the number of children in an object.
 *   @obj: The object.
 *   &returns: The number of children.
 */
static inline unsigned int json_obj_cnt(struct json_obj_t *obj)
{
	return obj->root.count;
}

/*
 * verification declarations
 */
double json_num_range(struct json_t *json, double low, double high);

bool json_str_get(struct json_t *json, const char **out);
bool json_str_objget(struct json_obj_t *obj, const char *id, const char **out);

bool json_get_double(struct json_t *json, double *out);
bool json_get_int(struct json_t *json, int *out);
bool json_get_uint16(struct json_t *json, uint16_t *out);

struct json_arr_t *json_get_arr(struct json_t *json, int len);
struct json_obj_t *json_chk_obj(struct json_t *json, ...);

char *json_getf(struct json_t *json, const char *restrict fmt, ...);
char *json_vgetf(struct json_t *json, const char *restrict fmt, va_list args);
char *json_vgetfptr(struct json_t *json, const char *restrict *restrict fmt, struct arglist_t *args);


/**
 * Delete a JSON object if non-null.
 *   @json: The JSON object.
 */
static inline void json_erase(struct json_t *json)
{
	if(json != NULL)
		json_delete(json);
}

#endif
