#include "../common.h"


/*
 * local declarations
 */
static BOOL mutex_once(PINIT_ONCE InitOnce, PVOID arg, PVOID *ctx);

static DWORD task_proc(LPVOID arg);


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
 * Initialize a conditional variable.
 *   @flags: The flags.
 *   &returns: The conditional variable.
 */
sys_cond_t sys_cond_init(unsigned int flags)
{
	sys_cond_t cond;

	InitializeConditionVariable(&cond);

	return cond;
}

/**
 * Destroy a conditional variable.
 *   @cond: The conditional variable.
 */
void sys_cond_destroy(sys_cond_t *cond)
{
}


/**
 * Wait on a conditional variable.
 *   @cond: The conditional variable.
 *   @mutex: The associated mutex.
 */
void sys_cond_wait(sys_cond_t *cond, sys_mutex_t *mutex)
{
	SleepConditionVariableCS(cond, &mutex->lock, INFINITE);
}

/**
 * Signal a conditional variable.
 *   @cond: The conditional variable.
 */
void sys_cond_signal(sys_cond_t *cond)
{
	WakeConditionVariable(cond);
}

/**
 * Broadcast a conditional variable.
 *   @cond: The conditional variable.
 */
void sys_cond_broadcast(sys_cond_t *cond)
{
	WakeAllConditionVariable(cond);
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


/**
 * Task structure.
 *   @sync, thread: The synchronization and thread handles.
 *   @func: The task function.
 *   @arg: The argument.
 */
struct sys_task_t {
	HANDLE sync, thread;

	sys_task_f func;
	void *arg;
};


/**
 * Create a new task.
 *   @func: The task function.
 *   @arg: The argument.
 *   &returns: The task.
 */
struct sys_task_t *sys_task_new(sys_task_f func, void *arg)
{
	struct sys_task_t *task;

	task = malloc(sizeof(struct sys_task_t));
	task->func = func;
	task->arg = arg;
	task->sync = CreateEvent(NULL, TRUE, FALSE, NULL);
	task->thread = CreateThread(NULL, 0, task_proc, task, 0, NULL);

	return task;
}

/**
 * Delete a task.
 *   @task: The task.
 */
void sys_task_delete(struct sys_task_t *task)
{
	SetEvent(task->sync);
	WaitForSingleObject(task->thread, INFINITE);
	CloseHandle(task->sync);
	CloseHandle(task->thread);
	free(task);
}

/**
 * Process a task thread.
 *   @arg: The argument.
 *   &returns: Always zero.
 */
static DWORD task_proc(LPVOID arg)
{
	struct sys_task_t *task = arg;

	task->func(w32_fd(task->sync), task->arg);

	return 0;
}
