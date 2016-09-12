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

		str = mprintf("hi %d there %u", 10, 20);
		suc &= chk(strcmp(str, "hi 10 there 20") == 0, "printf0");
		free(str);
	}

	return suc;
}
