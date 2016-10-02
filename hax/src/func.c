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
 * Compare two 16-bit unsigned integer pointer.
 *   @left: The left integer.
 *   @right: The right integer.
 *   &returns: Their order.
 */
int compare_uint16(const void *left, const void *right)
{
	uint16_t l = *(const uint16_t *)left, r = *(const uint16_t *)right;

	if(l > r)
		return 1;
	else if(l < r)
		return -1;
	else
		return 0;
}

/**
 * Compare two characters.
 *   @left: The left characters.
 *   @right: The right characters.
 *   &returns: Their order.
 */
int compare_char(const void *left, const void *right)
{
	return *(const char *)left - *(const char *)right;
}

/**
 * Compare two strings.
 *   @left: The left string.
 *   @right: The right string.
 *   &returns: Their order.
 */
int compare_str(const void *left, const void *right)
{
	return strcmp(left, right);
}

/**
 * Compare two pointers.
 *   @left: The left pointer.
 *   @right: The right pointer.
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
