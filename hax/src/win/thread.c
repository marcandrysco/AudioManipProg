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


static DWORD thread_proc(LPVOID param)
{
	bool detach;
	sys_thread_t thread = param;

	thread->ret = thread->func(thread->arg);

	EnterCriticalSection(&thread->lock);
	detach = thread->detach;
	thread->detach = true;
	LeaveCriticalSection(&thread->lock);

	if(detach) {
		DeleteCriticalSection(&thread->lock);
		free(thread);
	}

	return 0;
}

/**
 * Create a thread.
 *   @flags: The flags.
 *   @func: The callback function.
 *   @arg: The arguments.
 */
sys_thread_t sys_thread_create(unsigned int flags, void *(*func)(void *), void *arg)
{
	sys_thread_t thread;

	thread = malloc(sizeof(struct sys_thread_t));
	thread->func = func;
	thread->arg = arg;
	thread->detach = false;
	InitializeCriticalSection(&thread->lock);

	thread->handle = CreateThread(NULL, 0, thread_proc, thread, 0, NULL);
	if(thread->handle == NULL)
		fatal("Failed to create thread.");

	return thread;
}

/**
 * Join a thread.
 *   @thread: The thread.
 *   &returns: The returned value.
 */
void *sys_thread_join(sys_thread_t *thread)
{
	void *ret;

	WaitForSingleObject((*thread)->handle, INFINITE);
	ret = (*thread)->ret;

	DeleteCriticalSection(&(*thread)->lock);
	CloseHandle((*thread)->handle);
	free((*thread));

	return ret;
}

/**
 * Detach from a thread.
 *   @thread: The thread.
 */
void sys_thread_detach(sys_thread_t *thread)
{
	bool detach;

	EnterCriticalSection(&(*thread)->lock);
	detach = (*thread)->detach;
	(*thread)->detach = true;
	LeaveCriticalSection(&(*thread)->lock);

	if(detach) {
		DeleteCriticalSection(&(*thread)->lock);
		free((*thread));
	}

	CloseHandle((*thread)->handle);
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
