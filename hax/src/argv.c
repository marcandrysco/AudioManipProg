#include "common.h"


/**
 * Parse an argument array.
 *   @str: The input string.
 *   @argv: Ref. The output argument array.
 *   @argc: Ref. Optional. The output argument count.
 */
void argv_parse(const char *str, char ***argv, unsigned int *argc)
{
	struct strbuf_t buf;
	unsigned int n = 0;

	*argv = malloc(sizeof(char *));

	while(true) {
		while(isspace(*str))
			str++;

		if(*str == '\0')
			break;

		buf = strbuf_init(32);

		if(*str == '"') {
			str++;
			while((*str != '"') && (*str != '\0')) {
				if(*str == '\\')
					str++;

				strbuf_addch(&buf, *str++);
			}

			if(*str != '\0')
				str++;
		}
		else {
			while(!isspace(*str) && (*str != '\0')) {
				if(*str == '\\')
					str++;

				strbuf_addch(&buf, *str++);
			}
		}

		(*argv)[n++] = strbuf_done(&buf);
		*argv = realloc(*argv, (n + 1) * sizeof(char *));
	}

	(*argv)[n] = NULL;

	if(argc != NULL)
		*argc = n;
}

/**
 * Delete an argument array.
 *   @argv: The argument array.
 */
void argv_free(char **argv)
{
	char **arg;

	for(arg = argv; *arg != NULL; arg++)
		free(*arg);

	free(argv);
}

/**
 * Serialize the arguments into a string.
 *   @argv: The argument array.
 *   &reutrns: The arguments in a single string. Must be freed with 'free'.
 */
char *argv_serial(char *const *argv)
{
	struct strbuf_t buf;
	const char *str;

	if(*argv == NULL)
		return NULL;

	buf = strbuf_init(32);

	while(true) {
		strbuf_addch(&buf, '\"');

		for(str = *argv; *str != '\0'; str++) {
			if((*str == '"') || (*str == '\\'))
				strbuf_addch(&buf, '\\');

			strbuf_addch(&buf, *str);
		}

		strbuf_addch(&buf, '\"');

		argv++;
		if(*argv == NULL)
			break;

		strbuf_addch(&buf, ' ');
	}

	return strbuf_done(&buf);
}
