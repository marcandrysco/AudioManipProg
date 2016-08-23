#include "../common.h"


/*
 * local declarations
 */
static BOOL mutex_once(PINIT_ONCE InitOnce, PVOID arg, PVOID *ctx);

/**
 * Initialize a mutex.
 *   @flags: The flags.
 *   &returns: The mutex.
 */
sys_mutex_t sys_mutex_init(unsigned int flags)
{
	sys_mutex_t mutex;

	InitOnceInitialize(&mutex.once);
	InitOnceExecuteOnce(&mutex.once, mutex_once, &mutex.lock, NULL);

	return mutex;
}

/**
 * Destroy a mutex.
 *   @mutex: The mutex.
 */
void sys_mutex_destroy(sys_mutex_t *mutex)
{
	InitOnceExecuteOnce(&mutex->once, mutex_once, &mutex->lock, NULL);
	DeleteCriticalSection(&mutex->lock);
}


/**
 * Lock a mutex.
 *   @mutex: The mutex.
 */
void sys_mutex_lock(struct sys_mutex_t *mutex)
{
	InitOnceExecuteOnce(&mutex->once, mutex_once, &mutex->lock, NULL);
	EnterCriticalSection(&mutex->lock);
}

/**
 * Attempt to lock a mutex.
 *   @mutex: The mutex.
 *   &returns: True if locked, false otherwise.
 */
bool sys_mutex_trylock(struct sys_mutex_t *mutex)
{
	InitOnceExecuteOnce(&mutex->once, mutex_once, &mutex->lock, NULL);

	return TryEnterCriticalSection(&mutex->lock);
}

/**
 * Unlock a mutex.
 *   @mutex: The mutex.
 */
void sys_mutex_unlock(struct sys_mutex_t *mutex)
{
	InitOnceExecuteOnce(&mutex->once, mutex_once, &mutex->lock, NULL);

	LeaveCriticalSection(&mutex->lock);
}


/**
 * Once callback for mutexes.
 *   @once: The init once object.
 *   @arg: The argument (critical section).
 *   @ctx: Unused. Context.
 *   &returns: Always true.
 */
static BOOL mutex_once(PINIT_ONCE once, PVOID arg, PVOID *ctx)
{
	InitializeCriticalSection(arg);
	return TRUE;
}


struct sys_task_t {
};

struct sys_task_t *sys_task_new(void *p1,void *p2)
{
	return NULL;
}

void sys_task_delete(struct sys_task_t *task)
{
}
