#ifndef COMMON_H
#define COMMON_H

/*
 * common includes
 */
#include "config.h"

#include "../../hax.h"

#define chk(b, n) (b ? 0 : fprintf(stderr, "Failed test '%s'.\n", n), b)

#endif
