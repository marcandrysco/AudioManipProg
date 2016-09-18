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
#include <signal.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <hax.h>
#include <libdsp.h>
#include <muselang.h>
#include <amplib.h>

#include "config.h"
#include "defs.h"
#include "inc.h"

#include "comm.h"
#include "engine.h"
#include "exec.h"
#include "watch.h"

#ifdef ALSA
#	include "alsa/audio.h"
#	include "alsa/midi.h"
#endif

#ifdef PULSE
#	include "pulse/audio.h"
#	include "pulse/conn.h"
#	include "pulse/hack.h"
#endif

#endif
