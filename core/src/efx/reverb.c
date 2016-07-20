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
	opt_freqlo_e,
	opt_freqhi_e,
	opt_n
};

/**
 * Reverb structure.
 *   @type: The type.
 *   @vary, param: The varying parameter and parameter array.
 *   @fast, fixed: Fast and fixed parameter flag.
 *   @s, rate: The state and sample rate.
 *   @ring: The ring buffer.
 */
struct amp_reverb_t {
	enum amp_reverb_e type;
	struct amp_param_t *vary, *param[opt_n];

	bool fast, fixed;
	double i, s[8], rate;
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
 *   @vary: The varying parameter.
 *   @rate: The sample rate.
 *   &returns: The reverb.
 */
struct amp_reverb_t *amp_reverb_new(enum amp_reverb_e type, double len, struct amp_param_t *vary, double rate)
{
	unsigned int i;
	struct amp_reverb_t *reverb;

	reverb = malloc(sizeof(struct amp_reverb_t));
	reverb->type = type;
	reverb->rate = rate;
	reverb->fast = true;
	reverb->vary = vary;
	reverb->ring = dsp_ring_new(len * rate);
	reverb->fixed = (reverb->vary->type == amp_param_flt_e) && (reverb->vary->flt == len);
	reverb->i = 0.0;
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
	copy->vary = amp_param_copy(reverb->vary);
	copy->fixed = reverb->fixed;
	copy->i = 0.0;
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
	amp_param_delete(reverb->vary);
	free(reverb);
}


/**
 * Create a pure delay reverb.
 *   @len: The length.
 *   @vary: The varying delay.
 *   @gain: The gain.
 *   @rate: The sample rate.
 *   &returns: The reverb.
 */
struct amp_reverb_t *amp_reverb_delay(double len, struct amp_param_t *vary, struct amp_param_t *gain, double rate)
{
	struct amp_reverb_t *reverb;

	reverb = amp_reverb_new(amp_reverb_delay_e, len, vary, rate);
	reverb->fast = amp_param_isfast(gain);
	amp_param_set(&reverb->param[opt_gain_e], gain);

	return reverb;
}

/**
 * Create an allpass reverb.
 *   @len: The length.
 *   @vary: The varying delay.
 *   @gain: The gain.
 *   @rate: The sample rate.
 *   &returns: The reverb.
 */
struct amp_reverb_t *amp_reverb_allpass(double len, struct amp_param_t *vary, struct amp_param_t *gain, double rate)
{
	struct amp_reverb_t *reverb;

	reverb = amp_reverb_new(amp_reverb_allpass_e, len, vary, rate);
	reverb->fast = amp_param_isfast(gain);
	amp_param_set(&reverb->param[opt_gain_e], gain);

	return reverb;
}

/**
 * Create a comb-feedback reverb.
 *   @len: The length.
 *   @vary: The varying delay.
 *   @gain: The gain.
 *   @rate: The sample rate.
 *   &returns: The reverb.
 */
struct amp_reverb_t *amp_reverb_comb(double len, struct amp_param_t *vary, struct amp_param_t *gain, double rate)
{
	struct amp_reverb_t *reverb;

	reverb = amp_reverb_new(amp_reverb_comb_e, len, vary, rate);
	reverb->fast = amp_param_isfast(gain);
	amp_param_set(&reverb->param[opt_gain_e], gain);

	return reverb;
}

/**
 * Create a low-pass comb-feedback reverb.
 *   @len: The length.
 *   @vary: The varying delay.
 *   @gain: The gain.
 *   @freq: The frequency.
 *   @rate: The sample rate.
 *   &returns: The reverb.
 */
struct amp_reverb_t *amp_reverb_lpcf(double len, struct amp_param_t *vary, struct amp_param_t *gain, struct amp_param_t *freq, double rate)
{
	struct amp_reverb_t *reverb;

	reverb = amp_reverb_new(amp_reverb_lpcf_e, len, vary, rate);
	reverb->fast = amp_param_isfast(gain) && amp_param_isfast(freq);
	amp_param_set(&reverb->param[opt_gain_e], gain);
	amp_param_set(&reverb->param[opt_freq_e], freq);

