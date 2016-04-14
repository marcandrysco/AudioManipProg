#ifndef SEQ_DEFS_H
#define SEQ_DEFS_H


/*
 * queue declarations
 */
#define AMP_QUEUE_LEN 64

/**
 * Queue structure.
 *   @idx: The index.
 */
struct amp_queue_t {
	unsigned int idx;
	struct amp_action_t arr[AMP_QUEUE_LEN];
};

/**
 * Initialize a queue.
 *   @queue: The queue.
 */
static inline void amp_queue_init(struct amp_queue_t *queue)
{
	queue->idx = 0;
}

/**
 * Copy a queue.
 *   @dest: The destination queue.
 *   @src: The source queue.
 */
static inline void amp_queue_copy(struct amp_queue_t *dest, const struct amp_queue_t *src)
{
	unsigned int i;

	dest->idx = src->idx;

	for(i = 0; i < src->idx; i++)
		dest->arr[i] = src->arr[i];
}

/**
 * Add an action to the queue.
 *   @queue: The queue.
 *   @action: The action.
 *   &returns: True if added, false otherwise.
 */
static inline bool amp_queue_add(struct amp_queue_t *queue, struct amp_action_t action)
{
	unsigned int i;

	if(queue->idx == AMP_QUEUE_LEN)
		return false;

	for(i = 0; i < queue->idx; i++) {
		if(action.delay < queue->arr[i].delay) {
			struct amp_action_t tmp;

			tmp = queue->arr[i];
			queue->arr[i] = action;
			action = tmp;
		}
	}

	queue->arr[queue->idx++] = action;

	action.queue = queue;

	return true;
}

/**
 * Remove an action from the queue.
 *   @queue: The queue.
 *   @idx: The index.
 */
static inline void amp_queue_remove(struct amp_queue_t *queue, unsigned int idx)
{
	for(idx++; idx < queue->idx; idx++)
		queue->arr[idx-1] = queue->arr[idx];

	queue->idx--;
}

/**
 * Retrieve the next action from the queue.
 *   @queue: The queue.
 *   @n: Ref. The position pointer. Incremented if successful.
 *   @idx: The current index.
 *   &returns: The action pointer or null.
 */
static inline struct amp_action_t *amp_queue_action(struct amp_queue_t *queue, unsigned int *n, unsigned int idx)
{
	struct amp_action_t *action;

	if(*n >= queue->idx)
		return NULL;

	action = &queue->arr[*n];
	if(action->delay > idx)
		return NULL;

	(*n)++;
	return action;
}

/**
 * Retrieve the next event from the queue.
 *   @queue: The queue.
 *   @n: Ref. The position pointer. Incremented if successful.
 *   @idx: The current index.
 *   &returns: The event pointer or null.
 */
static inline struct amp_event_t *amp_queue_event(struct amp_queue_t *queue, unsigned int *n, unsigned int idx)
{
	struct amp_action_t *action;

	action = amp_queue_action(queue, n, idx);
	return action ? &action->event : NULL;
}


/**
 * Sequencer processing function.
 *   @ref: The reference.
 *   @time: The time.
 *   @len: The length.
 *   @queue: The action queue.
 */
typedef void (*amp_seq_f)(void *ref, struct amp_time_t *time, unsigned int len, struct amp_queue_t *queue);

/**
 * Sequencer interface.
 *   @info: Information.
 *   @proc: Processing.
 *   @copy: Copy.
 *   @delete: Deletion.
 */
struct amp_seq_i {
	amp_info_f info;
	amp_seq_f proc;
	amp_copy_f copy;
	amp_delete_f delete;
};

/**
 * Sequencer structure.
 *   @ref: The reference.
 *   @iface: THe interface.
 */
struct amp_seq_t {
	void *ref;
	const struct amp_seq_i *iface;
};


/**
 * Process information on a sequencer.
 *   @seq: The sequencer.
 *   @info: The infoormation.
 */
static inline void amp_seq_info(struct amp_seq_t seq, struct amp_info_t info)
{
	seq.iface->info(seq.ref, info);
}

/**
 * Process a sequencer.
 *   @seq: The sequencer.
 *   @time: The time.
 *   @len: The length.
 *   @queue: The action queue.
 */
static inline void amp_seq_proc(struct amp_seq_t seq, struct amp_time_t *time, unsigned int len, struct amp_queue_t *queue)
{
	seq.iface->proc(seq.ref, time, len, queue);
}

/**
 * Copy a sequencer.
 *   @seq: The original sequencer.
 *   &returns: The copied sequencer.
 */
static inline struct amp_seq_t amp_seq_copy(struct amp_seq_t seq)
{
	return (struct amp_seq_t){ seq.iface->copy(seq.ref), seq.iface };
}

/**
 * Delete a sequencer.
 *   @seq: The sequencer.
 */
static inline void amp_seq_delete(struct amp_seq_t seq)
{
	seq.iface->delete(seq.ref);
}

/**
 * Delete a sequencer if not null.
 *   @seq: The sequencer.
 */
static inline void amp_seq_erase(struct amp_seq_t seq)
{
	if(seq.iface != NULL)
		amp_seq_delete(seq);
}

/**
 * Set a sequencer.
 *   @dest: The destination, deleted if not null.
 *   @src: Consumed. The source sequencer.
 */
static inline void amp_seq_set(struct amp_seq_t *dest, struct amp_seq_t src)
{
	amp_seq_erase(*dest);
	*dest = src;
}

#endif
