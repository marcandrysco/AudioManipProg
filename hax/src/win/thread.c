#include "../common.h"


/**
 * Initialize a mutex.
 *   @flags: The flags.
 *   &returns: The mutex.
 */
sys_mutex_t sys_mutex_init(unsigned int flags)
{
	sys_mutex_t mutex;

	InitializeCriticalSection(&mutex.lock);

	return mutex;
}

/**
 * Destroy a mutex.
 *   @mutex: The mutex.
 */
void sys_mutex_destroy(sys_mutex_t *mutex)
{
	DeleteCriticalSection(&mutex->lock);
}


/**
 * Lock a mutex.
 *   @mutex: The mutex.
 */
void sys_mutex_lock(struct sys_mutex_t *mutex)
{
	EnterCriticalSection(&mutex->lock);
}

/**
 * Attempt to lock a mutex.
 *   @mutex: The mutex.
 *   &returns: True if locked, false otherwise.
 */
bool sys_mutex_trylock(struct sys_mutex_t *mutex)
{
	return TryEnterCriticalSection(&mutex->lock);
}

/**
 * Unlock a mutex.
 *   @mutex: The mutex.
 */
void sys_mutex_unlock(struct sys_mutex_t *mutex)
{
	LeaveCriticalSection(&mutex->lock);
}
