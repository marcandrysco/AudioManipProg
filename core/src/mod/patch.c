#include "../common.h"


/**
 * Patch structure.
 *   @input: The input module.
 *   @effect: The effect.
 */
struct amp_patch_t {
	struct amp_module_t input;
	struct amp_effect_t effect;
};


/*
 * global variables
 */
const struct amp_module_i amp_patch_iface = {
	(amp_info_f)amp_patch_info,
	(amp_module_f)amp_patch_proc,
	(amp_copy_f)amp_patch_copy,
	(amp_delete_f)amp_patch_delete
};


/**
 * Create a patch.
 *   @input: Consumed. The input module.
 *   @effect: Consumed. The applied effect.
 *   &returns: The patch.
 */
struct amp_patch_t *amp_patch_new(struct amp_module_t input, struct amp_effect_t effect)
{
	struct amp_patch_t *patch;

	patch = malloc(sizeof(struct amp_patch_t));
	patch->input = input;
	patch->effect = effect;

	return patch;
}

/**
 * Copy a patch.
 *   @patch: The original patch.
 *   &returns: The copied patch.
 */
struct amp_patch_t *amp_patch_copy(struct amp_patch_t *patch)
{
	return amp_patch_new(amp_module_copy(patch->input), amp_effect_copy(patch->effect));
}

/**
 * Delete a patch.
 *   @patch: The patch.
 */
void amp_patch_delete(struct amp_patch_t *patch)
{
	amp_module_delete(patch->input);
	amp_effect_delete(patch->effect);
	free(patch);
}


/**
 * Create a patch from a value.
 *   @value: The value.
 *   @env: The environment.
 *   @err: The error.
 *   &returns: The value or null.
 */
struct ml_value_t *amp_patch_make(struct ml_value_t *value, struct ml_env_t *env, char **err)
{
	struct amp_module_t input;
	struct amp_effect_t effect;

	*err = amp_match_unpack(value, "(M,E)", &input, &effect);
	if(*err == NULL)
		return amp_pack_module((struct amp_module_t){ amp_patch_new(input, effect), &amp_patch_iface });
	else
		return NULL;
}


/**
 * Handle information on a patch.
 *   @patch: The patch.
 *   @info: The information.
 */
void amp_patch_info(struct amp_patch_t *patch, struct amp_info_t info)
{
	amp_module_info(patch->input, info);
	amp_effect_info(patch->effect, info);
}

/**
 * Process a patch.
 *   @patch: The patch.
 *   @buf: The buffer.
 *   @time: The time.
 *   @len: The length.
 *   @queue: The action queue.
 *   &returns: The continuation flag.
 */
bool amp_patch_proc(struct amp_patch_t *patch, double *buf, struct amp_time_t *time, unsigned int len, struct amp_queue_t *queue)
{
	bool cont = false;

	cont |= amp_module_proc(patch->input, buf, time, len, queue);
	cont |= amp_effect_proc(patch->effect, buf, time, len, queue);

	return cont;
}
