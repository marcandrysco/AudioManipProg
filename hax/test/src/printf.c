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

		str = mprintf("hi %d there %u %zu", 10, 20, 24749814879);
		suc &= chk(strcmp(str, "hi 10 there 20 24749814879") == 0, "printf0");
		free(str);
	}

	return suc;
}
