Threading
=========

The threading API provides a basic set of primitives that closely resemble
Pthreads. On Windows systems, the API provides a small interoperability layer
with the native threading APIs.


## Basic Threading

    sys_thread_t;
    sys_thread_t sys_thread_create(unsigned int flags, void *(*func)(void *), void *arg);
    void *sys_thread_join(sys_thread_t thread);
    void sys_thread_detach(sys_thread_t thread);

Threads are creating using the `sys_thread_create` API and freed using either
`sys_thread_join` or `sys_thread_deatch`. You must call one of these functions
in order to prevent leaking system resources.

The `sys_thread_create` function creates a thread, calling `func` using the
argument `arg`. The `flags` value should be set to zero -- future change may
use the flags variable.

The `sys_thread_join` function waits for the thread to terminate. The value
void pointer returned by the thread's function is returned through
`sys_thread_join`.

The `sys_thread_detach` function invalidates the reference from `thread` so
that resources used by `thread` are automatically freed when the thread
terminates.


## Mutex

    sys_mutex_t;
    sys_mutex_t sys_mutex_init(unsigned int flags);
    void sys_mutex_destroy(sys_mutex_t *mutex);
    #define PTHREAD_MUTEX_INIT

Mutex provide locking between threads, yielding to the scheduler as necessary.
The mutex is defined using the opaque type `sys_mutex_t` which may be
implemented as either a pointer or a structure.

The `sys_mutex_init` function initalizes the mutex. The `flags` value should
be set to zero -- future changes may use the flags variables.

The `sys_mutex_destroy` function destroys the mutex. All calls to
`sys_mutex_init` must be matched with a call to `sys_mutex_destroy` to prevent
leaking resources.

    void sys_mutex_lock(sys_mutex_t *mutex);
    bool sys_mutex_trylock(sys_mutex_t *mutex);
    void sys_mutex_unlock(sys_mutex_t *mutex);

The `sys_mutex_lock` function locks the mutex, waiting if the lock is already
held by another thread.

The `sys_mutex_trylock` function attempts to lock the mutex, returning
immediately if the lock is already held. The function returns `true` if the
lock is acquired, `false` if the attempt failed.

The `sys_mutex_unlock` function unlocks the mutex that was acquired by
`sys_mutex_lock` or a successful call to `sys_mutex_trylock`.


## Condition Variable

    sys_cond_t;

    sys_cond_t sys_cond_init(unsigned int flags);
    void sys_cond_destroy(sys_cond_t *cond);

The condition variable APIs provide an interface for using condition variables
as a primitive for multithreading. The `sys_cond_t` type is defined as an
opaque type that may be implemented as a native type of a strcture.

The `sys_cond_init` function initializes the condition variable.  The `flags`
value should be set to zero -- future changes may use the flags variables.

The `sys_cond_destroy` function destroys the conditoin variable. All calls to
`sys_cond_init` must be matched with a call to `sys_cond_destroy` to prevent
leaking resources.

    void sys_cond_wait(sys_cond_t *cond, sys_mutex_t *mutex);
    void sys_cond_signal(sys_cond_t *cond);
    void sys_cond_broadcast(sys_cond_t *cond);

The `sys_cond_wait` function waits for the condition variable `cond` to be
woken up. The `mutex` lock should be held before waiting, and the lock will be
held after the call returns.

The `sys_cond_signal` signals to wake up a single thread waiting on the
condition variable. The `sys_cond_broadcast` wakes up all thread currently
waiting on the condition variable.


## Thread Task

