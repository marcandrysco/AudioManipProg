#include "../common.h"

/*
 * local declarations
 */
static void *task_proc(void *arg);


/**
 * Create a thread.
 *   @flags: The flags.
 *   @func: The function.
 *   @arg; The argument.
 *   &returns: The thread.
 */
sys_thread_t sys_thread_create(unsigned int flags, void *(*func)(void *), void *arg)
{
	int err;
	sys_thread_t thread;

	err = pthread_create(&thread.pthrd, NULL, func, arg);
	if(err != 0)
		fatal("Failed to create thread (%d). %s.", err, strerror(err));

	return thread;
}

/**
 * Join with a thread.
 *   @thread: The thread.
 *   &returns: The returned value.
 */
void *sys_thread_join(sys_thread_t *thread)
{
	int err;
	void *ptr;

	err = pthread_join(thread->pthrd, &ptr);
	if(err != 0)
		fatal("Failed to join thread (%d). %s.", err, strerror(err));

	return ptr;
}

/**
 * Detach a thread.
 *   @thread: The thread.
 */
void sys_thread_detach(sys_thread_t *thread)
{
	int err;

	err = pthread_detach(thread->pthrd);
	if(err != 0)
		fatal("Failed to detach thread (%d). %s.", err, strerror(err));
}


/**
 * Initialize a mutex.
 *   @flags: The flags.
 *   &returns: The mutex.
 */
struct sys_mutex_t sys_mutex_init(unsigned int flags)
{
	struct sys_mutex_t mutex;

	pthread_mutex_init(&mutex.pthrd, NULL);

	return mutex;
}

/**
 * Destroy a mutex.
 *   @mutex: The mutex.
 */
void sys_mutex_destroy(struct sys_mutex_t *mutex)
{
	pthread_mutex_destroy(&mutex->pthrd);
}


/**
 * Lock a mutex.
 *   @mutex: The mutex.
 */
void sys_mutex_lock(struct sys_mutex_t *mutex)
{
	pthread_mutex_lock(&mutex->pthrd);
}

/**
 * Attempt to lock a mutex.
 *   @mutex: The mutex.
 *   &returns: True if locked, false otherwise.
 */
bool sys_mutex_trylock(struct sys_mutex_t *mutex)
{
	int err;

	err = pthread_mutex_trylock(&mutex->pthrd);
	if(err == 0)
		return true;
	else if(err == EBUSY)
		return false;
	else
		fatal("Mutex trylock failed. %s.", strerror(err));
}

/**
 * Unlock a mutex.
 *   @mutex: The mutex.
 */
void sys_mutex_unlock(struct sys_mutex_t *mutex)
{
	pthread_mutex_unlock(&mutex->pthrd);
}


/**
 * Initialize a conditional variable.
 *   @flags: The flags.
 *   &returns: The conditional variable.
 */
sys_cond_t sys_cond_init(unsigned int flags)
{
	int err;
	struct sys_cond_t cond;

	err = pthread_cond_init(&cond.pthrd, NULL);
	if(err != 0)
		fatal("Failed to create condition variable (%d). %s.", err, strerror(errno));

	return cond;
}

/**
 * Destroy a conditional variable.
 *   @cond: The conditional variable.
 */
void sys_cond_destroy(sys_cond_t *cond)
{
	pthread_cond_destroy(&cond->pthrd);
}


/**
 * Wait on a conditional variable.
 *   @cond: The conditional variable.
 *   @mutex: The associated mutex.
 */
void sys_cond_wait(sys_cond_t *cond, sys_mutex_t *mutex)
{
	int err;

	err = pthread_cond_wait(&cond->pthrd, &mutex->pthrd);
	if(err != 0)
		fatal("Failed to wait on a condition variable (%d). %s.", err, strerror(errno));
}

/**
 * Signal a conditional variable.
 *   @cond: The conditional variable.
 */
void sys_cond_signal(sys_cond_t *cond)
{
	int err;

	err = pthread_cond_signal(&cond->pthrd);
	if(err != 0)
		fatal("Failed to signal a condition variable (%d). %s.", err, strerror(errno));
}

/**
 * Broadcast a conditional variable.
 *   @cond: The conditional variable.
 */
void sys_cond_broadcast(sys_cond_t *cond)
{
	int err;

	err = pthread_cond_broadcast(&cond->pthrd);
	if(err != 0)
		fatal("Failed to broadcast a condition variable (%d). %s.", err, strerror(errno));
}


/**
 * One-time initialization.
 *   @once: The once variable.
 *   @func: The initialization function.
 */
void sys_once(sys_once_t *once, void (*func)(void))
{
	int err;

	err = pthread_once(once, func);
	if(err != 0)
		fatal("Failed to to run one-time initialization (%d). %s.", err, strerror(errno));
}


/**
 * Task structure.
 *   @pipe: The sychronization pipe.
 *   @thread: The thread.
 *   @func: The callback function.
 *   @arg: The argument.
 */
struct sys_task_t {
	int pipe[2];
	sys_thread_t thread;

	sys_task_f func;
	void *arg;
};

/**
 * Create a task.
 *   @func: The function.
 *   @arg: The argument.
 */
struct sys_task_t *sys_task_new(sys_task_f func, void *arg)
{
	struct sys_task_t *task;

	task = malloc(sizeof(struct sys_task_t));
	task->func = func;
	task->arg = arg;
	if(pipe(task->pipe) < 0)
		fatal("Failed to create pipe (%d). %s.", errno, strerror(errno));

	task->thread = sys_thread_create(0, task_proc, task);

	return task;
}

/**
 * Delete a task, waiting to join the thread.
 *   @task: The task.
 */
void sys_task_delete(struct sys_task_t *task)
{
	ssize_t wr;
	uint8_t one = 1;

	do
		wr = write(task->pipe[1], &one, 1);
	while((wr == 0) || ((wr < 0) && (errno == EINTR)));

	if(wr < 0)
		fatal("Failed to synchronize task (%d). %s.", errno, strerror(errno));

	sys_thread_join(&task->thread);
	free(task);
}

/**
 * Process a task thread.
 *   @arg: The argument.
 *   &returns: Always null.
 */
static void *task_proc(void *arg)
{
	struct sys_task_t *task = arg;

	task->func(task->pipe[0], task->arg);

	return NULL;
}
