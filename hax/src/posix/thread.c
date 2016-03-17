#include "../common.h"


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
