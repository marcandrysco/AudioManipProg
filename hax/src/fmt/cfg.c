#include "../common.h"

/*
 * local declarations
 */
static char *getstr(FILE *file, int *ch, char **str);


/**
 * Parse a line from a file..
 *   @ret: Ref: The line.
 *   @file: The file.
 *   @lineno: Ref. Optional. The line number.
 *   &returns: Error.
 */
char *cfg_line_parse(struct cfg_line_t **line, FILE *file, unsigned int *lineno)
{
#define onexit cfg_line_delete(*line);
	int ch;

	*line = NULL;
	do {
		ch = fgetc(file);
		if((ch == '\n') && (lineno != NULL))
			(*lineno)++;
	} while(isspace(ch));

	if(ch == EOF)
		return NULL;

	*line = malloc(sizeof(struct cfg_line_t));
	(*line)->n = 0;
	(*line)->key = NULL;
	(*line)->val = malloc(0);

	chkfail(getstr(file, &ch, &(*line)->key));

	while(true) {
		while(isspace(ch) && (ch != '\n'))
			ch = fgetc(file);

		if((ch == '\n') || (ch == EOF))
			break;

		(*line)->val = realloc((*line)->val, ((*line)->n + 1) * sizeof(char *));
		chkfail(getstr(file, &ch, &(*line)->val[(*line)->n]));
		(*line)->n++;
	}

	if(lineno != NULL)
		(*lineno)++;

	return NULL;
#undef onexit
}

/**
 * Delete a line.
 *   @line: The line.
 */
void cfg_line_delete(struct cfg_line_t *line)
{
	unsigned int i;

	for(i = 0; i < line->n; i++)
		free(line->val[i]);

	erase(line->key);
	free(line->val);
	free(line);
}


/**
 * Load a configuration file using callbacks.
 *   @path: The path.
 *   @func: The function.
 *   @arg: The argument.
 */
char *cfg_load(const char *path, struct cfg_load_t *func, void *arg)
{
#define onexit fclose(file); if(line != NULL) cfg_line_delete(line);
	FILE *file;
	unsigned int i, lineno = 1;
	struct cfg_line_t *line = NULL;

	file = fopen(path, "r");
	if(file == NULL)
		return mprintf("Unable to open '%s'. %s.", strerror(errno));

	while(true) {
		line = NULL;
		chkfail(cfg_line_parse(&line, file, &lineno));
		if(line == NULL)
			break;

		for(i = 0; func[i].key != NULL; i++) {
			if(strcasecmp(line->key, func[i].key) == 0)
				break;
		}

		if(func[i].key == NULL)
			fail("%s:%u: Unknown key '%s'.", path, lineno, line->key);

		chkfail(func[i].proc(line, arg));
		cfg_line_delete(line);
	}

	fclose(file);

	return NULL;
#undef onexit
}


/**
 * Read an integer from a value.
 *   @value: The value.
 *   @ptr: The pointer.
 *   &returns: Error.
 */
char *cfg_read_int(const char *value, int *ptr)
{
	long val;

	errno = 0;
	val = strtol(value, (char **)&value, 0);
	if((errno != 0) || (*value != '\0'))
		return mprintf("Invalid value.");
	else if((val < INT_MIN) || (val > INT_MAX))
		return mprintf("Value too large.");

	*ptr = val;
	return NULL;
}

/**
 * Read an unsigned integer from a value.
 *   @value: The value.
 *   @ptr: The pointer.
 *   &returns: Error.
 */
char *cfg_read_uint(const char *value, unsigned int *ptr)
{
	long val;

	errno = 0;
	val = strtol(value, (char **)&value, 0);
	if((errno != 0) || (*value != '\0'))
		return mprintf("Invalid value.");
	else if((val < 0) || (val > UINT_MAX))
		return mprintf("Value too large.");

	*ptr = val;
	return NULL;
}

/**
 * Read a 16-bit unsigned integer from a value.
 *   @value: The value.
 *   @ptr: The pointer.
 *   &returns: Error.
 */
char *cfg_read_uint16(const char *value, uint16_t *ptr)
{
	long val;

	errno = 0;
	val = strtol(value, (char **)&value, 0);
	if((errno != 0) || (*value != '\0'))
		return mprintf("Invalid value.");
	else if((val < 0) || (val > UINT16_MAX))
		return mprintf("Value too large.");

	*ptr = val;
	return NULL;
}

