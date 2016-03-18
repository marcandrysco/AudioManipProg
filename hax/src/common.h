#ifndef COMMON_H
#define COMMON_H

/*
 * common headers
 */

#include "config.h"
#include "defs.h"

#ifdef WINDOWS
#	include "win/defs.h"
#	include "win/poll.h"
#	include "win/sock.h"
#	include "win/thread.h"
#else
#	include "posix/defs.h"
#	include "posix/poll.h"
#	include "posix/sock.h"
#	include "posix/thread.h"
#endif

#include "avltree.h"
#include "chunk.h"
#include "complex.h"
#include "file.h"
#include "list.h"
#include "mem.h"
#include "poll.h"
#include "print.h"
#include "strbuf.h"

#include "http.h"
#include "tcp.h"

#endif
