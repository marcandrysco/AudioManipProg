#include "common.h"


/**
 * Perform tests on the printf implementation.
 *   &returns: Success flag.
 */
bool test_printf(void)
{
	bool suc = true;

	{
		char *str;

		str = mprintf("hi %d there", 10);
		suc &= strcmp(str, "hi 10 there") == 0;
		free(str);
	}

	return suc;
}
