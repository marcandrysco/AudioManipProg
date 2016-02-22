#ifndef COMMON_H
#define COMMON_H

/*
 * common headers
 */

#define _GNU_SOURCE
#include <ctype.h>
#include <errno.h>
#include <locale.h>
#include <math.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "debug.h"
#include "defs.h"
#include "env.h"
#include "eval.h"
#include "expr.h"
#include "parse.h"
#include "token.h"
#include "value.h"


static inline void *ml_eprintf(char **err, const char *format, ...)
{
	va_list args;

	va_start(args, format);
	
	if(vasprintf(err, format, args) < 0)
		fprintf(stderr, "Allocation failure. %s.\n", strerror(errno)), abort();
	va_end(args);

#if DEBUG
	DBG_memcnt++;
#endif
	return NULL;
}

#endif
