#include "common.h"


/*
 * test declarations
 */
bool test_printf(void);

bool test_avltree(void);
bool test_strtrie(void);

bool test_sys_notify(void);
bool test_sys_task(void);
bool test_sys_thread(void);

bool test_cfg(void);
bool test_json(void);


/**
 * Main entry.
 *   @argc: The argument count.
 *   @argv: The argument array.
 *   &return: The exit code.
 */
int main(int argc, char **argv)
{
	bool suc = true;

	setbuf(stdout, NULL);
	setbuf(stderr, NULL);
	setlocale(LC_CTYPE, "");

	suc &= test_printf();

	suc &= test_avltree();
	suc &= test_strtrie();

	suc &= test_sys_thread();
	suc &= test_sys_task();
	//suc &= test_sys_notify();

	suc &= test_cfg();
	suc &= test_json();

	if(hax_memcnt != 0)
		suc &= false, fprintf(stderr, "Error. Missed %d allocations.\n", hax_memcnt);

	if(suc)
		printf("success\n");
	else
		printf("=== FAIL ===\n");

	return suc ? 0 : 1;
}
