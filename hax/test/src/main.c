#include "common.h"

/*
 * test declarations
 */
bool test_avltree(void);


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

	if(hax_memcnt != 0)
		suc &= false, fprintf(stderr, "Error. Missed %d allocations.\n", hax_memcnt);

	return suc ? 0 : 1;
}
