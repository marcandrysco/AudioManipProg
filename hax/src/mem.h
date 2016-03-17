#ifndef MEM_H
#define MEM_H

/*
 * memory declarations
 */

extern ssize_t hax_memcnt;

void *hax_malloc(size_t nbytes);
void hax_free(void *ptr);
char *hax_strdup(const char *str);

static inline void erase(void *ptr)
{
	if(ptr != NULL)
		hax_free(ptr);
}

static inline void hax_strset(char **dest, char *src)
#define strset hax_strset
{
	erase(*dest);
	*dest = src;
}

/*
 * fallback definitions
 */

static inline void *_malloc(size_t nbytes) { return malloc(nbytes); }
static inline void _free(void *ptr) { free(ptr); }

/*
 * shortened macros
 */

#undef malloc
#undef free
#undef strdup
#define malloc hax_malloc
#define free hax_free
#define strdup hax_strdup

#endif
