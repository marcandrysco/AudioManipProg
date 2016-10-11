#ifndef COMMON_H
#define COMMON_H

/*
 * common headers
 */
#include "config.h"

#include <hax.h>
#include <libdsp.h>
#include <muselang.h>
#include <amplib.h>

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