	return reverb;
}

/**
 * Create a band-pass comb-feedback reverb.
 *   @len: The length.
 *   @vary: The varying delay.
 *   @gain: The gain.
 *   @freq: The frequency.
 *   @rate: The sample rate.
 *   &returns: The reverb.
 */
struct amp_reverb_t *amp_reverb_bpcf(double len, struct amp_param_t *vary, struct amp_param_t *gain, struct amp_param_t *freqlo, struct amp_param_t *freqhi, double rate)
{
	struct amp_reverb_t *reverb;

	reverb = amp_reverb_new(amp_reverb_bpcf_e, len, vary, rate);
	reverb->fast = amp_param_isfast(gain) && amp_param_isfast(freqlo) && amp_param_isfast(freqhi);
	amp_param_set(&reverb->param[opt_gain_e], gain);
	amp_param_set(&reverb->param[opt_freqlo_e], freqlo);
	amp_param_set(&reverb->param[opt_freqhi_e], freqhi);

	return reverb;
}

/**
 * Create a 2nd order band-pass comb-feedback reverb.
 *   @len: The length.
 *   @vary: The varying delay.
 *   @gain: The gain.
 *   @freq: The frequency.
 *   @rate: The sample rate.
 *   &returns: The reverb.
 */
struct amp_reverb_t *amp_reverb_bpcf2(double len, struct amp_param_t *vary, struct amp_param_t *gain, struct amp_param_t *freqlo, struct amp_param_t *freqhi, double rate)
{
	struct amp_reverb_t *reverb;

	reverb = amp_reverb_new(amp_reverb_bpcf2_e, len, vary, rate);
	reverb->fast = amp_param_isfast(gain) && amp_param_isfast(freqlo) && amp_param_isfast(freqhi);
	amp_param_set(&reverb->param[opt_gain_e], gain);
	amp_param_set(&reverb->param[opt_freqlo_e], freqlo);
	amp_param_set(&reverb->param[opt_freqhi_e], freqhi);

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

	amp_param_info(reverb->vary, info);

