#include "common.h"


/*
 * test declarations
 */
bool test_avltree(void);
bool test_printf(void);


/**
 * Main entry.
 *   @argc: The argument count.
 *   @argv: The argument array.
 *   &return: The exit code.
 */
int main(int argc, char **argv)
{
	bool suc = true;

	suc &= test_avltree();
	suc &= test_printf();

	if(hax_memcnt != 0)
		suc &= false, fprintf(stderr, "Error. Missed %d allocations.\n", hax_memcnt);

	if(suc)
		printf("success\n");
	else
		printf("FAIL\n");

	return suc ? 0 : 1;
}
