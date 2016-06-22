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
 *   @feedback: Consumed. The feedback path.
 *   &returns: The chorus.
 */
struct amp_chorus_t *amp_chorus_new(unsigned int len, struct amp_param_t *osc, struct amp_param_t *feedback)
{
	struct amp_chorus_t *chorus;

	chorus = malloc(sizeof(struct amp_chorus_t));
	chorus->osc = osc;
	chorus->ring = dsp_ring_new(len);
	chorus->feedback = feedback;

	return chorus;
}

/**
 * Copy a chorus effect.
 *   @chorus: The original chorus.
 *   &returns: The copied chorus.
 */
struct amp_chorus_t *amp_chorus_copy(struct amp_chorus_t *chorus)
{
	return amp_chorus_new(chorus->ring->len, amp_param_copy(chorus->osc), amp_param_copy(chorus->feedback));
}

/**
 * Delete a chorus effect.
 *   @chorus: The chorus.
 */
void amp_chorus_delete(struct amp_chorus_t *chorus)
{
	amp_param_delete(chorus->osc);
	amp_param_delete(chorus->feedback);
	free(chorus);
}


/**
 * Create a chorus from a value.
 *   @ret: Ref. The returned value.
 *   @value: The value.
 *   @env: The environment.
 *   &returns: Error.
 */
char *amp_chorus_make(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env)
{
#define onexit
	double len;
	struct amp_param_t *osc, *feedback;

	chkfail(amp_match_unpack(value, "(f,P,P)", &len, &osc, &feedback));
	*ret = amp_pack_effect((struct amp_effect_t){ amp_chorus_new(len * amp_core_rate(env), osc, feedback), &amp_chorus_iface });

	return NULL;
#undef onexit
}


/**
 * Handle information on a chorus.
 *   @chorus: The chorus.
 *   @info: The information.
 */
void amp_chorus_info(struct amp_chorus_t *chorus, struct amp_info_t info)
{
	amp_param_info(chorus->osc, info);
	amp_param_info(chorus->feedback, info);
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
	double delay[len], feedback[len];
	bool cont = false;
	struct dsp_ring_t *ring = chorus->ring;

	cont |= amp_param_proc(chorus->osc, delay, time, len, queue);
	cont |= amp_param_proc(chorus->feedback, feedback, time, len, queue);

	for(i = 0; i < len; i++) {
		double x, v;

		v = dsp_ring_getf(ring, dsp_clamp(delay[i], 0.0, 1.0));
		x = buf[i] + dsp_clamp(feedback[i], 0.0, 0.999) * v;
		dsp_ring_put(ring, x);
		buf[i] = v;
	}

	return cont;
}
