#include "common.h"


/**
 * Train structure.
 *   @serv: The server.
 *   @id: The identifier.
 */
struct web_train_t {
	struct web_serv_t *serv;
	const char *id;
};


/**
 * Create a new tainer.
 *   @serv: The server.
 *   @id: The indetifier.
 *   &returns: The tainer.
 */
struct web_train_t *web_train_new(struct web_serv_t *serv, const char *id)
{
	struct web_train_t *train;

	train = malloc(sizeof(struct web_train_t));
	train->serv = serv;
	train->id = id;

	return train;
}

/**
 * Delete a trainer.
 *   @train: The trainer.
 */
void web_train_delete(struct web_train_t *train)
{
	free(train);
}


/**
 * Process information on a trainer.
 *   @train: The trainer.
 *   @info: The information.
 */
void web_train_info(struct web_train_t *train, struct amp_info_t info)
{
}

/**
 * Process data on a trainer.
 *   @train: The trainer.
 *   @buf: The buffer.
 *   @time: The time.
 *   @len: The length.
 *   @queue: The action queue.
 *   &returns: The continuation flag.
 */
bool web_train_proc(struct web_train_t *train, double *buf, struct amp_time_t *time, unsigned int len, struct amp_queue_t *queue)
{

	return false;
}
