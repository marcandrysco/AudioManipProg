#include "common.h"


/*
 * local definitions
 */
#define prev(idx) (((idx) - 1 + AMP_PERF_LEN) % AMP_PERF_LEN)


/**
 * Initialize the performance structure.
 *   @perf: The performance structure.
 */
struct amp_perf_t amp_perf_init(void)
{
	unsigned int i;
	struct amp_perf_t perf;

	perf.idx = 0;

	for(i = 0; i < AMP_PERF_LEN; i++)
		perf.arr[i] = 0;

	return perf;
}

/**
 * Obtain the worst case over the last second.
 *   @perf: The performance structure.
 *   &returns: The worse is microseconds.
 */
uint64_t amp_perf_worst(struct amp_perf_t *perf)
{
	unsigned int i;
	uint64_t cur = sys_utime() - 1000000;

	i = perf->idx & ~0x1;

	for(i = prev(perf->idx & ~0x1); perf->arr[i] > cur; i = prev(perf->idx)) {
	}

	return 0;
}

/**
 * Obtain the average usage over the last second.
 *   @perf: The performance structure.
 *   &returns: The average.
 */
double amp_perf_ave(struct amp_perf_t *perf)
{
	uint64_t act = 0, tot = 0;
	unsigned int i, orig;
	uint64_t cur = sys_utime() - 1000000, tm[3];

	orig = i = prev(perf->idx & ~0x1);
	while(perf->arr[i] > cur) {
		tm[2] = perf->arr[i];
		i = prev(i);
		tm[1] = perf->arr[i];
		i = prev(i);
		tm[0] = perf->arr[i];

		act += tm[2] - tm[1];
		tot += tm[2] - tm[0];

		if(i == orig)
			break;
	}

	return (double)act / (double)tot;
}
