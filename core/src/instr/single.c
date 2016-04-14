#include "../common.h"


/*
 * local declarations
 */
struct amp_instr_i amp_single_iface = {
	(amp_info_f)amp_single_info,
	(amp_instr_f)amp_single_proc,
	(amp_copy_f)amp_single_copy,
	(amp_delete_f)amp_single_delete
};


/**
 * Create a new single.
 *   @idx: The index.
 *   @effect: The effect.
 *   &returns: The single.
 */
struct amp_single_t *amp_single_new(unsigned int idx, struct amp_effect_t effect)
{
	struct amp_single_t *single;

	single = malloc(sizeof(struct amp_single_t));
	single->idx = idx;
	single->effect = effect;

	return single;
}

/**
 * Copy a mixer.
 *   @single: The original single.
 *   &returns: The copied single.
 */
struct amp_single_t *amp_single_copy(struct amp_single_t *single)
{
	return amp_single_new(single->idx, amp_effect_copy(single->effect));
}

/**
 * Delete a single.
 *   @single: The single.
 */
void amp_single_delete(struct amp_single_t *single)
{
	amp_effect_delete(single->effect);
	free(single);
}


/**
 * Create a single from a value.
 *   @value: The value.
 *   @env: The environment.
 *   @err: The rror.
 *   &returns: The value or null.
 */
struct ml_value_t *amp_single_make(struct ml_value_t *value, struct ml_env_t *env, char **err)
{
	int idx;
	struct amp_effect_t effect;

	*err = amp_match_unpack(value, "(d,E)", &idx, &effect);
	return (*err == NULL) ? amp_pack_instr((struct amp_instr_t){ amp_single_new(idx, effect), &amp_single_iface }) : NULL;
}


/**
 * Handle information on the single.
 *   @single: The single.
 *   @info: The information.
 */
void amp_single_info(struct amp_single_t *single, struct amp_info_t info)
{
	if(info.type == amp_info_action_e)
		amp_effect_info(single->effect, info);
}

/**
 * Process a single.
 *   @single: The single.
 *   @buf: The buffer.
 *   @time: The time.
 *   @len: The length.
 */
void amp_single_proc(struct amp_single_t *single, double **buf, struct amp_time_t *time, unsigned int len, struct amp_queue_t *queue)
{
	if(single->idx < 2)
		amp_effect_proc(single->effect, buf[single->idx], time, len, queue);
}
