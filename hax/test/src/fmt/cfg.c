#include "../common.h"

/*
 * local declarations
 */
static char *opt(struct cfg_line_t *line, void *arg);


/**
 * Perform tests on the configuration implementation.
 *   &returns: Success flag.
 */
bool test_cfg(void)
{
	bool suc = true;

	{
		static struct cfg_load_t spec[] = {
			{ "Opt",  opt },
			{ "Many", opt },
			{ "Vec",  opt },
			{ NULL,   NULL }
		};

		suc &= chkwarn(cfg_load("dat/cfg", spec, NULL));
	}

	return suc;
}

/**
 * Process the option.
 *   @line: The line.
 *   @arg: The argument.
 *   &returns: Error.
 */
static char *opt(struct cfg_line_t *line, void *arg)
{
	if(strcasecmp(line->key, "Opt") == 0) {
		int v;

		chkabort(cfg_read_int(line->val[0], &v));
		if(v != 12)
			return mprintf("Failed to read 12 from 'Opt'.");
	}
	else if(strcasecmp(line->key, "Many") == 0) {
		int i;
		unsigned int u;
		const char *str;

		chkabort(cfg_readf(line, "uds$", &u, &i, &str));

		if((u != 0x10) || (i != 40) || (strcmp(str, "Str") != 0))
			return mprintf("Failed to read values from 'Many'.");
	}
	else if(strcasecmp(line->key, "Vec") == 0) {
		uint16_t *vec;
		unsigned int len;

		chkabort(cfg_readf(line, "u16*", &vec, &len));

		if((len != 4) || (vec[0] != 0) || (vec[1] != 4) || (vec[2] != 8) || (vec[3] != 2))
			return mprintf("Failed to read values from 'Vec'.");

		free(vec);
	}
	else
		return mprintf("Invalid option '%s'.", line->key);

	return NULL;
}
