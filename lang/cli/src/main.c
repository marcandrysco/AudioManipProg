#include "common.h"


/**
 * Main entry point.
 *   @argc: The number of argument.
 *   @argv: The argument array.
 *   &returns: The exit status.
 */
int main(int argc, char **argv)
{
	int i;
	bool interact = false;
	struct ml_env_t *env;

	env = ml_env_new();

	for(i = 1; i < argc; i++) {
		if(argv[i][0] == '-') {
			int ii;

			for(ii = 1; argv[i][ii] != '\0'; ii++) {
				if(argv[i][ii] == 'i')
					interact = true;
				else
					fatal("Unknown option '-%c'.", argv[i][ii]);
			}
		}
		else
			//printf("proc: %p\n", ml_env_proc(argv[i], &env));
			chkexit(ml_parse_file(&env, argv[i]));
	}

	if(interact) {
		char line[4096];

		while(true) {
			printf("> ");
			fgets(line, sizeof(line), stdin);

			if(feof(stdin))
				break;
		}

		printf("\n");
	}

	ml_env_delete(env);

	if(hax_memcnt != 0)
		fprintf(stderr, "allocated memory: %d\n", hax_memcnt);

	return 0;
}
