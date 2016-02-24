#include "common.h"


char *bufinit(unsigned int len)
{
	return malloc(len);
}

void bufadd(char **str, unsigned int *idx, unsigned int *len, char ch)
{
	if(*idx == *len) {
		*str = realloc(*str, 2 * *len);
		*len *= 2;
	}

	(*str)[(*idx)++] = ch;
}

char *bufdone(char *str, unsigned int *idx, unsigned int *len)
{
	bufadd(&str, idx, len, '\0');

	return str;
}


/**
 * Parse an argument array.
 *   @str: The input string.
 *   @argv: The output argument array.
 *   @argc: The output argument count.
 */

void argv_parse(const char *str, char ***argv, unsigned int *argc)
{
	char *buf;
	unsigned int i, len, n = 0;

	*argv = malloc(sizeof(char *));

	while(true) {
		while(isspace(*str))
			str++;

		if(*str == '\0')
			break;

		i = 0;
		buf = bufinit(len = 32);

		if(*str == '"') {
			str++;
			while((*str != '"') && (*str != '\0')) {
				if(*str == '\\')
					str++;

				bufadd(&buf, &i, &len, *str++);
			}

			if(*str != '\0')
				str++;
		}
		else {
			while(!isspace(*str) && (*str != '\0')) {
				if(*str == '\\')
					str++;

				bufadd(&buf, &i, &len, *str++);
			}
		}

		(*argv)[n++] = bufdone(buf, &i, &len);
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
 *   &reutrns: The arguments in a single allocated string. Must be freed with
 *     'mem_free'.
 */

char *argv_serial(char **argv)
{
	char *buf;
	const char *str;
	unsigned int i, len;

	if(*argv == NULL)
		return NULL;

	buf = bufinit(len = 32);

	while(true) {
		bufadd(&buf, &i, &len, '\"');

		for(str = *argv; *str != '\0'; str++) {
			if((*str == '"') || (*str == '\\'))
				bufadd(&buf, &i, &len, '\\');

			bufadd(&buf, &i, &len, *str);
		}

		bufadd(&buf, &i, &len, '\"');

		argv++;
		if(*argv == NULL)
			break;

		bufadd(&buf, &i, &len, ' ');
	}

	return bufdone(buf, &i, &len);
}
