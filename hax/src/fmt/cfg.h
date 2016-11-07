#ifndef FMT_CFG_H
#define FMT_CFG_H

/**
 * Configure line structure.
 *   @n: The number of values.
 *   @key, val: The key and value list.
 */
struct cfg_line_t {
	unsigned int n;
	char *key, **val;
};


/**
 * Load function.
 *   @line: The line.
 *   @arg: The argument.
 */
typedef char *(*cfg_load_f)(struct cfg_line_t *line, void *arg);

/**
 * Configuration processing structure.
 *   @key: The key.
 *   @proc: The processing function.
 */
struct cfg_load_t {
	const char *key;
	cfg_load_f proc;
};


/*
 * configuration declarations
 */
char *cfg_line_parse(struct cfg_line_t **line, FILE *file, unsigned int *lineno);
void cfg_line_delete(struct cfg_line_t *line);

char *cfg_load(const char *path, struct cfg_load_t *func, void *arg);

char *cfg_read_int(const char *value, int *ptr);
char *cfg_read_uint(const char *value, unsigned int *ptr);
char *cfg_read_uint16(const char *value, uint16_t *ptr);
char *cfg_read_uint16vec(char **value, unsigned int n, uint16_t **vec, unsigned int *len);
char *cfg_read_bool(const char *value, bool *ptr);
char *cfg_read_double(const char *value, double *ptr);
char *cfg_readf(struct cfg_line_t *line, const char *restrict fmt, ...);

void cfg_write_int(FILE *file, int val);
void cfg_write_uint(FILE *file, unsigned int val);
void cfg_write_uint16(FILE *file, uint16_t val);
void cfg_write_uint16vec(FILE *file, uint16_t *vec, unsigned int len);
void cfg_write_bool(FILE *file, bool val);
void cfg_write_double(FILE *file, double val);
void cfg_write_string(FILE *file, const char *val);
void cfg_writef(FILE *file, const char *restrict key, const char *restrict fmt, ...);

/**
 * Delete a line if non-null.
 *   @line: The line.
 */
static inline void cfg_line_erase(struct cfg_line_t *line)
{
	if(line != NULL)
		cfg_line_delete(line);
}

#endif
