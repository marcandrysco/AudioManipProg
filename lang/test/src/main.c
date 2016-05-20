#include <hax.h>
#include "../../muselang.h"

/*
 * test macros
 */
#define gen_err(path) fprintf(stderr, "Processed the invalid file '%s' without producing an error.\n", path);

#define token_suc(token, path) do { char *_err = ml_token_load(token, path); if(_err) { fprintf(stderr, "%s\n", _err); return 1; } } while(0)
#define token_err(token, path) do { char *_err = ml_token_load(token, path); if(_err) free(_err); else { ml_token_delete(*token); gen_err(path); return 1; } } while(0)


/**
 * Token test functions.
 *   &returns: Zero on success, one on error.
 */
int test_token1(void)
{
	struct ml_token_t *token;

	token_suc(&token, "ml/token1.ml");
	ml_token_delete(token);

	return 0;
}
int test_token2(void)
{
	struct ml_token_t *token;

	token_err(&token, "ml/token2.ml");

	return 0;
}
int test_file(const char *str, struct ml_value_t *expect)
{
	int res = 1;
	char *err;
	struct ml_env_t *env;

	env = ml_env_new();
	err = ml_parse_file(&env, str);

	if(err == NULL) {
		if(expect != NULL) {
			struct ml_value_t *value = ml_env_lookup(env, "result");
			if(value != NULL) {
				if(ml_value_cmp(value, expect) == 0)
					res = 0;
				else
					fprintf(stderr, "error: expected %C, got %C.\n", ml_value_chunk(expect), ml_value_chunk(value));
			}
			else
				fprintf(stderr, "error: missing variable 'result'\n");
		}
		else
			fprintf(stderr, "error: successfully parsed '%s' when error was expected\n", str);
	}
	else {
		if(expect != NULL)
			fprintf(stderr, "error: %s\n", err);
		else
			res = 0;

		free(err);
	}

	ml_env_delete(env);

	if(expect != NULL)
		ml_value_delete(expect);

	return res;
}


/**
 * Main entry point.
 *   @argc: The argument count.
 *   @argv: The argument array.
 *   &returns: Zero on success, non-zero on any failed test.
 */
int main(int argc, char **argv)
{
	int err = 0;

	err = test_token1();
	err = test_token2();
	err = test_file("ml/file1.ml", ml_value_num(2, ml_tag_copy(ml_tag_null)));
	err = test_file("ml/file2.ml", NULL);
	err = test_file("ml/file3.ml", NULL);
	err = test_file("ml/file4.ml", ml_value_tuple(ml_list_newl(ml_value_num(5, ml_tag_copy(ml_tag_null)), ml_value_num(3, ml_tag_copy(ml_tag_null)), NULL), ml_tag_copy(ml_tag_null)));
	err = test_file("ml/file5.ml", NULL);
	err = test_file("ml/file6.ml", ml_value_num(8, ml_tag_copy(ml_tag_null)));
	err = test_file("ml/file7.ml", ml_value_num(2, ml_tag_copy(ml_tag_null)));

	if(err > 0)
		fprintf(stderr, "test failures: %d\n", err);
	else
		fprintf(stderr, "success!\n");

	if(hax_memcnt != 0)
		fprintf(stderr, "allocated memory: %d\n", hax_memcnt);

	return (err > 0) ? 1 : 0;
}
