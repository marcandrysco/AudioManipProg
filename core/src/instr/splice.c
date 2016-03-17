#include "../common.h"


/*
 * local declarations
 */
struct amp_instr_i amp_splice_iface = {
	(amp_info_f)amp_splice_info,
	(amp_instr_f)amp_splice_proc,
	(amp_copy_f)amp_splice_copy,
	(amp_delete_f)amp_splice_delete
};


/**
 * Create a new splice.
 *   &returns: The splice.
 */

struct amp_splice_t *amp_splice_new(struct amp_value_t left, struct amp_value_t right, struct amp_effect_t effect)
{
	struct amp_splice_t *splice;

	splice = malloc(sizeof(struct amp_splice_t));
	splice->left = left;
	splice->right = right;
	splice->effect = effect;

	return splice;
}

/**
 * Copy a mixer.
 *   @splice: The original splice.
 *   &returns: The copied splice.
 */

struct amp_splice_t *amp_splice_copy(struct amp_splice_t *splice)
{
	return amp_splice_new(amp_value_copy(splice->left), amp_value_copy(splice->right), amp_effect_copy(splice->effect));
}

/**
 * Delete a splice.
 *   @splice: The splice.
 */

void amp_splice_delete(struct amp_splice_t *splice)
{
	amp_value_delete(splice->left);
	amp_value_delete(splice->right);
	amp_effect_delete(splice->effect);
	free(splice);
}


/**
 * Create a splice from a value.
 *   @value: The value.
 *   @env: The environment.
 *   @err: The rror.
 *   &returns: The value or null.
 */

struct ml_value_t *amp_splice_make(struct ml_value_t *value, struct ml_env_t *env, char **err)
{
	struct amp_value_t left, right;
	struct amp_effect_t effect;

	*err = amp_match_unpack(value, "((V,V),E)", &left, &right, &effect);
	return (*err == NULL) ? amp_pack_instr((struct amp_instr_t){ amp_splice_new(left, right, effect), &amp_splice_iface }) : NULL;
}


/**
 * Handle information on the splice.
 *   @splice: The splice.
 *   @info: The information.
 */

void amp_splice_info(struct amp_splice_t *splice, struct amp_info_t info)
{
	if(info.type == amp_info_action_e) {
		//amp_value_event(&splice->left, info.data.action->event);
		//amp_value_event(&splice->right, info.data.action->event);
		amp_effect_info(splice->effect, info);
	}
}

/**
 * Process a splice.
 *   @splice: The splice.
 *   @buf: The buffer.
 *   @time: The time.
 *   @len: The length.
 */

void amp_splice_proc(struct amp_splice_t *splice, double **buf, struct amp_time_t *time, unsigned int len)
{
	unsigned int i;
	double left, right, tmp[len];

	left = splice->left.flt;
	right = splice->right.flt;

	for(i = 0; i < len; i++)
		tmp[i] = left * buf[0][i] + right * buf[1][i];

	amp_effect_proc(splice->effect, tmp, time, len);

	dsp_copy_d(buf[0], tmp, len);
	dsp_copy_d(buf[1], tmp, len);
}
