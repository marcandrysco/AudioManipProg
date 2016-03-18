#ifndef DEFS_H
#define DEFS_H

/*
 * common headers
 */
#ifdef WINDOWS
#	define WIN32_LEAN_AND_MEAN
#	define _WIN32_WINNT 0x600
#	include <windows.h>
#	include <winsock2.h>
#else
#	include <netinet/in.h>
#	include <sys/socket.h>
#endif

#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <locale.h>
#include <math.h>
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

/*
 * structure prototypes
 */
struct sys_poll_t;

/*
 * fail macros
 */
#define fail(...) do { do { onexit } while(0); return mprintf(__VA_ARGS__); } while(0)
#define chkfail(ptr) do { char *err = ptr; if(err != NULL) { do { onexit } while(0); return err; } } while(0)
#define chkwarn(ptr) do { char *err = ptr; if(err != NULL) { fprintf(stderr, "%s\n", err); free(err); } } while(0)
#define chkexit(ptr) do { char *err = ptr; if(err != NULL) { fprintf(stderr, "%s\n", err); exit(1); } } while(0)
#define chkabort(ptr) do { char *err = ptr; if(err != NULL) { fprintf(stderr, "%s\n", err); abort(); } } while(0)

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
