#include "../common.h"

/*
 * local declarations
 */
static void notify0(struct sys_change_t *change, void *ptr);


/**
 * Perform tests on the change notify implementation.
 *   &returns: Success flag.
 */
bool test_sys_notify(void)
{
	bool suc = true;

	{
		struct sys_task_t *task;
		
		task = sys_notify_async1(".", notify0, NULL);

		fclose(fopen("tmpfile", "w"));
		fclose(fopen("tmpfile2", "w"));
		sleep(1);
		sys_task_delete(task);
	}

	return suc;
}
static void notify0(struct sys_change_t *change, void *ptr)
{
	printf("here\n");
}
