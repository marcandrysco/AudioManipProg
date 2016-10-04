#include "common.h"


/*
 * local declarations
 */
static char *optlong(char ***arg, const char *opt);


/**
 * Create a string list.
 *   &returns: The list.
 */
char **strlist_new(void)
{
	char **list;

	list = malloc(sizeof(char *));
	list[0] = NULL;

	return list;
}

/**
 * Add a string to the string list.
 *   @list: Ref. The string list reference.
 *   @str: The new string.
 */
void strlist_add(char ***list, char *str)
{
	unsigned int i;

	i = 0;
	while((*list)[i] != NULL)
		i++;

	*list = realloc(*list, (i + 2) * sizeof(void *));
	(*list)[i] = str;
	(*list)[i+1] = NULL;
}

/**
 * Delete a string list.
 *   @list: The string list.
 */
void strlist_delete(char **list)
{
	unsigned int i;

	for(i = 0; list[i] != NULL; i++)
		free(list[i]);

	free(list);
}


/**
 * Configure line structure.
 *   @n: The number of values.
 *   @key, val: The key and value list.
 */
struct cfg_line_t {
	unsigned int n;
	char *key, **val;
};

char *get_str(FILE *file, int *ch, char **str)
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


/*
 * configuration declarations
 */
char *cfg_line_parse(struct cfg_line_t **line, FILE *file, unsigned int *lineno);
void cfg_line_delete(struct cfg_line_t *line);


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

	chkfail(get_str(file, &ch, &(*line)->key));

	while(true) {
		while(isspace(ch) && (ch != '\n'))
			ch = fgetc(file);

		if((ch == '\n') || (ch == EOF))
			break;

		(*line)->val = realloc((*line)->val, ((*line)->n + 1) * sizeof(char *));
		chkfail(get_str(file, &ch, &(*line)->val[(*line)->n]));
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

		for(i = 0; func[i].key != NULL; i++) {
			if(strcmp(line->key, func[i].key) == 0)
				break;
		}

		if(func[i].key == NULL)
			fail("%s:%u: Unknown key '%s.", path, lineno, line->key);

		chkfail(func[i].proc(line, arg));
		cfg_line_delete(line);
	}

	fclose(file);

	return NULL;
#undef onexit
}


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

		if(*fmt == '\0') {
			if(n != 0)
				fail("Extra values.");
			else
				break;
		}
		else if(n == 0)
			fail("Missing value.");

		switch(*fmt) {
		case 'd':
			{
				long v;
				char *endptr;

				errno = 0;
				v = strtol(*val, &endptr, 0);
				if((errno != 0) || (*endptr != '\0') || (v > INT_MAX) || (v < INT_MIN))
					fail("Value '%s' is not a valid integer.", *val);

				val++; n--; fmt++;
				*va_arg(args, int *) = v;

				if(*fmt == '{') {
					int low, high, len;

					sscanf(fmt, "{%d,%d}%n", &low, &high, &len);

					printf("[%d %d %d]\n", low, high, len);

					fmt += len;
				}
			}
			break;

		default:
			fatal("Invalid format specifier '%c'.", *fmt);
		}
	}
	va_end(args);

	return NULL;
#undef onexit
}


/*
char *cfg_matchf(struct cfg_line_t *line, const char *restrict format, ...)
{
	chkret(cfg_line_parse(&line, file, lineno));

	while(true) {
		while(isspace(*format) || (*format == ','))
			format++;

		if(*format == '\0');

	return NULL;
}
*/


/**
 * Main entry point.
 *   @argc: The number of arguments.
 *   @argv: The argument array.
 *   &returns: Always zero.
 */
