#ifndef MEM_H
#define MEM_H

/*
 * memory declarations
 */

extern ssize_t hax_memcnt;

void *hax_malloc(size_t nbytes);
void *hax_realloc(void *ptr, size_t nbytes);
void hax_free(void *ptr);
char *hax_strdup(const char *str);
char *hax_strndup(const char *str, size_t n);

void hax_inc(void);
void hax_dec(void);

/*
 * fallback definitions
 */
static inline void *_malloc(size_t nbytes) { return malloc(nbytes); }
static inline void *_realloc(void *ptr, size_t nbytes) { return realloc(ptr, nbytes); }
static inline void _free(void *ptr) { free(ptr); }

/*
 * shortened macros
 */

#undef malloc
#undef realloc
#undef free
#undef strdup
#undef strndup
#define malloc hax_malloc
#define realloc hax_realloc
#define free hax_free
#define strdup hax_strdup
#define strndup hax_strndup


/**
 * Erase memory if non-null.
 *   @ptr: The pointer.
 */
static inline void erase(void *ptr)
{
	if(ptr != NULL)
		free(ptr);
}

static inline void hax_strset(char **dest, char *src)
#define strset hax_strset
{
	erase(*dest);
	*dest = src;
}


#endif
