#include "../common.h"


/**
 * Enable structure.
 *   @on: The on flag.
 *   @id: The identifier.
 *   @seq: The sequencer.
 */
struct amp_enable_t {
	bool on;
	struct amp_id_t id;
	struct amp_seq_t seq;
};


/*
 * global variables
 */
const struct amp_seq_i amp_enable_iface = {
	(amp_info_f)amp_enable_info,
	(amp_seq_f)amp_enable_proc,
	(amp_copy_f)amp_enable_copy,
	(amp_delete_f)amp_enable_delete
};


/**
 * Create a new enable.
 *   @id: The identifier.
 *   @seq: The sequencer.
 *   &returns: The enable.
 */
struct amp_enable_t *amp_enable_new(struct amp_id_t id, struct amp_seq_t seq)
{
	struct amp_enable_t *enable;

	enable = malloc(sizeof(struct amp_enable_t));
	enable->on = false;
	enable->id = id;
	enable->seq = seq;

	return enable;
}

/**
 * Copy an enable.
 *   @enable: The original enable.
 *   &returns: The copied enable.
 */
struct amp_enable_t *amp_enable_copy(struct amp_enable_t *enable)
{
	return amp_enable_new(enable->id, amp_seq_copy(enable->seq));
}

/**
 * Delete an enable.
 *   @enable: The enable.
 */
void amp_enable_delete(struct amp_enable_t *enable)
{
	amp_seq_delete(enable->seq);
	free(enable);
}


/**
 * Create an enable from a value.
 *   @ret: Ref. The returned value.
 *   @value: The value.
 *   @env: The environment.
 *   &returns: Error.
 */
char *amp_enable_make(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env)
{
#define onexit
	int dev, key;
	struct amp_seq_t seq;

	chkfail(amp_match_unpack(value, "((d,d),S)", &dev, &key, &seq));
	*ret = amp_pack_seq((struct amp_seq_t){ amp_enable_new((struct amp_id_t){ dev, key }, seq), &amp_enable_iface });

	return NULL;
#undef onexit
}


/**
 * Process information on an enable.
 *   @enable: The enable.
 *   @queue: The queue.
 *   @time: The time.
 *   @len: The length.
 */
void amp_enable_info(struct amp_enable_t *enable, struct amp_info_t info)
{
	amp_seq_info(enable->seq, info);
}

/**
 * Process an enable.
 *   @enable: The enable.
 *   @time: The time.
 *   @len: The length.
 *   @queue: The action queue.
 */
void amp_enable_proc(struct amp_enable_t *enable, struct amp_time_t *time, unsigned int len, struct amp_queue_t *queue)
{
	bool on;
	unsigned int i, n = 0, idx = 0;
	struct amp_event_t *event;

	for(i = 0; i < len; i++) {
		while((event = amp_queue_event(queue, &n, i)) != NULL) {
			if((event->dev != enable->id.dev) || (event->key != enable->id.key))
				continue;

			on = event->val > 0;
			if(enable->on == on)
				continue;

			if(enable->on)
				;//amp_seq_proc(enable->seq, time + idx, len - idx, queue);

			idx = i;
			enable->on = on;
		}
	}

	if(enable->on)
		amp_seq_proc(enable->seq, time + idx, len - idx, queue);
}
