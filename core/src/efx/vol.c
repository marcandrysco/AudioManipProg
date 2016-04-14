#include "../common.h"

/*
 * global variables
 */
const struct amp_effect_i amp_vol_iface = {
	(amp_info_f)amp_vol_info,
	(amp_effect_f)amp_vol_proc,
	(amp_copy_f)amp_vol_copy,
	(amp_delete_f)amp_vol_delete
};


/**
 * Create a volume effect.
 *   @lpf: Consumed. The low-pass filter frequency.
 *   @rate: The sample rate.
 *   &returns: The volume.
 */
struct amp_vol_t *amp_vol_new(struct amp_param_t *lpf, unsigned int rate)
{
	struct amp_vol_t *vol;

	vol = malloc(sizeof(struct amp_vol_t));
	vol->s = 0;
	vol->lpf = lpf;
	vol->rate = rate;

	return vol;
}

/**
 * Copy a volume effect.
 *   @vol: The original volume.
 *   &returns: The copied volume.
 */
struct amp_vol_t *amp_vol_copy(struct amp_vol_t *vol)
{
	return amp_vol_new(amp_param_copy(vol->lpf), vol->rate);
}

/**
 * Delete a volume effect.
 *   @vol: The volume.
 */
void amp_vol_delete(struct amp_vol_t *vol)
{
	amp_param_delete(vol->lpf);
	free(vol);
}


/**
 * Create a volume from a value.
 *   @ret: Ref. The returned value.
 *   @value: The value.
 *   @env: The environment.
 *   &returns: Error.
 */
char *amp_vol_make(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env)
{
#define onexit
	struct amp_param_t *lpf;

	chkfail(amp_match_unpack(value, "P", &lpf));

	*ret = amp_pack_effect((struct amp_effect_t){ amp_vol_new(lpf, amp_core_rate(env)), &amp_vol_iface });
	return NULL;
#undef onexit
}


/**
 * Handle information on a volume.
 *   @vol: The volume.
 *   @info: The information.
 */
void amp_vol_info(struct amp_vol_t *vol, struct amp_info_t info)
{
	amp_param_info(vol->lpf, info);
}

/**
 * Process a volume.
 *   @vol: The volume.
 *   @buf: The buffer.
 *   @time: The time.
 *   @len: The length.
 *   @queue: The action queue.
 *   &returns: The continuation flag.
 */
bool amp_vol_proc(struct amp_vol_t *vol, double *buf, struct amp_time_t *time, unsigned int len, struct amp_queue_t *queue)
{
	bool cont;
	unsigned int i;

	if(amp_param_isfast(vol->lpf)) {
		struct dsp_lpf_t lpf = dsp_lpf_init(vol->lpf->flt, vol->rate);

		for(i = 0; i < len; i++)
			buf[i] = dsp_lpf_proc(fabs(buf[i] * (M_PI / 2.0)), lpf, &vol->s);
	}
	else {
		double freq[len];

		cont = amp_param_proc(vol->lpf, freq, time, len, queue);

		for(i = 0; i < len; i++)
			buf[i] = dsp_lpf_proc(fabs(buf[i] * (M_PI / 2.0)), dsp_lpf_init(freq[i], vol->rate), &vol->s);
	}

	return cont;
}