int main(int argc, char **argv)
{
	{
		struct cfg_line_t *line;
		unsigned int lineno = 1;
		FILE *file;

		file = fopen("test", "r");
		assert(file != NULL);

		chkwarn(cfg_line_parse(&line, file, &lineno));

		int v;
		chkwarn(cfg_readf(line, "d", &v));
		printf("%s: ", line->key);
		for(int i = 0; i < line->n; i++)
			printf("[%s] ", line->val[i]);
		printf("\n");

		return 0;
	}

	struct amp_audio_t audio;
	struct amp_comm_t *comm;
	const struct amp_audio_i *iface = NULL;
	char **arg, *file = NULL, **plugin, *val, *conf = NULL;

	srand(sys_utime());

	plugin = strlist_new();
	comm = amp_comm_new();

	for(arg = argv + 1; *arg != NULL; arg++) {
		if((*arg)[0] != '-') {
			if(file != NULL)
				fprintf(stderr, "Cannot specify two files."), exit(1);

			file = *arg;
		}
		else if((*arg)[1] == '-') {
			if((val = optlong(&arg, "--plugin")) != NULL)
				strlist_add(&plugin, strdup(val));
			else if((val = optlong(&arg, "--alsa")) != NULL) {
				if(iface != NULL)
					fprintf(stderr, "Cannot specify multiple audio interfaces.\n"), exit(1);

#if ALSA
				conf = val;
				iface = &alsa_audio_iface;
#else
				fprintf(stderr, "ALSA is unavailable.\n"), exit(1);
#endif
			}
			else if((val = optlong(&arg, "--pulse")) != NULL) {
				if(iface != NULL)
					fprintf(stderr, "Cannot specify multiple audio interfaces.\n"), exit(1);

#if PULSE
				conf = val;
				iface = &pulse_audio_iface;
#else
				fprintf(stderr, "PulseAudio is unavailable.\n"), exit(1);
#endif
			}
			else if((val = optlong(&arg, "--midi")) != NULL) {
				const char *str;
				unsigned long dev;
				struct amp_midi_i *iface;

				switch(val[0]) {
				case 'a':
#if ALSA
					iface = &alsa_midi_iface;
#else
					fprintf(stderr, "ALSA is unavailable.\n"), exit(1);
#endif
					break;

				default:
					fprintf(stderr, "Invalid MIDI interface '%c'.\n", val[0]), exit(1);
				}

				str = val + 1;
				if(isdigit(*str)) {
					errno = 0;
					dev = strtoul(str, (char **)&str, 0);
					if((errno != 0) || (dev > UINT16_MAX))
						fprintf(stderr, "Invalid device number.\n"), exit(1);
				}
				else
					dev = 0;

				while(isspace(*str))
					str++;

				if(*str == '\0')
					fprintf(stderr, "MIDI device missing identifier.\n"), exit(1);

				amp_comm_add(comm, dev, str, iface);
			}
			else
				fprintf(stderr, "Unknown option '%s'.\n", *arg), exit(1);
		}
		else
			fprintf(stderr, "Cannot handle short options.\n"), exit(1);
	}

	if(iface == NULL)
		fprintf(stderr, "No audio interface selected.\n"), exit(1);
	if(file == NULL)
		fprintf(stderr, "Missing source file.\n"), exit(1);

	audio = amp_audio_open(conf, iface);
	amp_exec(audio, file, plugin, comm);
	amp_audio_close(audio);
	strlist_delete(plugin);

	if(hax_memcnt != 0)
		fprintf(stderr, "allocated memory: %d\n", hax_memcnt);

	return 0;
}

/**
 * Retrieve a long option.
 *   @arg: The argument pointer.
 *   @opt: The option.
 *   &returns: The value if matched, false otherwise.
 */
static char *optlong(char ***arg, const char *opt)
{
	unsigned int len = strlen(opt);

	if(memcmp(**arg, opt, len) != 0)
		return NULL;

	if((**arg)[len] == '=') {
		return **arg + len + 1;
	}
	else if((**arg)[len] == '\0') {
		(*arg)++;
		if(**arg == NULL)
			fprintf(stderr, "Option '%s' expected parameter.\n", opt), exit(1);

		return **arg;
	}
	else
		return NULL;
}
