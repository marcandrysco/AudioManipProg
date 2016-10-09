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
#include <amprt.h>

#include "inc.h"
#include "defs.h"


/**
 * Boolean check macro. Immediately returns false if value is false.
 *   @val: The value to check.
 *   &action: Returns false from current function if value is false.
 */
#define chk(val) do { if(!val) return false; } while(0)

/**
 * Error check macro. Immediately return false if error is present.
 *   @err: The error to check.
 *   &actions: Returns false if error is non-null.
 */
#define chkerr(err) do { if(err != NULL) { fprintf(stderr, "%s\n", err); return false; } } while(0)

#endif
