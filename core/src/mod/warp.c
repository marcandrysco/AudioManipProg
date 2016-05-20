#include "../common.h"


/**
 * Sample structure.
 *   @phase: The phase signal.
 *   @dist: The amount of distortion.
 */
struct amp_warp_t {
	struct amp_module_t phase;
	struct amp_param_t *dist;
};


/*
 * global variables
 */
const struct amp_module_i amp_warp_iface = {
	(amp_info_f)amp_warp_info,
	(amp_module_f)amp_warp_proc,
	(amp_copy_f)amp_warp_copy,
	(amp_delete_f)amp_warp_delete
};


/**
 * Create a warp.
 *   @phase: Consumed. The phase signal.
 *   @dist: Consumed. The distortion.
 *   &returns: The warp.
 */
struct amp_warp_t *amp_warp_new(struct amp_module_t phase, struct amp_param_t *dist)
{
	struct amp_warp_t *warp;

	warp = malloc(sizeof(struct amp_warp_t));
	warp->phase = phase;
	warp->dist = dist;

	return warp;
}

/**
 * Copy a warp.
 *   @warp: The original warp.
 *   &returns: The copied warp.
 */
struct amp_warp_t *amp_warp_copy(struct amp_warp_t *warp)
{
	return amp_warp_new(amp_module_copy(warp->phase), amp_param_copy(warp->dist));
}

/**
 * Delete a warp.
 *   @warp: The warp.
 */
void amp_warp_delete(struct amp_warp_t *warp)
{
	amp_module_delete(warp->phase);
	amp_param_delete(warp->dist);
	free(warp);
}


/**
 * Create a warp from a value.
 *   @ret: Ref. The return value.
 *   @value: The value.
 *   @env: The environment.
 *   &returns: Error
 */
char *amp_warp_make(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env)
{
#define onexit
	struct amp_module_t phase;
	struct amp_param_t *dist;

	chkfail(amp_match_unpack(value, "(M,P)", &phase, &dist));
	
	*ret = amp_pack_module((struct amp_module_t){ amp_warp_new(phase, dist), &amp_warp_iface });
	return NULL;
#undef onexit
}


/**
 * Handle information on a warp.
 *   @warp: The warp.
 *   @info: The information.
 */
void amp_warp_info(struct amp_warp_t *warp, struct amp_info_t info)
{
	amp_module_info(warp->phase, info);
}

/**
 * Process a warp.
 *   @warp: The warp.
 *   @buf: The buffer.
 *   @time: The time.
 *   @len: The length.
 *   @queue: The action queue.
 *   &returns: The continuation flag.
 */
bool amp_warp_proc(struct amp_warp_t *warp, double *buf, struct amp_time_t *time, unsigned int len, struct amp_queue_t *queue)
{
	bool cont;
	unsigned int i;

	cont = amp_module_proc(warp->phase, buf, time, len, queue);

	if(amp_param_isfast(warp->dist)) {
		double dist = warp->dist->flt;

		for(i = 0; i < len; i++)
			buf[i] = dsp_osc_warp(fmod(buf[i] + 0.25, 1.0), dist) - 0.25;
	}
	else {
		double dist[len];

		cont |= amp_param_proc(warp->dist, dist, time, len, queue);

		for(i = 0; i < len; i++)
			buf[i] = dsp_osc_warp(fmod(buf[i] + 0.25, 1.0), dist[i]) - 0.25;
	}

	return cont;
}
