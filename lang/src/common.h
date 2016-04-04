#ifndef COMMON_H
#define COMMON_H

/*
 * common headers
 */

#include <hax.h>

#include "debug.h"
#include "defs.h"

#include "env.h"
#include "eval.h"
#include "expr.h"
#include "match.h"
#include "parse.h"
#include "print.h"
#include "token.h"
#include "value.h"

#include "eval/math.h"


static inline void *ml_eprintf(char **err, const char *format, ...)
{
	va_list args;

	va_start(args, format);
	*err = vmprintf(format, args);
	va_end(args);

	return NULL;
}

#endif
