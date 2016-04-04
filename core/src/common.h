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

#include "debug.h"
#include "defs.h"
#include "clk/defs.h"
#include "efx/defs.h"
#include "instr/defs.h"
#include "mod/defs.h"
#include "seq/defs.h"

#include "box.h"
#include "cache.h"
#include "core.h"
#include "ctrl.h"
#include "key.h"
#include "io.h"
#include "math.h"
#include "midi.h"
#include "param.h"

#include "clk/basic.h"

#include "efx/bitcrush.h"
#include "efx/chain.h"
#include "efx/chorus.h"
#include "efx/clip.h"
#include "efx/comp.h"
#include "efx/effect.h"
#include "efx/expcrush.h"
#include "efx/filt.h"
#include "efx/gain.h"
#include "efx/gen.h"
#include "efx/mix.h"
#include "efx/reverb.h"
#include "efx/scale.h"
#include "efx/sect.h"
#include "efx/track.h"

#include "eval/sched.h"

#include "instr/mixer.h"
#include "instr/pan.h"
#include "instr/series.h"
#include "instr/single.h"
#include "instr/splice.h"

#include "mod/adsr.h"
#include "mod/mul.h"
#include "mod/noise.h"
#include "mod/osc.h"
#include "mod/patch.h"
#include "mod/sample.h"
#include "mod/shot.h"
#include "mod/sum.h"
#include "mod/synth.h"
#include "mod/trig.h"

#include "seq/merge.h"
#include "seq/player.h"
#include "seq/repeat.h"
#include "seq/sched.h"


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
