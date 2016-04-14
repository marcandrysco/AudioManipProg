#include "../common.h"


/**
 * Oscillator structure.
 *   @t: The time.
 *   @type: The type.
 *   @freq, warp: The frequency and warping.
 *   @prev: The previous warp.
 *   @rate: Sample rate.
 */
struct amp_osc_t {
	double t;
	enum amp_osc_e type;
	struct amp_param_t *freq, *warp;
	double prev;

	unsigned int rate;
};


/*
 * global variables
 */
const struct amp_module_i amp_osc_iface = {
	(amp_info_f)amp_osc_info,
	(amp_effect_f)amp_osc_proc,
	(amp_copy_f)amp_osc_copy,
	(amp_delete_f)amp_osc_delete
};


/**
 * Create an oscillator.
 *   @type: The type.
 *   @freq: Consumed. The frequency parameter.
 *   @warp: Consumed. The warping parameter.
 *   &returns: The oscillator.
 */
struct amp_osc_t *amp_osc_new(enum amp_osc_e type, struct amp_param_t *freq, struct amp_param_t *warp, unsigned int rate)
{
	struct amp_osc_t *osc;

	osc = malloc(sizeof(struct amp_osc_t));
	osc->t = 0.0;
	osc->type = type;
	osc->freq = freq;
	osc->warp = warp;
	osc->rate = rate;
	osc->prev = 0.0;

	return osc;
}

/**
 * Copy an oscillator.
 *   @osc: The original oscillator.
 *   &returns: The copied oscillator.
 */
struct amp_osc_t *amp_osc_copy(struct amp_osc_t *osc)
{
	return amp_osc_new(osc->type, amp_param_copy(osc->freq), amp_param_copy(osc->warp), osc->rate);
}

/**
 * Delete an oscillator.
 *   @osc: The oscillator.
 */
void amp_osc_delete(struct amp_osc_t *osc)
{
	amp_param_delete(osc->freq);
	amp_param_delete(osc->warp);
	free(osc);
}


/**
 * Create an oscillator from a value.
 *   @value: The value.
 *   @env: The environment.
 *   @err: The error.
 *   &returns: The value or null.
 */

struct ml_value_t *amp_osc_make(struct ml_value_t *value, struct ml_env_t *env, char **err)
{
	int type;
	struct amp_param_t *freq, *warp;

	*err = amp_match_unpack(value, "(eO,P,P)", &type, &freq, &warp);
	if(*err != NULL)
		return NULL;

	return amp_pack_module((struct amp_module_t){ amp_osc_new(type, freq, warp, amp_core_rate(env)), &amp_osc_iface });
}


/**
 * Handle information on an oscillator.
 *   @osc: The oscillator.
 *   @info: The information.
 */
void amp_osc_info(struct amp_osc_t *osc, struct amp_info_t info)
{
	if(info.type == amp_info_note_e) {
		if(info.data.note->init)
			osc->t = 0.0;
	}

	amp_param_info(osc->freq, info);
	amp_param_info(osc->warp, info);
}

/**
 * Process an oscillator.
 *   @osc: The oscillator.
 *   @buf: The buffer.
 *   @time: The time.
 *   @len: The length.
 *   @queue: The action queue.
 *   &returns: The continuation flag.
 */
bool amp_osc_proc(struct amp_osc_t *osc, double *buf, struct amp_time_t *time, unsigned int len, struct amp_queue_t *queue)
{
	double t, freq[len];
	unsigned int i, rate;

	if(amp_param_isfast(osc->warp)) {
		double warp;

		rate = osc->rate;
		warp = osc->warp->flt;
		t = dsp_osc_unwarp(dsp_osc_warp(osc->t, osc->prev), warp);

		amp_param_proc(osc->freq, freq, time, len, queue);

		switch(osc->type) {
		case amp_osc_sine_e:
			for(i = 0; i < len; i++)
				buf[i] = dsp_osc_sine_f(dsp_osc_warp(t = dsp_osc_inc(t, dsp_osc_step(freq[i], rate)), warp));

			break;

		case amp_osc_tri_e:
			for(i = 0; i < len; i++)
				buf[i] = dsp_osc_tri(dsp_osc_warp(t = dsp_osc_inc(t, dsp_osc_step(freq[i], rate)), warp));

			break;

		case amp_osc_square_e:
			for(i = 0; i < len; i++)
				buf[i] = dsp_osc_square(dsp_osc_warp(t = dsp_osc_inc(t, dsp_osc_step(freq[i], rate)), warp));

			break;
		}

		osc->t = t;
		osc->prev = warp;
	}
	else {
	}

	return false;
}


/**
 * Convert a string oscillator type into a value.
 *   @str: The string.
 *   &returns: The enumerated value.
 */
int amp_osc_type(const char *str)
{
	if(strcmp(str, "sine") == 0)
		return amp_osc_sine_e;
	else if(strcmp(str, "tri") == 0)
		return amp_osc_tri_e;
	else if(strcmp(str, "square") == 0)
		return amp_osc_square_e;
	else
		return -1;
}
