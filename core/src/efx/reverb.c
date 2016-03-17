#include "../common.h"


/**
 * Option enumerator.
 *   @opt_gain_e: The gain.
 *   @opt_freq_e: Frequency.
 *   @opt_freqlo_e: Low frequency.
 *   @opt_freqhi_e: High frequency.
 *   @opt_n: The number of options.
 */

enum opt_e {
	opt_gain_e,
	opt_freq_e,
	//opt_freqlo_e,
	//opt_freqhi_e,
	opt_n
};

/**
 * Reverb structure.
 *   @type: The type.
 *   @param: The parameter array.
 *   @fast: Static parameter flag.
 *   @s, rate: The state and sample rate.
 *   @ring: The ring buffer.
 */

struct amp_reverb_t {
	enum amp_reverb_e type;
	struct amp_param_t *param[opt_n];

	bool fast;
	double s[8], rate;
	struct dsp_ring_t *ring;
};


/*
 * global variables
 */

const struct amp_effect_i amp_reverb_iface = {
	(amp_info_f)amp_reverb_info,
	(amp_effect_f)amp_reverb_proc,
	(amp_copy_f)amp_reverb_copy,
	(amp_delete_f)amp_reverb_delete
};


/**
 * Create a blank reverb.
 *   @type: The type.
 *   @len: The length.
 *   @rate: The sample rate.
 *   &returns: The reverb.
 */

struct amp_reverb_t *amp_reverb_new(enum amp_reverb_e type, double len, double rate)
{
	unsigned int i;
	struct amp_reverb_t *reverb;

	reverb = malloc(sizeof(struct amp_reverb_t));
	reverb->type = type;
	reverb->rate = rate;
	reverb->fast = true;
	reverb->ring = dsp_ring_new(len * rate);
	dsp_zero_d(reverb->s, 8);

	for(i = 0; i < opt_n; i++)
		reverb->param[i] = amp_param_flt(0.0);

	return reverb;
}

/**
 * Copy a reverb.
 *   @reverb: The original reverb.
 *   &returns: The copied reverb.
 */

struct amp_reverb_t *amp_reverb_copy(struct amp_reverb_t *reverb)
{
	unsigned int i;
	struct amp_reverb_t *copy;

	copy = malloc(sizeof(struct amp_reverb_t));
	copy->type = reverb->type;
	copy->rate = reverb->rate;
	copy->fast = reverb->fast;
	copy->ring = dsp_ring_new(reverb->ring->len);
	dsp_zero_d(copy->s, 8);

	for(i = 0; i < opt_n; i++)
		copy->param[i] = amp_param_copy(reverb->param[i]);

	return copy;
}

/**
 * Delete a reverb.
 *   @reverb: The reverb.
 */

void amp_reverb_delete(struct amp_reverb_t *reverb)
{
	unsigned int i;

	for(i = 0; i < opt_n; i++)
		amp_param_delete(reverb->param[i]);

	dsp_ring_delete(reverb->ring);
	free(reverb);
}


/**
 * Create a pure delay reverb.
 *   @len: The length.
 *   @gain: The gain.
 *   @rate: The sample rate.
 *   &returns: The reverb.
 */

struct amp_reverb_t *amp_reverb_delay(double len, struct amp_param_t *gain, double rate)
{
	struct amp_reverb_t *reverb;

	reverb = amp_reverb_new(amp_reverb_delay_e, len, rate);
	reverb->fast = amp_param_isfast(gain);
	amp_param_set(&reverb->param[opt_gain_e], gain);

	return reverb;
}

/**
 * Create an allpass reverb.
 *   @len: The length.
 *   @gain: The gain.
 *   @rate: The sample rate.
 *   &returns: The reverb.
 */

struct amp_reverb_t *amp_reverb_allpass(double len, struct amp_param_t *gain, double rate)
{
	struct amp_reverb_t *reverb;

	reverb = amp_reverb_new(amp_reverb_allpass_e, len, rate);
	reverb->fast = amp_param_isfast(gain);
	amp_param_set(&reverb->param[opt_gain_e], gain);

	return reverb;
}

/**
 * Create a comb-feedback reverb.
 *   @len: The length.
 *   @gain: The gain.
 *   @rate: The sample rate.
 *   &returns: The reverb.
 */

struct amp_reverb_t *amp_reverb_comb(double len, struct amp_param_t *gain, double rate)
{
	struct amp_reverb_t *reverb;

	reverb = amp_reverb_new(amp_reverb_comb_e, len, rate);
	reverb->fast = amp_param_isfast(gain);
	amp_param_set(&reverb->param[opt_gain_e], gain);

	return reverb;
}

/**
 * Create a low-pass comb-feedback reverb.
 *   @len: The length.
 *   @gain: The gain.
 *   @freq: The frequency.
 *   @rate: The sample rate.
 *   &returns: The reverb.
 */

struct amp_reverb_t *amp_reverb_lpcf(double len, struct amp_param_t *gain, struct amp_param_t *freq, double rate)
{
	struct amp_reverb_t *reverb;

	reverb = amp_reverb_new(amp_reverb_lpcf_e, len, rate);
	reverb->fast = amp_param_isfast(gain) | amp_param_isfast(freq);
	amp_param_set(&reverb->param[opt_gain_e], gain);
	amp_param_set(&reverb->param[opt_freq_e], freq);

	return reverb;
}


