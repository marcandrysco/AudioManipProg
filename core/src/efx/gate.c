#include "../common.h"

/*
 * global variables
 */
const struct amp_effect_i amp_gate_iface = {
	(amp_info_f)amp_gate_info,
	(amp_effect_f)amp_gate_proc,
	(amp_copy_f)amp_gate_copy,
	(amp_delete_f)amp_gate_delete
};


/**
 * Create a gate effect.
 *   @left: Consumed. The left effect.
 *   @right: Consumed. The right effect.
 *   &returns: The gate.
 */
struct amp_gate_t *amp_gate_new(struct amp_effect_t left, struct amp_effect_t right)
{
	struct amp_gate_t *gate;

	gate = malloc(sizeof(struct amp_gate_t));
	gate->left = left;
	gate->right = right;

	return gate;
}

/**
 * Copy a gate effect.
 *   @gate: The original gate.
 *   &returns: The copied gate.
 */
struct amp_gate_t *amp_gate_copy(struct amp_gate_t *gate)
{
	return amp_gate_new(amp_effect_copy(gate->left), amp_effect_copy(gate->right));
}

/**
 * Delete a gate effect.
 *   @gate: The gate.
 */
void amp_gate_delete(struct amp_gate_t *gate)
{
	amp_effect_delete(gate->left);
	amp_effect_delete(gate->right);
	free(gate);
}


/**
 * Create a gate from a value.
 *   @ret: Ref. The returned value.
 *   @value: The value.
 *   @env: The environment.
 *   &returns: Error.
 */
char *amp_gate_make(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env)
{
#define onexit
	struct amp_effect_t left, right;

	chkfail(amp_match_unpack(value, "(E,E)", &left, &right));

	*ret = amp_pack_effect((struct amp_effect_t){ amp_gate_new(left, right), &amp_gate_iface });
	return NULL;
#undef onexit
}


/**
 * Handle information on a gate.
 *   @gate: The gate.
 *   @info: The information.
 */
void amp_gate_info(struct amp_gate_t *gate, struct amp_info_t info)
{
	amp_effect_info(gate->left, info);
	amp_effect_info(gate->right, info);
}

/**
 * Process a gate.
 *   @gate: The gate.
 *   @buf: The buffer.
 *   @time: The time.
 *   @len: The length.
 *   @queue: The action queue.
 *   &returns: The continuation flag.
 */
bool amp_gate_proc(struct amp_gate_t *gate, double *buf, struct amp_time_t *time, unsigned int len, struct amp_queue_t *queue)
{
	unsigned int i;
	bool cont = false;
	double left[len], right[len];

	dsp_copy_d(left, buf, len);
	cont |= amp_effect_proc(gate->left, left, time, len, queue);

	dsp_copy_d(right, buf, len);
	cont |= amp_effect_proc(gate->right, right, time, len, queue);

	for(i = 0; i < len; i++)
		buf[i] = left[i] * right[i];

	return cont;
}
