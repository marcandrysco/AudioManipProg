#include "../common.h"

/*
 * local declarations
 */
static void *thread0(void *ptr);


/**
 * Perform tests on the threading implementation.
 *   &returns: Success flag.
 */
bool test_sys_thread(void)
{
	bool suc = true;

	{
		void *ret;
		sys_thread_t thread;

		thread = sys_thread_create(0, thread0, NULL);
		ret = sys_thread_join(&thread);

		suc &= chk(ret == (void *)5, "thread0");
	}

	return suc;
}

/*
 * thread callbacks
 */
static void *thread0(void *ptr)
{
	return (void *)5;
}