/**
 * Handle information on a reverb.
 *   @reverb: The reverb.
 *   @info: The information.
 */

void amp_reverb_info(struct amp_reverb_t *reverb, struct amp_info_t info)
{
	unsigned int i;

	for(i = 0; i < opt_n; i++)
		amp_param_info(reverb->param[i], info);
}

/**
 * Process a reverb.
 *   @reverb: The reverb.
 *   @buf: The buffer.
 *   @time: The time.
 *   @len: The length.
 *   &returns: The continuation flag.
 */

bool amp_reverb_proc(struct amp_reverb_t *reverb, double *buf, struct amp_time_t *time, unsigned int len)
{
	unsigned int i;
	bool cont = false;
	struct dsp_ring_t *ring = reverb->ring;

	switch(reverb->type) {
	case amp_reverb_delay_e:
		if(!reverb->fast) {
			double gain[len];

			cont |= amp_param_proc(reverb->param[opt_gain_e], buf, time, len);

			for(i = 0; i < len; i++)
				buf[i] = gain[i] * dsp_ring_proc(ring, buf[i]);
		}
		else {
			double gain = reverb->param[opt_gain_e]->flt;

			for(i = 0; i < len; i++)
				buf[i] = gain * dsp_ring_proc(ring, buf[i]);
		}

		break;

	case amp_reverb_allpass_e:
		if(!reverb->fast) {
			double gain[len];

			cont |= amp_param_proc(reverb->param[opt_gain_e], buf, time, len);

			for(i = 0; i < len; i++)
				buf[i] = dsp_reverb_allpass(buf[i], ring, gain[i]);
		}
		else {
			double gain = reverb->param[opt_gain_e]->flt;

			for(i = 0; i < len; i++)
				buf[i] = dsp_reverb_allpass(buf[i], ring, gain);
		}

		break;

	case amp_reverb_comb_e:
		if(!reverb->fast) {
			double gain[len];

			cont |= amp_param_proc(reverb->param[opt_gain_e], buf, time, len);

			for(i = 0; i < len; i++)
				buf[i] = dsp_reverb_comb(buf[i], ring, gain[i]);
		}
		else {
			double gain = reverb->param[opt_gain_e]->flt;

			for(i = 0; i < len; i++)
				buf[i] = dsp_reverb_comb(buf[i], ring, gain);
		}

		break;

	case amp_reverb_lpcf_e:
		if(!reverb->fast) {
			double *s = reverb->s, gain[len], freq[len], rate = reverb->rate;

			for(i = 0; i < len; i++)
				buf[i] = dsp_reverb_lpcf(buf[i], ring, gain[i], dsp_lpf_init(freq[i], rate), s);
		}
		else {
			double *s = reverb->s, gain = reverb->param[opt_gain_e]->flt;
			struct dsp_lpf_t lpf = dsp_lpf_init(reverb->param[opt_freq_e]->flt, reverb->rate);

			for(i = 0; i < len; i++)
				buf[i] = dsp_reverb_lpcf(buf[i], ring, gain, lpf, s);
		}

		break;
	}

	return cont;
}


/**
 * Create a pure delay reverb from a value.
 *   @value: The value.
 *   @env: The environment.
 *   @err: The error.
 *   &returns: The value or null.
 */

struct ml_value_t *amp_delay_make(struct ml_value_t *value, struct ml_env_t *env, char **err)
{
	double len;
	struct amp_param_t *gain;

	*err = amp_match_unpack(value, "(f,P)", &len, &gain);
	if(*err != NULL)
		return NULL;

	return amp_pack_effect(amp_reverb_effect(amp_reverb_delay(len, gain, amp_core_rate(env))));
}

/**
 * Create an all-pass reverb from a value.
 *   @value: The value.
 *   @env: The environment.
 *   @err: The error.
 *   &returns: The value or null.
 */

struct ml_value_t *amp_allpass_make(struct ml_value_t *value, struct ml_env_t *env, char **err)
{
	double len;
	struct amp_param_t *gain;

	*err = amp_match_unpack(value, "(f,P)", &len, &gain);
	if(*err != NULL)
		return NULL;

	return amp_pack_effect(amp_reverb_effect(amp_reverb_allpass(len, gain, amp_core_rate(env))));
}

/**
 * Create a comb-feedback reverb from a value.
 *   @value: The value.
 *   @env: The environment.
 *   @err: The error.
 *   &returns: The value or null.
 */

struct ml_value_t *amp_comb_make(struct ml_value_t *value, struct ml_env_t *env, char **err)
{
	double len;
	struct amp_param_t *gain;

	*err = amp_match_unpack(value, "(f,P)", &len, &gain);
	if(*err != NULL)
		return NULL;

	return amp_pack_effect(amp_reverb_effect(amp_reverb_comb(len, gain, amp_core_rate(env))));
}

/**
 * Create a low-pass comb-feedback reverb from a value.
 *   @value: The value.
 *   @env: The environment.
 *   @err: The error.
 *   &returns: The value or null.
 */

struct ml_value_t *amp_lpcf_make(struct ml_value_t *value, struct ml_env_t *env, char **err)
{
	double len;
	struct amp_param_t *gain, *freq;

	*err = amp_match_unpack(value, "(f,P,P)", &len, &gain, &freq);
	if(*err != NULL)
		return NULL;

	return amp_pack_effect(amp_reverb_effect(amp_reverb_lpcf(len, gain, freq, amp_core_rate(env))));
}
