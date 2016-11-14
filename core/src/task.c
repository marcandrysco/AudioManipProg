#include "common.h"


/**
 * Asynchronous structure.
 *   @rd, wr, nbytes: The read, write, and number of bytes.
 *   @buf: The data buffer.
 */
struct amp_async_t {
	unsigned int rd, wr, nbytes;
	uint8_t buf[];
};


/**
 * Create an asynchronous structure.
 *   @nbytes: The number of bytes.
 *   &returns: The asynchronous structure.
 */
struct amp_async_t *amp_async_new(unsigned int nbytes)
{
	struct amp_async_t *async;

	async = malloc(nbytes + sizeof(struct amp_async_t));
	async->rd = 0;
	async->wr = 0;
	async->nbytes = nbytes;

	return async;
}

/**
 * Delete an asynchronous structure.
 *   @async: The asynchronous structure.
 */
void amp_async_delete(struct amp_async_t *async)
{
	free(async);
}


/**
 * Reset the asynchronous structure.
 *   @async: The asynchronous structure.
 */
void amp_async_reset(struct amp_async_t *async)
{
	async->rd = 0;
	async->wr = 0;
}

/**
 * Write an asynchronous message.
 *   @async: The asynchronous structure.
 *   @buf: The buffer.
 *   @nbyte: The number of bytes.
 */
void amp_async_write(struct amp_async_t *async, const void *buf, unsigned int nbytes)
{
	const uint8_t *ptr = buf;
	unsigned int wr, len = async->nbytes;

	wr = async->wr;

	while(nbytes--) {
		async->buf[wr] = *ptr++;
		wr = (wr + 1) % len;
	}

	__sync_synchronize();
	async->wr = wr;
}

/**
 * Try to read an asynchronous message.
 *   @async: The asynchronous structure.
 *   @buf: The buffer.
 *   @nbyte: The number of bytes.
 *   &returns: True if successful.
 */
bool amp_async_read(struct amp_async_t *async, void *buf, unsigned int nbytes)
{
	uint8_t *ptr = buf;
	unsigned int rd, len = async->nbytes;

	if(async->rd == async->wr)
		return false;

	rd = async->rd;

	while(nbytes--) {
		*ptr++ = async->buf[rd];
		rd = (rd + 1) % len;
	}

	__sync_synchronize();
	async->rd = rd;

	return true;
}


/**
 * Task structure.
 *   @inst: The instance list.
 */
struct amp_task_t {
	struct amp_task_inst_t *inst;
};

/**
 * Task instance structure.
 *   @next: The next instance.
 */
struct amp_task_inst_t {
	struct amp_task_inst_t *next;
};


/**
 * Create a new task.
 *   &returns: The task.
 */
struct amp_task_t *amp_task_new(void)
{
	struct amp_task_t *task;

	task = malloc(sizeof(struct amp_task_t));
	task->inst = NULL;

	return task;
}

/**
 * Delete a task.
 *   @task: The task.
 */
void amp_task_delete(struct amp_task_t *task)
{
	free(task);
}


struct amp_task_inst_t *amp_task_add(struct amp_task_t *task)
{
	struct amp_task_inst_t *inst;

	inst = malloc(sizeof(struct amp_task_inst_t));
	inst->next = task->inst;
	task->inst = inst;

	return inst;
}


void amp_task_in_write(struct amp_task_inst_t *inst, const void *buf, size_t len)
{
}

size_t amp_task_in_read(struct amp_task_inst_t *inst, void *buf)
{
	return false;
}


void amp_task_out_write(struct amp_task_inst_t *inst, const void *buf, size_t len)
{
}

size_t amp_task_out_read(struct amp_task_inst_t *inst, void *buf)
{
	return false;
}