/**
 * Read a 16-bit unsigned integer vector from a value list.
 *   @value: The value list.
 *   @n: The number of values.
 *   @vec: Ref. The destination vector.
 *   @len: Ref. The length.
 *   &returns: Error.
 */
char *cfg_read_uint16vec(char **value, unsigned int n, uint16_t **vec, unsigned int *len)
{
	unsigned int i;
	uint16_t arr[n];

	for(i = 0; i < n; i++)
		chkabort(cfg_read_uint16(value[i], &arr[i]));

	*vec = malloc(n * sizeof(uint16_t));
	memcpy(*vec, arr, (*len = n) * sizeof(uint16_t));

	return NULL;
}

/**
 * Read a boolean from a value.
 *   @value: The value.
 *   @ptr: The pointer.
 *   &returns: Error.
 */
char *cfg_read_bool(const char *value, bool *ptr)
{
	if((strcasecmp(value, "true") == 0) || (strcasecmp(value, "t") == 0) || (strcmp(value, "1") == 0))
		*ptr = true;
	else if((strcasecmp(value, "false") == 0) || (strcasecmp(value, "f") == 0) || (strcmp(value, "0") == 0))
		*ptr = false;
	else
		return mprintf("Invalid boolean '%s'.", value);

	return NULL;
}

/**
 * Read a double from a value.
 *   @value: The value.
 *   @ptr: The pointer.
 *   &returns: Error.
 */
char *cfg_read_double(const char *value, double *ptr)
{
	double val;

	errno = 0;
	val = strtod(value, (char **)&value);
	if((errno != 0) || (*value != '\0'))
		return mprintf("Invalid value.");

	*ptr = val;
	return NULL;
}

/**
 * Read formatted input from a configuration line.
 *   @file: The file.
 *   @key: The key.
 *   @fmt, ...: The format and options.
 *   &returns: Error.
 */
char *cfg_readf(struct cfg_line_t *line, const char *restrict fmt, ...)
{
#define onexit va_end(args);
	va_list args;
	char **val = line->val;
	unsigned int n = line->n;

	va_start(args, fmt);

	while(true) {
		while(isspace(*fmt) || (*fmt == ','))
			fmt++;

		if(*fmt == '$') {
			if(n != 0)
				fail("Not enough parameters for '%s'.", line->key);
			else
				break;
		}
		else if(*fmt == '\0')
			break;
		else if(n == 0)
			fail("Not enough parameters for '%s'.", line->key);

		switch(*fmt) {
		case 's':
			*va_arg(args, const char **) = *val;
			val++, n--, fmt++;
			break;

		case 'd':
			chkfail(cfg_read_int(*val, va_arg(args, int *)));
			val++, n--, fmt++;
			break;

		case 'u':
			fmt++;
			if((fmt[0] == '1') && (fmt[1] == '6') && (fmt[2] == '*')) {
				uint16_t **vec = va_arg(args, uint16_t **);
				unsigned int *len = va_arg(args, unsigned int *);
				chkfail(cfg_read_uint16vec(val, n, vec, len));
				fmt += 3; val += n; n = 0;
			}
			else if((fmt[0] == '1') && (fmt[1] == '6')) {
				chkfail(cfg_read_uint16(*val, va_arg(args, uint16_t *)));
				fmt += 2; val++, n--;
			}
			else if(fmt[2] == '*') {
				unsigned int i, arr[n], **vec;

				for(i = 0; i < n; i++)
					chkfail(cfg_read_uint(val[i], &arr[i]));

				fmt += 3;
				vec = va_arg(args, unsigned int **);
				*vec = malloc(n * sizeof(unsigned int));
				memcpy(*vec, arr, n * sizeof(unsigned int));
				*va_arg(args, unsigned int *) = n;
			}
			else {
				chkfail(cfg_read_int(*val, va_arg(args, int *)));
				val++, n--;
			}
			break;

		case 'b':
			chkfail(cfg_read_bool(*val, va_arg(args, bool *)));
			val++, n--; fmt++;
			break;

		case 'f':
			chkfail(cfg_read_double(*val, va_arg(args, double *)));
			val++, n--; fmt++;
			break;

		default:
			fatal("Invalid format specifier '%c'.", *fmt);
		}
	}

	va_end(args);

	return NULL;
#undef onexit
}


/**
 * Write an integer.
 *   @file: The file.
 *   @val: The value.
 */
void cfg_write_int(FILE *file, int val)
{
	fprintf(file, " \"%d\"", val);
}

