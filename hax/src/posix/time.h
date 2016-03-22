#ifndef POSIX_TIME_H
#define POSIX_TIME_H

/*
 * time function declarations
 */
int64_t sys_time(void);
int64_t sys_utime(void);

void sys_sleep(uint64_t sec);
void sys_usleep(uint64_t usec);

int64_t sys_clock_realtime();
int64_t sys_clock_monotonic();

#endif
