#include "common.h"



int DBG_memcnt = 0;
int DBG_rescnt = 0;


/**
 * Main entry function.
 *   @argc: The number of arguments.
 *   @argv: The argument array.
 *   &returns: Always zero.
 */

int main(int argc, char **argv)
{
	char *err = NULL;
	struct ml_token_t *token;
	struct ml_env_t *env;

	token = ml_token_load("test.ml", &err);
	if(err != NULL)
		fprintf(stderr, "%s\n", err), abort();

	env = ml_env_new();

	ml_parse_top(token, env, &err);
	if(err != NULL)
		fprintf(stderr, "%s\n", err), abort();

	ml_env_delete(env);
	ml_token_clean(token);

	if(DBG_memcnt != 0)
		printf("memcnt: %d\n", DBG_memcnt);

	return 0;
}
