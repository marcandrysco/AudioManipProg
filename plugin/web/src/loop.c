#include "common.h"


/**
 * Looper structure.
 *   @serv: The server.
 *   @id: The identifier.
 */
struct web_loop_t {
	struct web_serv_t *serv;
	const char *id;
};


/*
 * local declarations
 */
static void loop_proc(struct io_file_t file, void *arg);


/**
 * Create a new looper.
 *   @serv: The server.
 *   @id: The indetifier.
 *   &returns: The looper.
 */
struct web_loop_t *web_loop_new(struct web_serv_t *serv, const char *id)
{
	struct web_loop_t *loop;

	loop = malloc(sizeof(struct web_loop_t));
	loop->serv = serv;
	loop->id = id;

	return loop;
}

/**
 * Delete a looper.
 *   @loop: The looper.
 */
void web_loop_delete(struct web_loop_t *loop)
{
	free(loop);
}


/**
 * Process information on a looper.
 *   @loop: The looper.
 *   @info: The information.
 */
void web_loop_info(struct web_loop_t *loop, struct amp_info_t info)
{
}

/**
 * Process data on a looper.
 *   @loop: The looper.
 *   @buf: The buffer.
 *   @time: The time.
 *   @len: The length.
 *   @queue: The action queue.
 *   &returns: The continuation flag.
 */
bool web_loop_proc(struct web_loop_t *loop, double *buf, struct amp_time_t *time, unsigned int len, struct amp_queue_t *queue)
{

	return false;
}


/**
 * Retrieve the information from a looper.
 *   @loop: The looper.
 *   @file: The file.
 */
void web_loop_print(struct web_loop_t *loop, struct io_file_t file)
{
	hprintf(file, "{}");
}

/**
 * Create a chunk for the looper.
 *   @loop: The looper.
 *   &returns: The chunk.
 */
struct io_chunk_t web_loop_chunk(struct web_loop_t *loop)
{
	return (struct io_chunk_t){ loop_proc, loop };
}
static void loop_proc(struct io_file_t file, void *arg)
{
	web_loop_print(arg, file);
}
