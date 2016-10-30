#include "common.h"


/**
 * Controller structure.
 *   @serv: The server.
 *   @id: The identifier.
 */
struct web_ctrl_t {
	struct web_serv_t *serv;
	const char *id;
};


/*
 * local declarations
 */
static void ctrl_proc(struct io_file_t file, void *arg);


/**
 * Create a new ctrler.
 *   @serv: The server.
 *   @id: The indetifier.
 *   &returns: The ctrler.
 */
struct web_ctrl_t *web_ctrl_new(struct web_serv_t *serv, const char *id)
{
	struct web_ctrl_t *ctrl;

	ctrl = malloc(sizeof(struct web_ctrl_t));
	ctrl->serv = serv;
	ctrl->id = id;

	return ctrl;
}

/**
 * Delete a controller.
 *   @ctrl: The controller.
 */
void web_ctrl_delete(struct web_ctrl_t *ctrl)
{
	free(ctrl);
}


/**
 * Process information on a controller.
 *   @ctrl: The controller.
 *   @info: The information.
 */
void web_ctrl_info(struct web_ctrl_t *ctrl, struct amp_info_t info)
{
}

/**
 * Process data on a controller.
 *   @ctrl: The controller.
 *   @buf: The buffer.
 *   @time: The time.
 *   @len: The length.
 *   @queue: The action queue.
 *   &returns: The continuation flag.
 */
bool web_ctrl_proc(struct web_ctrl_t *ctrl, double *buf, struct amp_time_t *time, unsigned int len, struct amp_queue_t *queue)
{

	return false;
}


/**
 * Retrieve the information from a controller.
 *   @ctrl: The controller.
 *   @file: The file.
 */
void web_ctrl_print(struct web_ctrl_t *ctrl, struct io_file_t file)
{
	hprintf(file, "{}");
}

/**
 * Create a chunk for the controller.
 *   @ctrl: The controller.
 *   &returns: The chunk.
 */
struct io_chunk_t web_ctrl_chunk(struct web_ctrl_t *ctrl)
{
	return (struct io_chunk_t){ ctrl_proc, ctrl };
}
static void ctrl_proc(struct io_file_t file, void *arg)
{
	web_ctrl_print(arg, file);
}
