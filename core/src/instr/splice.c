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
 *   @effect: The effect.
 *   &returns: The splice.
 */
struct amp_splice_t *amp_splice_new(struct amp_effect_t effect)
{
	struct amp_splice_t *splice;

	splice = malloc(sizeof(struct amp_splice_t));
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
	return amp_splice_new(amp_effect_copy(splice->effect));
}

/**
 * Delete a splice.
 *   @splice: The splice.
 */
void amp_splice_delete(struct amp_splice_t *splice)
{
	amp_effect_delete(splice->effect);
	free(splice);
}


/**
 * Create a splice from a value.
 *   @ret: Ref. The returned value.
 *   @value: The value.
 *   @env: The environment.
 *   &returns: Error.
 */
char *amp_splice_make(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env)
{
#define onexit
	struct amp_effect_t effect;

	chkfail(amp_match_unpack(value, "E", &effect));

	*ret = amp_pack_instr((struct amp_instr_t){ amp_splice_new(effect), &amp_splice_iface });
	return NULL;
#undef onexit
}


/**
 * Handle information on the splice.
 *   @splice: The splice.
 *   @info: The information.
 */
void amp_splice_info(struct amp_splice_t *splice, struct amp_info_t info)
{
	amp_effect_info(splice->effect, info);
}

/**
 * Process a splice.
 *   @splice: The splice.
 *   @buf: The buffer.
 *   @time: The time.
 *   @len: The length.
 */
void amp_splice_proc(struct amp_splice_t *splice, double **buf, struct amp_time_t *time, unsigned int len, struct amp_queue_t *queue)
{
	unsigned int i;
	double tmp[len];

	for(i = 0; i < len; i++)
		tmp[i] = buf[0][i] + buf[1][i];

	amp_effect_proc(splice->effect, tmp, time, len, queue);

	dsp_copy_d(buf[0], tmp, len);
	dsp_copy_d(buf[1], tmp, len);
}
