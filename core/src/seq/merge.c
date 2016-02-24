#include "../common.h"


/**
 * Merge structure.
 *   @head, tail: The head and tail instances.
 */

struct amp_merge_t {
	struct inst_t *head, *tail;
};

/**
 * Instance structure.
 *   @seq: The sequencer.
 *   @prev, next: The previous and next instances.
 */

struct inst_t {
	struct amp_seq_t seq;

	struct inst_t *prev, *next;
};


/**
 * Create a new merge.
 *   &returns: The merge.
 */

struct amp_merge_t *amp_merge_new(void)
{
	struct amp_merge_t *merge;

	merge = malloc(sizeof(struct amp_merge_t));

	return merge;
}

/**
 * Copy a merge.
 *   @merge: The original merge.
 *   &returns: The copied merge.
 */

struct amp_merge_t *amp_merge_copy(struct amp_merge_t *merge)
{
	return amp_merge_new();
}

/**
 * Delete a merge.
 *   @merge: The merge.
 */

void amp_merge_delete(struct amp_merge_t *merge)
{
	free(merge);
}


/**
 * Process information on a merge.
 *   @merge: The merge.
 *   @queue: The queue.
 *   @time: The time.
 *   @len: The length.
 */

void amp_merge_info(struct amp_merge_t *merge, struct amp_info_t info)
{
}

/**
 * Process a merge.
 *   @merge: The merge.
 *   @queue: The queue.
 *   @time: The time.
 *   @len: The length.
 */

void amp_merge_proc(struct amp_merge_t *merge, struct amp_queue_t *queue, struct amp_time_t *time, unsigned int len)
{
}
