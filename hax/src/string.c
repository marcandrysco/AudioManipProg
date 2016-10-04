#include "common.h"


/**
 * Check if a string has a prefix.
 *   @str: The string.
 *   @pre: The prefix.
 *   &returns: The pointer to str after the prefix, or null if no prefix.
 */
char *strprefix(const char *str, const char *pre)
{
	while(*pre != '\0') {
		if(*pre++ != *str++)
			return NULL;
	}

	return (char *)str;
}
