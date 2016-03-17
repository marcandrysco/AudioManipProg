#include "common.h"


/*
 * local declarations
 */
static void str_proc(struct io_file_t file, void *arg);

/**
 * Process a chunk on a file.
 *   @chunk: The chunk.
 *   @file: The file.
 */
void io_chunk_proc(struct io_chunk_t chunk, struct io_file_t file)
{
	chunk.func(file, chunk.arg);
}


/**
 * Create a chunk of a string.
 *   @str: The string.
 *   &returns: The chunk.
 */
struct io_chunk_t io_chunk_str(const char *str)
{
	return (struct io_chunk_t){ str_proc, (void *)str };
}
static void str_proc(struct io_file_t file, void *arg)
{
	io_file_write(file, arg, strlen(arg));
}
