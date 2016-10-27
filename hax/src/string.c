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



/**
 * Convert a hex character into a digit.
 *   @ch: The character.
 *   &returns: The hex value, or negative if invalid character.
 */
int ch_hex2int(char ch)
{
	if((ch >= '0') && (ch <= '9'))
		return ch - '0';
	else if((ch >= 'A') && (ch <= 'F'))
		return ch - 'A' + 10;
	else if((ch >= 'a') && (ch <= 'f'))
		return ch - 'a' + 10;
	else
		return -1;
}
