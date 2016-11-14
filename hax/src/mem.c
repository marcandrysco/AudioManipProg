#include "common.h"

/*
 * global variables
 */
ssize_t hax_memcnt = 0;

/*
 * local variables
 */
#if DEBUG
static sys_mutex_t lock;
#endif


/**
 * Allocate memory.
 *   @nbytes: The number of bytes.
 *   &returns: The allocated memory.
 */
void *hax_malloc(size_t nbytes)
{
#undef malloc
	void *ptr;

	ptr = _malloc(nbytes ?: 1);
	if(ptr == NULL)
		fatal("Memory allocation failed, %s.", strerror(errno));

#if DEBUG
	sys_mutex_lock(&lock);
	hax_memcnt++;
	sys_mutex_unlock(&lock);
#endif

	return ptr;
}

/**
 * Reallocate memory.
 *   @ptr: The original pointer.
 *   @nbytes: The number of bytes.
 *   &returns: The new pointer.
 */
void *hax_realloc(void *ptr, size_t nbytes)
{
	if(ptr == NULL)
		fatal("Cannot realloc null pointer.");

	return _realloc(ptr, nbytes ?: 1);
}

/**
 * Free memory.
 *   @ptr: The pointer.
 */
void hax_free(void *ptr)
{
#undef free
	if(ptr == NULL)
		fatal("Attempted to free null pointer,");

#if DEBUG
	sys_mutex_lock(&lock);
	hax_memcnt--;
	sys_mutex_unlock(&lock);
#endif

	free(ptr);
}

/**
 * Duplicate a string.
 *   @str: The string.
 *   &returns: The duplicated string.
 */
char *hax_strdup(const char *str)
{
#undef strdup
	char *dup;

	dup = hax_malloc(strlen(str) + 1);
	strcpy(dup, str);

	return dup;
}

/**
 * Duplicate a string at most 'n' characters.
 *   @str: The string.
 *   @n: The maximum length.
 *   &returns: The duplicated string.
 */
char *hax_strndup(const char *str, size_t n)
{
#undef strndup
	char *dup;
	size_t len = strlen(str);

	if(n < len)
		len = n;

	dup = hax_malloc(len + 1);
	memcpy(dup, str, len);
	dup[len] = '\0';

	return dup;
}


/**
 * Increment the resource usage.
 */
void hax_inc(void)
{
#if DEBUG
	sys_mutex_lock(&lock);
	hax_memcnt++;
	sys_mutex_unlock(&lock);
#endif
}

/**
 * Decrement the resource usage.
 */
void hax_dec(void)
{
#if DEBUG
	sys_mutex_lock(&lock);
	hax_memcnt--;
	sys_mutex_unlock(&lock);
#endif
}
