#include "../common.h"


/**
 * Option enumerator.
 *   @opt_res_e: Resonance.
 *   @opt_qual_e: Quality.
 *   @opt_freq_e: Frequency.
 *   @opt_freqlo_e: Low frequency.
 *   @opt_freqhi_e: High frequency.
 *   @opt_gain_e: Gain.
 *   @opt_gainlo_e: Low gain.
 *   @opt_gainhi_e: High gain.
 *   @opt_n: The number of options.
 */
enum opt_e {
	opt_res_e,
	opt_qual_e,
	opt_freq_e,
	opt_freqlo_e,
	opt_freqhi_e,
	opt_gain_e,
	opt_gainlo_e,
	opt_gainhi_e,
	opt_n
};

/**
 * Filter structure.
 *   @type: The type.
 *   @param: The parameter array.
 *   @fast: Static parameter flag.
 *   @s, rate: The state and sample rate.
 */

struct amp_filt_t {
	enum amp_filt_e type;
	struct amp_param_t *param[opt_n];

	bool fast;
	double s[8], rate;
};


/*
 * global variables
 */

const struct amp_effect_i amp_filt_iface = {
	(amp_info_f)amp_filt_info,
	(amp_effect_f)amp_filt_proc,
	(amp_copy_f)amp_filt_copy,
	(amp_delete_f)amp_filt_delete
};


/**
 * Create a blank filter.
 *   @type: The type.
 *   @rate: The sample rate.
 *   &returns: The filter.
 */

struct amp_filt_t *amp_filt_new(enum amp_filt_e type, double rate)
{
	unsigned int i;
	struct amp_filt_t *filt;

	filt = malloc(sizeof(struct amp_filt_t));
	filt->type = type;
	filt->rate = rate;
	filt->fast = true;
	dsp_zero_d(filt->s, 8);

	for(i = 0; i < opt_n; i++)
		filt->param[i] = amp_param_flt(0.0);

	return filt;
}

/**
 * Copy a filter.
 *   @filt: The original filter.
 *   &returns: The copied filter.
 */

struct amp_filt_t *amp_filt_copy(struct amp_filt_t *filt)
{
	unsigned int i;
	struct amp_filt_t *copy;

	copy = malloc(sizeof(struct amp_filt_t));
	copy->type = filt->type;
	copy->rate = filt->rate;
	copy->fast = filt->fast;
	dsp_zero_d(copy->s, 8);

	for(i = 0; i < opt_n; i++)
		copy->param[i] = amp_param_copy(filt->param[i]);

	return copy;
}

/**
 * Delete a filter.
 *   @filt: The filter.
 */

void amp_filt_delete(struct amp_filt_t *filt)
{
	unsigned int i;

	for(i = 0; i < opt_n; i++)
		amp_param_delete(filt->param[i]);

	free(filt);
}


/**
 * Create a low-pass filter.
 *   @freq: The frequency.
 *   @rate: The sample rate.
 *   &returns: The filter.
 */

struct amp_filt_t *amp_filt_lpf(struct amp_param_t *freq, double rate)
{
	struct amp_filt_t *filt;

	filt = amp_filt_new(amp_filt_lpf_e, rate);
	filt->fast = amp_param_isfast(freq);
	amp_param_set(&filt->param[opt_freq_e], freq);

	return filt;
}

/**
 * Create a high-pass filter.
 *   @freq: The frequency.
 *   @rate: The sample rate.
 *   &returns: The filter.
 */

struct amp_filt_t *amp_filt_hpf(struct amp_param_t *freq, double rate)
{
	struct amp_filt_t *filt;

	filt = amp_filt_new(amp_filt_hpf_e, rate);
	filt->fast = amp_param_isfast(freq);
	amp_param_set(&filt->param[opt_freq_e], freq);

	return filt;
}

/**
 * Create a Moog VCF.
 *   @freq: The frequency.
 *   @res: The resonance.
 *   @rate: The sample rate.
 *   &returns: The filter.
 */

struct amp_filt_t *amp_filt_moog(struct amp_param_t *freq, struct amp_param_t *res, double rate)
{
	struct amp_filt_t *filt;

	filt = amp_filt_new(amp_filt_hpf_e, rate);
	filt->fast = amp_param_isfast(freq) && amp_param_isfast(res);
	amp_param_set(&filt->param[opt_freq_e], freq);
	amp_param_set(&filt->param[opt_res_e], res);

