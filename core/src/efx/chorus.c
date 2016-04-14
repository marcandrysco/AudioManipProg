#include "../common.h"

/*
 * global variables
 */
const struct amp_effect_i amp_chorus_iface = {
	(amp_info_f)amp_chorus_info,
	(amp_effect_f)amp_chorus_proc,
	(amp_copy_f)amp_chorus_copy,
	(amp_delete_f)amp_chorus_delete
};


/**
 * Create a chorus effect.
 *   @len: The length in samples.
 *   @osc: Consumed. The oscillator.
 *   @depth: Consumed. The depth.
 *   @feedback1: Consumed. The first feedback path.
 *   @feedback2: Consumed. The second feedback path.
 *   &returns: The chorus.
 */
struct amp_chorus_t *amp_chorus_new(unsigned int len, struct amp_param_t *osc, struct amp_param_t *depth, struct amp_param_t *feedback1, struct amp_param_t *feedback2)
{
	struct amp_chorus_t *chorus;

	chorus = malloc(sizeof(struct amp_chorus_t));
	chorus->osc = osc;
	chorus->depth = depth;
	chorus->ring = dsp_ring_new(len);
	chorus->feedback[0] = feedback1;
	chorus->feedback[1] = feedback2;

	return chorus;
}

/**
 * Copy a chorus effect.
 *   @chorus: The original chorus.
 *   &returns: The copied chorus.
 */
struct amp_chorus_t *amp_chorus_copy(struct amp_chorus_t *chorus)
{
	return amp_chorus_new(chorus->ring->len, amp_param_copy(chorus->osc), amp_param_copy(chorus->depth), amp_param_copy(chorus->feedback[0]), amp_param_copy(chorus->feedback[1]));
}

/**
 * Delete a chorus effect.
 *   @chorus: The chorus.
 */
void amp_chorus_delete(struct amp_chorus_t *chorus)
{
	amp_param_delete(chorus->osc);
	amp_param_delete(chorus->depth);
	amp_param_delete(chorus->feedback[0]);
	amp_param_delete(chorus->feedback[1]);
	free(chorus);
}


/**
 * Create a chorus from a value.
 *   @value: The value.
 *   @env: The environment.
 *   @err: The error.
 *   &returns: The value or null.
 */
struct ml_value_t *amp_chorus_make(struct ml_value_t *value, struct ml_env_t *env, char **err)
{
	double len;
	struct amp_param_t *osc, *depth, *feedback[2];

	*err = amp_match_unpack(value, "(f,P,P,P,P)", &len, &osc, &depth, &feedback[0], &feedback[1]);
	if(*err != NULL)
		return NULL;

	return amp_pack_effect((struct amp_effect_t){ amp_chorus_new(len * amp_core_rate(env), osc, depth, feedback[0], feedback[1]), &amp_chorus_iface });
}


/**
 * Handle information on a chorus.
 *   @chorus: The chorus.
 *   @info: The information.
 */
void amp_chorus_info(struct amp_chorus_t *chorus, struct amp_info_t info)
{
	amp_param_info(chorus->osc, info);
	amp_param_info(chorus->depth, info);
	amp_param_info(chorus->feedback[0], info);
	amp_param_info(chorus->feedback[1], info);
}

/**
 * Process a chorus.
 *   @chorus: The chorus.
 *   @buf: The buffer.
 *   @time: The time.
 *   @len: The length.
 *   @queue: The action queue.
 *   &returns: The continuation flag.
 */
bool amp_chorus_proc(struct amp_chorus_t *chorus, double *buf, struct amp_time_t *time, unsigned int len, struct amp_queue_t *queue)
{
	unsigned int i;
	double delay[len], depth[len], fb[2][len];
	bool cont = false;
	struct dsp_ring_t *ring = chorus->ring;

	cont |= amp_param_proc(chorus->osc, delay, time, len, queue);
	cont |= amp_param_proc(chorus->depth, depth, time, len, queue);
	cont |= amp_param_proc(chorus->feedback[0], fb[0], time, len, queue);
	cont |= amp_param_proc(chorus->feedback[1], fb[1], time, len, queue);

	for(i = 0; i < len; i++) {
		double x, v;

		v = dsp_ring_getf(ring, dsp_clamp(delay[i], 0.0, 1.0));
		x = buf[i] + fb[1][i] * v;
		dsp_ring_put(ring, x + fb[0][i] * v);
		buf[i] = x + depth[i] * v;
	}

	return cont;
}