	for(i = 0; i < opt_n; i++)
		amp_param_info(reverb->param[i], info);
}

/**
 * Process a reverb.
 *   @reverb: The reverb.
 *   @buf: The buffer.
 *   @time: The time.
 *   @len: The length.
 *   @queue: The action queue.
 *   &returns: The continuation flag.
 */
bool amp_reverb_proc(struct amp_reverb_t *reverb, double *buf, struct amp_time_t *time, unsigned int len, struct amp_queue_t *queue)
{
	unsigned int i;
	bool cont = false;
	struct dsp_ring_t *ring = reverb->ring;

	if(ring->len == 0)
		return false;

	switch(reverb->type) {
	case amp_reverb_delay_e:
		if(!reverb->fixed) {
		}
		else if(!reverb->fast) {
			double gain[len];

			cont |= amp_param_proc(reverb->param[opt_gain_e], buf, time, len, queue);

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
		if(!reverb->fixed) {
		}
		else if(!reverb->fast) {
			double gain[len];

			cont |= amp_param_proc(reverb->param[opt_gain_e], buf, time, len, queue);

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
		if(!reverb->fixed) {
			double gain[len], vary[len], rate = reverb->rate;

			cont |= amp_param_proc(reverb->vary, vary, time, len, queue);
			cont |= amp_param_proc(reverb->param[opt_gain_e], gain, time, len, queue);

			for(i = 0; i < len; i++)
				buf[i] = dsp_vary_comb(buf[i], ring, rate / vary[i], &reverb->i, gain[i]);
		}
		else if(!reverb->fast) {
			double gain[len];

			cont |= amp_param_proc(reverb->param[opt_gain_e], buf, time, len, queue);

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
		if(!reverb->fixed) {
			double *s = reverb->s, vary[len], gain[len], freq[len], rate = reverb->rate;

			cont |= amp_param_proc(reverb->vary, vary, time, len, queue);
			cont |= amp_param_proc(reverb->param[opt_gain_e], gain, time, len, queue);
			cont |= amp_param_proc(reverb->param[opt_freq_e], freq, time, len, queue);

			for(i = 0; i < len; i++)
				buf[i] = dsp_vary_lpcf(buf[i], ring, rate / vary[i], &reverb->i, gain[i], dsp_lpf_init(freq[i], rate), s);
		}
		else if(!reverb->fast) {
			double *s = reverb->s, gain[len], freq[len], rate = reverb->rate;

			cont |= amp_param_proc(reverb->param[opt_gain_e], gain, time, len, queue);
			cont |= amp_param_proc(reverb->param[opt_freq_e], freq, time, len, queue);

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

	case amp_reverb_bpcf_e:
		if(!reverb->fixed) {
			double *s = reverb->s, vary[len], gain[len], freqlo[len], freqhi[len], rate = reverb->rate;

			cont |= amp_param_proc(reverb->vary, vary, time, len, queue);
			cont |= amp_param_proc(reverb->param[opt_gain_e], gain, time, len, queue);
			cont |= amp_param_proc(reverb->param[opt_freqlo_e], freqlo, time, len, queue);
			cont |= amp_param_proc(reverb->param[opt_freqhi_e], freqhi, time, len, queue);

			for(i = 0; i < len; i++)
				buf[i] = dsp_vary_bpcf(buf[i], ring, rate / vary[i], &reverb->i, gain[i], dsp_bpf_init(freqlo[i], freqhi[i], rate), s);
		}
		else if(!reverb->fast) {
			double *s = reverb->s, gain[len], freqlo[len], freqhi[len], rate = reverb->rate;

			cont |= amp_param_proc(reverb->param[opt_gain_e], gain, time, len, queue);
			cont |= amp_param_proc(reverb->param[opt_freqlo_e], freqlo, time, len, queue);
			cont |= amp_param_proc(reverb->param[opt_freqhi_e], freqhi, time, len, queue);

			for(i = 0; i < len; i++)
				buf[i] = dsp_reverb_bpcf(buf[i], ring, gain[i], dsp_bpf_init(freqlo[i], freqhi[i], rate), s);
		}
		else {
			double *s = reverb->s, gain = reverb->param[opt_gain_e]->flt;
			struct dsp_bpf_t bpf = dsp_bpf_init(reverb->param[opt_freqlo_e]->flt, reverb->param[opt_freqhi_e]->flt, reverb->rate);

			for(i = 0; i < len; i++)
				buf[i] = dsp_reverb_bpcf(buf[i], ring, gain, bpf, s);
		}
		break;

	case amp_reverb_bpcf2_e:
		if(!reverb->fixed) {
			double *s = reverb->s, vary[len], gain[len], freqlo[len], freqhi[len], rate = reverb->rate;

			cont |= amp_param_proc(reverb->vary, vary, time, len, queue);
			cont |= amp_param_proc(reverb->param[opt_gain_e], gain, time, len, queue);
			cont |= amp_param_proc(reverb->param[opt_freqlo_e], freqlo, time, len, queue);
			cont |= amp_param_proc(reverb->param[opt_freqhi_e], freqhi, time, len, queue);

			for(i = 0; i < len; i++)
				buf[i] = dsp_vary_bpcf2(buf[i], ring, rate / vary[i], &reverb->i, gain[i], dsp_bpf2_init(freqlo[i], freqhi[i], rate), s);
		}
		else if(!reverb->fast) {
			double *s = reverb->s, gain[len], freqlo[len], freqhi[len], rate = reverb->rate;

			cont |= amp_param_proc(reverb->param[opt_gain_e], gain, time, len, queue);
			cont |= amp_param_proc(reverb->param[opt_freqlo_e], freqlo, time, len, queue);
			cont |= amp_param_proc(reverb->param[opt_freqhi_e], freqhi, time, len, queue);

			for(i = 0; i < len; i++)
				buf[i] = dsp_reverb_bpcf2(buf[i], ring, gain[i], dsp_bpf2_init(freqlo[i], freqhi[i], rate), s);
		}
		else {
			double *s = reverb->s, gain = reverb->param[opt_gain_e]->flt;
			struct dsp_bpf2_t bpf = dsp_bpf2_init(reverb->param[opt_freqlo_e]->flt, reverb->param[opt_freqhi_e]->flt, reverb->rate);

			for(i = 0; i < len; i++)
				buf[i] = dsp_reverb_bpcf2(buf[i], ring, gain, bpf, s);
		}
		break;
	}

	return cont;
}


/**
 * Create a pure delay reverb from a value.
 *   @ret: Ref. The returned value.
 *   @value: The value.
 *   @env: The environment.
 *   &returns: Error.
 */
char *amp_delay_make(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env)
{
#define onexit
	double len;
	struct amp_param_t *vary, *gain;

	chkfail(amp_match_unpack(value, "(f,P,P)", &len, &vary, &gain));

	*ret = amp_pack_effect(amp_reverb_effect(amp_reverb_delay(len, vary, gain, amp_core_rate(env))));
	return NULL;
#undef onexit
}

/**
 * Create a all-pass reverb from a value.
 *   @ret: Ref. The returned value.
 *   @value: The value.
 *   @env: The environment.
 *   &returns: Error.
 */
char *amp_allpass_make(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env)
{
#define onexit
	double len;
	struct amp_param_t *vary, *gain;

	chkfail(amp_match_unpack(value, "(f,P,P)", &len, &vary, &gain));

	*ret = amp_pack_effect(amp_reverb_effect(amp_reverb_allpass(len, vary, gain, amp_core_rate(env))));
	return NULL;
#undef onexit
}

/**
 * Create a comb-feedback reverb from a value.
 *   @ret: Ref. The returned value.
 *   @value: The value.
 *   @env: The environment.
 *   &returns: Error.
 */
char *amp_comb_make(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env)
{
#define onexit
	double len;
	struct amp_param_t *vary, *gain;

	chkfail(amp_match_unpack(value, "(f,P,P)", &len, &vary, &gain));

	*ret = amp_pack_effect(amp_reverb_effect(amp_reverb_comb(len, vary, gain, amp_core_rate(env))));
	return NULL;
#undef onexit
}

/**
 * Create a low-pass comb-feedback reverb from a value.
 *   @ret: Ref. The returned value.
 *   @value: The value.
 *   @env: The environment.
 *   &returns: Error.
 */
char *amp_lpcf_make(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env)
{
#define onexit
	double len;
	struct amp_param_t *vary, *gain, *freq;

	chkfail(amp_match_unpack(value, "(f,P,P,P)", &len, &vary, &gain, &freq));

	*ret = amp_pack_effect(amp_reverb_effect(amp_reverb_lpcf(len, vary, gain, freq, amp_core_rate(env))));
	return NULL;
#undef onexit
}

/**
 * Create a band-pass comb-feedback reverb from a value.
 *   @ret: Ref. The returned value.
 *   @value: The value.
 *   @env: The environment.
 *   &returns: Error.
 */
char *amp_bpcf_make(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env)
{
#define onexit
	double len;
	struct amp_param_t *vary, *gain, *freqlo, *freqhi;

	chkfail(amp_match_unpack(value, "(f,P,P,P,P)", &len, &vary, &gain, &freqlo, &freqhi));

	*ret = amp_pack_effect(amp_reverb_effect(amp_reverb_bpcf(len, vary, gain, freqlo, freqhi, amp_core_rate(env))));
	return NULL;
#undef onexit
}

/**
 * Create a band-pass comb-feedback reverb from a value.
 *   @ret: Ref. The returned value.
 *   @value: The value.
 *   @env: The environment.
 *   &returns: Error.
 */
char *amp_bpcf2_make(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env)
{
#define onexit
	double len;
	struct amp_param_t *vary, *gain, *freqlo, *freqhi;

	chkfail(amp_match_unpack(value, "(f,P,P,P,P)", &len, &vary, &gain, &freqlo, &freqhi));

	*ret = amp_pack_effect(amp_reverb_effect(amp_reverb_bpcf2(len, vary, gain, freqlo, freqhi, amp_core_rate(env))));
	return NULL;
#undef onexit
}
