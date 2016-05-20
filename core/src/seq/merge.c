#include "../common.h"


/**
 * Merge structure.
 *   @head, tail: The head and tail instances.
 */
struct amp_merge_t {
	struct amp_merge_inst_t *head, *tail;
};

/**
 * Instance structure.
 *   @seq: The sequencer.
 *   @prev, next: The previous and next instances.
 */
struct amp_merge_inst_t {
	struct amp_seq_t seq;

	struct amp_merge_inst_t *prev, *next;
};


/*
 * global variables
 */
const struct amp_seq_i amp_merge_iface = {
	(amp_info_f)amp_merge_info,
	(amp_seq_f)amp_merge_proc,
	(amp_copy_f)amp_merge_copy,
	(amp_delete_f)amp_merge_delete
};


/**
 * Create a new merge.
 *   &returns: The merge.
 */
struct amp_merge_t *amp_merge_new(void)
{
	struct amp_merge_t *merge;

	merge = malloc(sizeof(struct amp_merge_t));
	merge->head = merge->tail = NULL;

	return merge;
}

/**
 * Copy a merge.
 *   @merge: The original merge.
 *   &returns: The copied merge.
 */
struct amp_merge_t *amp_merge_copy(struct amp_merge_t *merge)
{
	struct amp_merge_inst_t *inst;
	struct amp_merge_t *copy;
	
	copy = amp_merge_new();

	for(inst = merge->head; inst != NULL; inst = inst->next)
		amp_merge_append(copy, amp_seq_copy(inst->seq));

	return copy;
}

/**
 * Delete a merge.
 *   @merge: The merge.
 */
void amp_merge_delete(struct amp_merge_t *merge)
{
	struct amp_merge_inst_t *cur, *next;

	for(cur = merge->head; cur != NULL; cur = next) {
		next = cur->next;

		amp_seq_delete(cur->seq);
		free(cur);
	}

	free(merge);
}


/**
 * Create a merge from a value.
 *   @ret: Ref. The returned value.
 *   @value: The value.
 *   @env: The environment.
 *   &returns: Error.
 */
char *amp_merge_make(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env)
{
#define onexit amp_merge_delete(merge);
	struct amp_merge_t *merge;
	struct ml_link_t *link;

	merge = amp_merge_new();

	if(value->type != ml_value_list_v)
		fail("Type error. Toggle expects type '[Seq]'.");

	for(link = value->data.list->head; link != NULL; link = link->next) {
		struct amp_seq_t seq;

		chkfail(amp_match_unpack(ml_value_copy(link->value), "S", &seq));
		amp_merge_append(merge, seq);
	}

	*ret = amp_pack_seq((struct amp_seq_t){ merge, &amp_merge_iface });

	return NULL;
#undef onexit
}


/**
 * Prepend an sequencer to a merge.
 *   @merge: The merge.
 *   @seq: Cosnumed. The sequencer.
 */
void amp_merge_prepend(struct amp_merge_t *merge, struct amp_seq_t seq)
{
	struct amp_merge_inst_t *inst;

	inst = malloc(sizeof(struct amp_merge_inst_t));
	inst->seq = seq;
	inst->next = merge->head;
	inst->prev = NULL;
	*(merge->head ? &merge->head->prev : &merge->tail) = inst;
	merge->head = inst;
}

/**
 * Append an sequencer to a merge.
 *   @merge: The merge.
 *   @seq: Cosnumed. The sequencer.
 */
void amp_merge_append(struct amp_merge_t *merge, struct amp_seq_t seq)
{
	struct amp_merge_inst_t *inst;

	inst = malloc(sizeof(struct amp_merge_inst_t));
	inst->seq = seq;
	inst->prev = merge->tail;
	inst->next = NULL;
	*(merge->tail ? &merge->tail->next : &merge->head) = inst;
	merge->tail = inst;
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
	struct amp_merge_inst_t *inst;

	for(inst = merge->head; inst != NULL; inst = inst->next)
		amp_seq_info(inst->seq, info);
}

/**
 * Process a merge.
 *   @merge: The merge.
 *   @time: The time.
 *   @len: The length.
 *   @queue: The action queue.
 */
void amp_merge_proc(struct amp_merge_t *merge, struct amp_time_t *time, unsigned int len, struct amp_queue_t *queue)
{
	struct amp_merge_inst_t *inst;

	for(inst = merge->head; inst != NULL; inst = inst->next)
		amp_seq_proc(inst->seq, time, len, queue);
}
