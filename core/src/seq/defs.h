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

static inline void amp_queue_init(struct amp_queue_t *queue)
{
	queue->idx = 0;
}

static inline bool amp_queue_add(struct amp_queue_t *queue, struct amp_action_t action)
{
	if(queue->idx == AMP_QUEUE_LEN)
		return false;

	queue->arr[queue->idx++] = action;

	return true;
}



/**
 * Sequencer processing function.
 *   @ref: The reference.
 *   @queue: The queue.
 *   @time: The time.
 *   @len: The length.
 */

typedef void (*amp_seq_f)(void *ref, struct amp_queue_t *queue, struct amp_time_t *time, unsigned int len);

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
 *   @queue: The queue.
 *   @time: The time.
 *   @len: The length.
 */

static inline void amp_seq_proc(struct amp_seq_t seq, struct amp_queue_t *queue, struct amp_time_t *time, unsigned int len)
{
	seq.iface->proc(seq.ref, queue, time, len);
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
