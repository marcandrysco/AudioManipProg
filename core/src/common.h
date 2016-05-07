#ifndef COMMON_H
#define COMMON_H

/*
 * common headers
 */

#define _GNU_SOURCE
#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <locale.h>
#include <math.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <hax.h>
#include <muselang.h>
#include <libdsp.h>

#include "config.h"
#include "inc.h"


static inline char *amp_printf(const char *format, ...)
{
	char *err;
	va_list args;

	va_start(args, format);
	err = vmprintf(format, args);
	va_end(args);

	return err;
}

static inline void *amp_eprintf(char **err, const char *format, ...)
{
	va_list args;

	va_start(args, format);
	*err = vmprintf(format, args);
	va_end(args);

	return NULL;
}

#endif
