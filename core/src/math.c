#include "common.h"


/**
 * Compare two times.
 *   @left: The left time.
 *   @right: The right time.
 *   &returns: Their order.
 */
int amp_time_compare(const void *left, const void *right)
{
	return amp_time_cmp(*(struct amp_time_t *)left, *(struct amp_time_t *)right);
}
