#include "common.h"


/**
 * Compare two integer pointer.
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

/**
 * Compare two pointers.
 *   @left: The left integer.
 *   @right: The right integer.
 *   &returns: Their order.
 */
int compare_ptr(const void *left, const void *right)
{
	if(left > right)
		return 1;
	else if(left < right)
		return -1;
	else
		return 0;
}


/**
 * Deletion callback noop.
 *   @ptr: The pointer.
 */
void delete_noop(void *ptr)
{
}
