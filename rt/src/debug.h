#ifndef DEBUG_H
#define DEBUG_H

/**
 * Debugging file. By running a debug build, all resource allocation functions
 * are overridden in order to trake the number of resources in use. When built
 * with debug options, the final binary must contain the symbols 'DBG_memcnt'
 * and 'DBG_rescnt' in order link correctly.
 */

/* non-debug versions */

static inline void _free(void *ptr) { free(ptr); }

#if DEBUG

/* debug definitions */

extern int DBG_memcnt;
extern int DBG_rescnt;

static inline void *ml_malloc(size_t size) { DBG_memcnt++; return malloc(size); }
static inline void ml_free(void *ptr) { assert(ptr != NULL); DBG_memcnt--; free(ptr); }
static inline char *ml_strdup(const char *str) { assert(str != NULL); DBG_memcnt++; return strdup(str); }

/* common allocation overrides */

#undef malloc
#define malloc ml_malloc

#undef free
#define free ml_free

#undef strdup
#define strdup ml_strdup

#endif

#endif
