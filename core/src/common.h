#ifndef COMMON_H
#define COMMON_H

/*
 * common headers
 */

#define _GNU_SOURCE
#include <assert.h>
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

#include "muselang.h"
#include "libdsp.h"

#include "debug.h"
#include "defs.h"
#include "clk/defs.h"
#include "efx/defs.h"
#include "handler/defs.h"
#include "instr/defs.h"
#include "mod/defs.h"
#include "seq/defs.h"

#include "box.h"
#include "core.h"
#include "math.h"
#include "param.h"

#include "clk/basic.h"

#include "efx/chain.h"
#include "efx/clip.h"
#include "efx/comp.h"
#include "efx/effect.h"
#include "efx/filt.h"
#include "efx/gain.h"
#include "efx/gen.h"
#include "efx/synth.h"

#include "handler/handler.h"
#include "handler/midi.h"

#include "instr/mixer.h"
#include "instr/pan.h"
#include "instr/splice.h"

#include "mod/adsr.h"
#include "mod/mul.h"
#include "mod/osc.h"
#include "mod/patch.h"
#include "mod/sampler.h"
#include "mod/sum.h"
#include "mod/trig.h"

#include "seq/merge.h"
#include "seq/repeat.h"
#include "seq/sched.h"


static inline char *amp_printf(const char *format, ...)
{
	char *err;
	va_list args;

	va_start(args, format);
	if(vasprintf(&err, format, args) < 0)
		fprintf(stderr, "Allocation failure. %s.\n", strerror(errno)), abort();
	va_end(args);

#if DEBUG
	DBG_memcnt++;
#endif

	return err;
}

static inline void *amp_eprintf(char **err, const char *format, ...)
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
