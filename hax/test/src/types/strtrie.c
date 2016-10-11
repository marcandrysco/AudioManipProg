#include "../common.h"


/**
 * Perform tests on the string trie implementation.
 *   &returns: Success flag.
 */
bool test_strtrie(void)
{
	bool suc = true;

	{
		struct strtrie_t trie;
		const char **iter;
		static const char *words[] = { "hello", "hi", "hullo", "hiya", "heya", "hey", "foobar", "foo", "foobaz", "bar", "baz", NULL };
		static const char *exclude[] = { "he", "heyo", "foobarbaz", "f", "", "hallo", NULL };

		trie = strtrie_init();

		for(iter = words; *iter != NULL; iter++)
			strtrie_add(&trie, *iter, *iter);

		for(iter = words; *iter != NULL; iter++)
			suc &= chk(strcmp(strtrie_lookup(&trie, *iter) ?: "", *iter) == 0, "strtrie0");

		for(iter = exclude; *iter != NULL; iter++)
			suc &= chk(strtrie_lookup(&trie, *iter) == NULL, "strtrie0");

		strtrie_destroy(&trie);
	}

	return suc;
}
