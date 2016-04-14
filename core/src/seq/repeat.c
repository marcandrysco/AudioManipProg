#include "../common.h"


/**
 * Schedule structure.
 *   @off: The offset.
 *   @len: The repeat length.
 *   @seq: The child sequencer.
 */
struct amp_repeat_t {
	int off;
	unsigned int len;
	struct amp_seq_t seq;
};


/*
 * global variables
 */
const struct amp_seq_i amp_repeat_iface = {
	(amp_info_f)amp_repeat_info,
	(amp_seq_f)amp_repeat_proc,
	(amp_copy_f)amp_repeat_copy,
	(amp_delete_f)amp_repeat_delete
};


/**
 * Create a new repeat.
 *   @off: The offset.
 *   @len: The repeat length.
 *   @seq: Consumed. The child sequencer.
 *   &returns: The repeat.
 */
struct amp_repeat_t *amp_repeat_new(int off, int len, struct amp_seq_t seq)
{
	struct amp_repeat_t *repeat;

	repeat = malloc(sizeof(struct amp_repeat_t));
	repeat->off = off;
	repeat->len = len;
	repeat->seq = seq;

	return repeat;
}

/**
 * Copy a repeat.
 *   @repeat: The original repeat.
 *   &returns: The copied repeat.
 */
struct amp_repeat_t *amp_repeat_copy(struct amp_repeat_t *repeat)
{
	return amp_repeat_new(repeat->off, repeat->len, amp_seq_copy(repeat->seq));
}

/**
 * Delete a repeat.
 *   @repeat: The repeat.
 */
void amp_repeat_delete(struct amp_repeat_t *repeat)
{
	amp_seq_delete(repeat->seq);
	free(repeat);
}


/**
 * Create a repeat from a value.
 *   @value: The value.
 *   @env: The environment.
 *   @err: The error.
 *   &returns: The value or null.
 */
struct ml_value_t *amp_repeat_make(struct ml_value_t *value, struct ml_env_t *env, char **err)
{
#undef fail
#define fail(format, ...) do { amp_seq_delete(seq); *err = amp_printf(format, ##__VA_ARGS__); NULL; } while(0)
	int off, len;
	struct amp_seq_t seq;

	*err = amp_match_unpack(value, "(d,d,S)", &off, &len, &seq);
	if(*err != NULL)
		return NULL;

	if(len <= 0)
		fail("Repeaat length must be positive.");

	return amp_pack_seq((struct amp_seq_t){ amp_repeat_new(off, len, seq), &amp_repeat_iface });
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
	amp_seq_info(repeat->seq, info);
}

/**
 * Process a repeat.
 *   @repeat: The repeat.
 *   @time: The time.
 *   @len: The length.
 *   @queue: The action queue.
 */
void amp_repeat_proc(struct amp_repeat_t *repeat, struct amp_time_t *time, unsigned int len, struct amp_queue_t *queue)
{
	unsigned int i;
	struct amp_time_t tmp[len];

	for(i = 0; i <= len; i++)
		tmp[i] = amp_time_repeat(time[i], repeat->off, repeat->len);

	amp_seq_proc(repeat->seq, tmp, len, queue);
}