/**
 * Write an unsigned integer.
 *   @file: The file.
 *   @val: The value.
 */
void cfg_write_uint(FILE *file, unsigned int val)
{
	fprintf(file, " \"%u\"", val);
}

/**
 * Write an unsigned 16-bit integer.
 *   @file: The file.
 *   @val: The value.
 */
void cfg_write_uint16(FILE *file, uint16_t val)
{
	fprintf(file, " \"%u\"", val);
}

/**
 * Write an unsigned 16-bit integer vector.
 *   @file: The file.
 *   @vec: The vector.
 *   @len: The length of the vector.
 */
void cfg_write_uint16vec(FILE *file, uint16_t *vec, unsigned int len)
{
	unsigned int i;

	for(i = 0; i < len; i++)
		cfg_write_uint16(file, vec[i]);
}

/**
 * Write boolean.
 *   @file: The file.
 *   @val: The value.
 */
void cfg_write_bool(FILE *file, bool val)
{
	fprintf(file, " \"%s\"", val ? "true" : "false");
}

/**
 * Write double.
 *   @file: The file.
 *   @val: The value.
 */
void cfg_write_double(FILE *file, double val)
{
	fprintf(file, " \"%.17g\"", val);
}

/**
 * Write string.
 *   @file: The file.
 *   @val: The value.
 */
void cfg_write_string(FILE *file, const char *val)
{
	const char *fnd;

	fprintf(file, " \"");

	while((fnd = strpbrk(val, "\"\\")) != NULL) {
		fwrite(val, 1, fnd - val, file);
		fprintf(file, "\\%c", *fnd);
		val = fnd + 1;
	}

	fprintf(file, "%s\"", val);
}

/**
 * Write formatted output to a configuration file.
 *   @file: The file.
 *   @key: The key.
 *   @fmt, ...: The format and options.
 */
void cfg_writef(FILE *file, const char *restrict key, const char *restrict fmt, ...)
{
	va_list args;
	char *endptr;

	fprintf(file, "%s", key);
	va_start(args, fmt);

	while(true) {
		while(isspace(*fmt) || (*fmt == ','))
			fmt++;

		if(*fmt == '\0')
			break;

		if((endptr = strprefix(fmt, "d")) != NULL)
			cfg_write_int(file, va_arg(args, int));
		else if((endptr = strprefix(fmt, "u16*")) != NULL) {
			uint16_t *arr = va_arg(args, uint16_t *);
			unsigned int len = va_arg(args, unsigned int);

			cfg_write_uint16vec(file, arr, len);
		}
		else if((endptr = strprefix(fmt, "u16")) != NULL)
			cfg_write_uint16(file, va_arg(args, int));
		else if((endptr = strprefix(fmt, "u")) != NULL)
			cfg_write_uint(file, va_arg(args, unsigned int));
		else if((endptr = strprefix(fmt, "b")) != NULL)
			cfg_write_bool(file, va_arg(args, int));
		else if((endptr = strprefix(fmt, "f")) != NULL)
			cfg_write_double(file, va_arg(args, double));
		else if((endptr = strprefix(fmt, "s")) != NULL)
			cfg_write_string(file, va_arg(args, const char *));
		else
			fatal("Invalid format specifier '%c'.", *fmt);

		fmt = endptr;
	}

	fprintf(file, "\n");
	va_end(args);
}


/**
 * Read a string from the file.
 *   @file: The file.
 *   @ch: Ref. The current character.
 *   @str: Ref. Output string.
 *   &returns: Error.
 */
static char *getstr(FILE *file, int *ch, char **str)
{
#define onexit strbuf_destroy(&buf);
	struct strbuf_t buf;

	buf = strbuf_init(32);

	if(*ch == '"') {
		while(true) {
			*ch = fgetc(file);
			if(*ch == '"')
				break;

			switch(*ch) {
			case EOF:
				fail("Unexpected end-of-file.");

			case '\n':
				fail("Unterminated quote.");

			default:
				strbuf_addch(&buf, *ch);
				break;
			}
		}
		*ch = fgetc(file);
	}
	else if(*ch == '\'') {
		fatal("stub");
	}
	else {
		do {
			strbuf_addch(&buf, *ch);
			*ch = fgetc(file);
		} while((*ch != EOF) && !isspace(*ch));
	}

	*str = strbuf_done(&buf);
	return NULL;
#undef onexit
}
