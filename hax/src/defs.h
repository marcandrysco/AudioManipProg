#ifndef DEFS_H
#define DEFS_H

/*
 * common headers
 */
#if WINDOWS || CYGWIN
#	define WIN32_LEAN_AND_MEAN
#	define _WIN32_WINNT 0x600
#	include <windows.h>
#	include <winsock2.h>
#	include <ws2tcpip.h>
#	define abort() DebugBreak(), exit(1)
#else
#	include <pthread.h>
#	include <netinet/in.h>
#	include <sys/socket.h>
#	include <sys/time.h>
#endif

#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <locale.h>
#include <math.h>
#include <signal.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define noreturn __attribute__((noreturn))

typedef void (*delete_f)(void *ref);


/**
 * Variable argument list wrapper structure.
 *   @args: The internal argument list.
 */
struct arglist_t {
	va_list args;
};


/*
 * structure prototypes
 */
struct sys_poll_t;

/*
 * fail macros
 */
#define fail(...) do { char *_fail_err = hax_mprintf(__VA_ARGS__); do { onexit } while(0); return _fail_err; } while(0)
#define chkret(ptr) do { char *_fail_err = ptr; if(_fail_err != NULL) { return _fail_err; } } while(0)
#define chkfail(ptr) do { char *_fail_err = ptr; if(_fail_err != NULL) { do { onexit } while(0); return _fail_err; } } while(0)
#define chkexit(ptr) do { char *_fail_err = ptr; if(_fail_err != NULL) { hax_fprintf(stderr, "%s\n", _fail_err); exit(1); } } while(0)
#define chkabort(ptr) do { char *_fail_err = ptr; if(_fail_err != NULL) { hax_fprintf(stderr, "%s\n", _fail_err); abort(); } } while(0)

/*
 * function prototypes
 */
void hax_free(void *ptr);
void hax_fprintf(FILE *file, const char *restrict format, ...);

static inline bool chkbool(char *err)
{
	if(err != NULL)
		hax_free(err);

	return err == NULL;
}

static inline bool chkwarn(char *err)
{
	if(err != NULL) {
		hax_fprintf(stderr, "%s\n", err);

		hax_free(err);
	}

	return err == NULL;
}

/**
 * Create callback.
 *   &returns: The pointer.
 */
typedef void *(*new_f)(void);

/**
 * Copy callback.
 *   @ptr: The original pointer.
 *   &returns: The copied pointer.
 */
typedef void *(*copy_f)(void *ptr);

/**
 * Delete callback.
 *   @ptr: The pointer.
 */
typedef void (*delete_f)(void *ptr);

/**
 * Compare two refrences.
 *   @left: The left reference.
 *   @right: The right reference.
 *   &returns: Their order.
 */
typedef int (*compare_f)(const void *left, const void *right);

/**
 * Retrive the parent data structure from a member pointer.
 *   @ptr: The member poitner.
 *   @type: The parent type.
 *   @member: The member identifier.
 *   &returns: The parent pointer.
 */
#define getparent(ptr, type, member) ((type *)((void *)(ptr) - offsetof(type, member)))

#endif
