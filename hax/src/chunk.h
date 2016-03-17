#ifndef CHUNK_H
#define CHUNK_H

/*
 * structure prototypes
 */
struct io_file_t;

/**
 * Chunk processing function.
 *   @file: The output file.
 *   @arg: The argument.
 */
typedef void (*io_chunk_f)(struct io_file_t file, void *arg);

/**
 * Chunk structure.
 *   @func: The processing function.
 *   @arg: The argument.
 */
struct io_chunk_t {
	io_chunk_f func;
	void *arg;
};

/*
 * chunk declarations
 */

void io_chunk_proc(struct io_chunk_t chunk, struct io_file_t file);

struct io_chunk_t io_chunk_str(const char *str);

#endif