	return filt;
}


/**
 * Handle information on a filter.
 *   @filt: The filter.
 *   @info: The information.
 */

void amp_filt_info(struct amp_filt_t *filt, struct amp_info_t info)
{
	unsigned int i;

	for(i = 0; i < opt_n; i++)
		amp_param_info(filt->param[i], info);
}

/**
 * Process a filter.
 *   @filt: The filter.
 *   @buf: The buffer.
 *   @time: The time.
 *   @len: The length.
 *   &returns: The continuation flag.
 */

bool amp_filt_proc(struct amp_filt_t *filt, double *buf, struct amp_time_t *time, unsigned int len)
{
	bool cont = false;
	unsigned int i;

	switch(filt->type) {
	case amp_filt_lpf_e:
		if(!filt->fast) {
			double freq[len];

			cont |= amp_param_proc(filt->param[opt_freq_e], buf, time, len);

			for(i = 0; i < len; i++)
				buf[i] = dsp_lpf_proc(buf[i], dsp_lpf_init(freq[i], filt->rate), filt->s);
		}
		else {
			struct dsp_lpf_t c = dsp_lpf_init(filt->param[opt_freq_e]->flt, filt->rate);

			for(i = 0; i < len; i++)
				buf[i] = dsp_lpf_proc(buf[i], c, filt->s);
		}

		break;

	case amp_filt_hpf_e:
		if(!filt->fast) {
			double freq[len];

			cont |= amp_param_proc(filt->param[opt_freq_e], buf, time, len);

			for(i = 0; i < len; i++)
				buf[i] = dsp_hpf_proc(buf[i], dsp_hpf_init(freq[i], filt->rate), filt->s);
		}
		else {
			struct dsp_hpf_t c = dsp_hpf_init(filt->param[opt_freq_e]->flt, filt->rate);

			for(i = 0; i < len; i++)
				buf[i] = dsp_hpf_proc(buf[i], c, filt->s);
		}

		break;

	case amp_filt_moog_e:
		if(!filt->fast) {
			double freq[len], res[len];

			cont |= amp_param_proc(filt->param[opt_freq_e], buf, time, len);
			cont |= amp_param_proc(filt->param[opt_res_e], buf, time, len);

			for(i = 0; i < len; i++)
				buf[i] = dsp_moog_proc(buf[i], dsp_moog_init(freq[i], res[i], filt->rate), filt->s);
		}
		else {
			struct dsp_moog_t c = dsp_moog_init(filt->param[opt_freq_e]->flt, filt->param[opt_res_e]->flt, filt->rate);

			for(i = 0; i < len; i++)
				buf[i] = dsp_moog_proc(buf[i], c, filt->s);
		}

		break;
	}

	return cont;
}


/**
 * Create a low-pass filter from a value.
 *   @value: The value.
 *   @env: The environment.
 *   @err: The error.
 *   &returns: The value or null.
 */

struct ml_value_t *amp_lpf_make(struct ml_value_t *value, struct ml_env_t *env, char **err)
{
	struct amp_param_t *freq;

	*err = amp_match_unpack(value, "P", &freq);
	if(*err != NULL)
		return NULL;

	return amp_pack_effect(amp_filt_effect(amp_filt_lpf(freq, amp_core_rate(env))));
}

/**
 * Create a high-pass filter from a value.
 *   @value: The value.
 *   @env: The environment.
 *   @err: The error.
 *   &returns: The value or null.
 */

struct ml_value_t *amp_hpf_make(struct ml_value_t *value, struct ml_env_t *env, char **err)
{
	struct amp_param_t *freq;

	*err = amp_match_unpack(value, "P", &freq);
	if(*err != NULL)
		return NULL;

	return amp_pack_effect(amp_filt_effect(amp_filt_hpf(freq, amp_core_rate(env))));
}

/**
 * Create a Moog VCF from a value.
 *   @value: The value.
 *   @env: The environment.
 *   @err: The error.
 *   &returns: The value or null.
 */

struct ml_value_t *amp_moog_make(struct ml_value_t *value, struct ml_env_t *env, char **err)
{
	struct amp_param_t *freq, *res;

	*err = amp_match_unpack(value, "(P,P)", &freq, &res);
	if(*err != NULL)
		return NULL;

	return amp_pack_effect(amp_filt_effect(amp_filt_moog(freq, res, amp_core_rate(env))));
}
