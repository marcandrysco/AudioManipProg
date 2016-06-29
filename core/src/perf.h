#ifndef PERF_H
#define PERF_H


/*
 * performance definitions
 */
#define AMP_PERF_LEN (8*1024)

/**
 * Performance structure.
 *   @idx: The current index.
 *   @arr: The time array.
 */
struct amp_perf_t {
	unsigned int idx;
	uint64_t arr[AMP_PERF_LEN];
};


/*
 * performance declarations
 */
struct amp_perf_t amp_perf_init(void);

double amp_perf_ave(struct amp_perf_t *perf);


/**
 * Mark a time.
 *   @perf: The performance structure.
 */
static inline void amp_perf_mark(struct amp_perf_t *perf)
{
	perf->arr[perf->idx] = sys_utime();
	perf->idx = (perf->idx + 1) % AMP_PERF_LEN;
}

#endif
