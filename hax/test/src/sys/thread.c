#include "../common.h"

/*
 * local declarations
 */
static void *thread0(void *ptr);

static void task0(sys_fd_t fd, void *ptr);


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
static void *thread0(void *ptr)
{
	return (void *)5;
}


/**
 * Perform tests on the task implementation.
 *   &returns: Success flag.
 */
bool test_sys_task(void)
{
	bool suc = true;

	{
		int val = -1;
		struct sys_task_t *task;

		task = sys_task_new(task0, &val);
		sys_task_delete(task);

		suc &= chk(val == 5, "task0");
	}

	return suc;
}
static void task0(sys_fd_t fd, void *ptr)
{
	sys_poll1(fd, sys_poll_in_e, -1);
	*(int *)ptr = 5;
}
