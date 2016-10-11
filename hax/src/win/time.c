#include "../common.h"


/**
 * Retrieve the time in seconds.
 *   &returns: The number of seconds since 1970.
 */
int64_t sys_time(void)
{
	return sys_utime() / 1000000;
}

/**
 * Retrieve the time in microseconds.
 *   &returns: The number of microseconds since 1970.
 */
int64_t sys_utime(void)
{
	FILETIME ft;
	GetSystemTimeAsFileTime(&ft);

	return (((int64_t)ft.dwHighDateTime << 32) + ft.dwLowDateTime) / 10;
}
