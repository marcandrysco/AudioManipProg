#include "../common.h"


/**
 * Schedule structure.
 */

struct amp_repeat_t {
};


/**
 * Create a new repeat.
 *   &returns: The repeat.
 */

struct amp_repeat_t *amp_repeat_new(void)
{
	struct amp_repeat_t *repeat;

	repeat = malloc(sizeof(struct amp_repeat_t));

	return repeat;
}

/**
 * Copy a repeat.
 *   @repeat: The original repeat.
 *   &returns: The copied repeat.
 */

struct amp_repeat_t *amp_repeat_copy(struct amp_repeat_t *repeat)
{
	return amp_repeat_new();
}

/**
 * Delete a repeat.
 *   @repeat: The repeat.
 */

void amp_repeat_delete(struct amp_repeat_t *repeat)
{
	free(repeat);
}


/**
 * Process information on a repeat.
 *   @repeat: The repeat.
 *   @queue: The queue.
 *   @time: The time.
 *   @len: The length.
 */

void amp_repeat_info(struct amp_repeat_t *repeat, struct amp_info_t info)
{
}

/**
 * Process a repeat.
 *   @repeat: The repeat.
 *   @queue: The queue.
 *   @time: The time.
 *   @len: The length.
 */

void amp_repeat_proc(struct amp_repeat_t *repeat, struct amp_queue_t *queue, struct amp_time_t *time, unsigned int len)
{
}
