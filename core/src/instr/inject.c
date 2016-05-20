#include "../common.h"

/*
 * global variables
 */
const struct amp_instr_i amp_inject_iface = {
	(amp_info_f)amp_inject_info,
	(amp_instr_f)amp_inject_proc,
	(amp_copy_f)amp_inject_copy,
	(amp_delete_f)amp_inject_delete
};


/**
 * Create an inject instrument.
 *   @seq: Consumed. The sequencer.
 *   @instr: Consumed. The instrument.
 *   &returns: The inject.
 */
struct amp_inject_t *amp_inject_new(struct amp_seq_t seq, struct amp_instr_t instr)
{
	struct amp_inject_t *inject;

	inject = malloc(sizeof(struct amp_inject_t));
	inject->seq = seq;
	inject->instr = instr;

	return inject;
}

/**
 * Copy an inject instrument.
 *   @inject: The original inject.
 *   &returns: The copied inject.
 */
struct amp_inject_t *amp_inject_copy(struct amp_inject_t *inject)
{
	return amp_inject_new(amp_seq_copy(inject->seq), amp_instr_copy(inject->instr));
}

/**
 * Delete an inject instrument.
 *   @inject: The inject.
 */
void amp_inject_delete(struct amp_inject_t *inject)
{
	amp_seq_delete(inject->seq);
	amp_instr_delete(inject->instr);
	free(inject);
}


/**
 * Create an inject from a value.
 *   @ret: Ref. The returned value.
 *   @value: The value.
 *   @env: The environment.
 *   &returns: Error.
 */
char *amp_inject_make(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env)
{
#define onexit
	struct amp_seq_t seq;
	struct amp_instr_t instr;

	chkfail(amp_match_unpack(value, "(S,I)", &seq, &instr));
	*ret = amp_pack_instr((struct amp_instr_t){ amp_inject_new(seq, instr), &amp_inject_iface });

	return NULL;
#undef onexit
}


/**
 * Handle information on an inject.
 *   @inject: The inject.
 *   @info: The information.
 */
void amp_inject_info(struct amp_inject_t *inject, struct amp_info_t info)
{
	amp_seq_info(inject->seq, info);
	amp_instr_info(inject->instr, info);
}

/**
 * Process an inject.
 *   @inject: The inject.
 *   @buf: The buffer.
 *   @time: The time.
 *   @len: The length.
 *   @queue: The action queue.
 *   &returns: The continuation flag.
 */
bool amp_inject_proc(struct amp_inject_t *inject, double **buf, struct amp_time_t *time, unsigned int len, struct amp_queue_t *queue)
{
	struct amp_queue_t copy;

	amp_queue_copy(&copy, queue);
	amp_seq_proc(inject->seq, time, len, &copy);
	amp_instr_proc(inject->instr, buf, time, len, &copy);

	return false;
}
