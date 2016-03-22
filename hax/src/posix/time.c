#include "../common.h"


/**
 * Retrieve the time in seconds.
 *   &returns: The number of seconds since 1970.
 */
int64_t sys_time(void)
{
	return time(NULL);
}

/**
 * Retrieve the time in microseconds.
 *   &returns: The number of microseconds since 1970.
 */
int64_t sys_utime(void)
{
	struct timeval tv;

	if(gettimeofday(&tv, NULL) < 0)
		fatal("Failed to get current time (%d). %s.", errno, strerror(errno));

	return 1000000 * (int64_t)tv.tv_sec + (int64_t)tv.tv_usec;
}


/**
 * Sleep an interval given in seocnds.
 *   @sec: The number of seconds to sleep.
 */
void sys_sleep(uint64_t sec)
{
	sleep(sec);
}

/**
 * Sleep an interval given in microseocnds.
 *   @usec: The number of microseconds to sleep.
 */
void sys_usleep(uint64_t usec)
{
	struct timespec rem, req = { usec / 1000000, (usec % 1000000) * 1000 };

	while(nanosleep(&req, &rem) < 0) {
		if(errno != EINTR)
			fatal("Failed to sleep (%d). %s.", errno, strerror(errno));

		req = rem;
	}
}


/**
 * Retrieve the current realtime clock time.
 *   &returns: The time in nanoseconds.
 */
int64_t sys_clock_realtime()
{
	struct timespec ts;

	if(clock_gettime(CLOCK_REALTIME, &ts) < 0)
		fatal("Failed to get current time (%d). %s.", errno, strerror(errno));

	return 1000000000 * (int64_t)ts.tv_sec + (int64_t)ts.tv_nsec;
}

/**
 * Retrieve the current monotonic clock time.
 *   &returns: The time in nanoseconds.
 */
int64_t sys_clock_monotonic()
{
	struct timespec ts;

	if(clock_gettime(CLOCK_MONOTONIC, &ts) < 0)
		fatal("Failed to get current time (%d). %s.", errno, strerror(errno));

	return 1000000000 * (int64_t)ts.tv_sec + (int64_t)ts.tv_nsec;
}
