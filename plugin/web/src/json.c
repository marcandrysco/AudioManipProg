#include "common.h"


/*
 * local declarations
 */
static void pack_proc(struct io_file_t file, void *arg);


/**
 * Pack a location into a chunk.
 *   @loc: The location.
 *   &returns: the chunk.
 */
struct io_chunk_t web_pack_loc(struct amp_loc_t *loc)
{
	return (struct io_chunk_t){ pack_proc, loc };
}
static void pack_proc(struct io_file_t file, void *arg)
{
	struct amp_loc_t *loc = arg;

	hprintf(file, "{\"bar\":%d,\"beat\":%.8f}", loc->bar, loc->beat);
}
