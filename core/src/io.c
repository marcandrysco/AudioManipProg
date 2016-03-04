#include "common.h"


/**
 * I/O manager structure.
 */

struct amp_io_t {
};


/**
 * Create a new I/O manager.
 *   &returns: The I/O manager.
 */

struct amp_io_t *amp_io_new(void)
{
	struct amp_io_t *io;

	io = malloc(sizeof(struct amp_io_t));

	return io;
}

/**
 * Delete an I/O manager.
 *   @io: The I/O manager.
 */

void amp_io_delete(struct amp_io_t *io)
{
	free(io);
}
