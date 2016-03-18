#include "common.h"


/**
 * Compare to integer pointer.
 *   @left: The left integer.
 *   @right: The right integer.
 *   &returns: Their order.
 */
int compare_int(const void *left, const void *right)
{
	int l = *(const int *)left, r = *(const int *)right;

	if(l > r)
		return 1;
	else if(l < r)
		return -1;
	else
		return 0;
}
