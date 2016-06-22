#include "../common.h"


/*
 * global variables
 */
const struct amp_effect_i amp_filt_iface = {
	(amp_info_f)amp_filt_info,
	(amp_effect_f)amp_filt_proc,
	(amp_copy_f)amp_filt_copy,
	(amp_delete_f)amp_filt_delete
};

/*
 * constructor definitions
 */
typedef struct amp_filt_t *amp_butter_f(struct amp_param_t *, double);


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

	for(i = 0; i < amp_filt_opt_n; i++)
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

	for(i = 0; i < amp_filt_opt_n; i++)
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

	for(i = 0; i < amp_filt_opt_n; i++)
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
	amp_param_set(&filt->param[amp_filt_opt_freq_e], freq);

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
	amp_param_set(&filt->param[amp_filt_opt_freq_e], freq);

	return filt;
}

/**
 * Create a state-variable low-pass filter.
 *   @freq: The frequency.
 *   @res: The resonance.
 *   @rate: The sample rate.
 *   &returns: The filter.
 */
struct amp_filt_t *amp_filt_svlpf(struct amp_param_t *freq, struct amp_param_t *res, double rate)
{
	struct amp_filt_t *filt;

	filt = amp_filt_new(amp_filt_svlpf_e, rate);
	filt->fast = amp_param_isfast(freq) && amp_param_isfast(res);
	amp_param_set(&filt->param[amp_filt_opt_freq_e], freq);
	amp_param_set(&filt->param[amp_filt_opt_res_e], res);

	return filt;
}

/**
 * Create a state-variable high-pass filter.
 *   @freq: The frequency.
 *   @res: The resonance.
 *   @rate: The sample rate.
 *   &returns: The filter.
 */
struct amp_filt_t *amp_filt_svhpf(struct amp_param_t *freq, struct amp_param_t *res, double rate)
{
	struct amp_filt_t *filt;

	filt = amp_filt_new(amp_filt_svhpf_e, rate);
	filt->fast = amp_param_isfast(freq) && amp_param_isfast(res);
	amp_param_set(&filt->param[amp_filt_opt_freq_e], freq);
	amp_param_set(&filt->param[amp_filt_opt_res_e], res);

	return filt;
}

/**
 * Create a peaking filter.
 *   @freq: The frequency.
 *   @gain: The gain.
 *   @qual: The quality.
 *   @rate: The sample rate.
 *   &returns: The filter.
 */
struct amp_filt_t *amp_filt_peak(struct amp_param_t *freq, struct amp_param_t *gain, struct amp_param_t *qual, double rate)
{
	struct amp_filt_t *filt;

	filt = amp_filt_new(amp_filt_peak_e, rate);
	filt->fast = amp_param_isfast(freq) && amp_param_isfast(gain) && amp_param_isfast(qual);
	amp_param_set(&filt->param[amp_filt_opt_freq_e], freq);
	amp_param_set(&filt->param[amp_filt_opt_gain_e], gain);
	amp_param_set(&filt->param[amp_filt_opt_qual_e], qual);

	return filt;
}

/**
 * Create a resonance filter.
 *   @freq: The frequency.
 *   @gain: The gain.
 *   @qual: The quality.
 *   @rate: The sample rate.
 *   &returns: The filter.
 */
struct amp_filt_t *amp_filt_res(struct amp_param_t *freq, struct amp_param_t *qual, double rate)
{
	struct amp_filt_t *filt;

	filt = amp_filt_new(amp_filt_res_e, rate);
	filt->fast = amp_param_isfast(freq) && amp_param_isfast(qual);
	amp_param_set(&filt->param[amp_filt_opt_freq_e], freq);
	amp_param_set(&filt->param[amp_filt_opt_qual_e], qual);

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

	filt = amp_filt_new(amp_filt_moog_e, rate);
	filt->fast = amp_param_isfast(freq) && amp_param_isfast(res);
	amp_param_set(&filt->param[amp_filt_opt_freq_e], freq);
	amp_param_set(&filt->param[amp_filt_opt_res_e], res);

	return filt;
}

/**
 * Create a 2nd-order butterworth low-pass filter.
 *   @freq: The frequency.
 *   @rate: The sample rate.
 *   &returns: The filter.
 */
struct amp_filt_t *amp_filt_butter2low(struct amp_param_t *freq, double rate)
{
	struct amp_filt_t *filt;

	filt = amp_filt_new(amp_filt_butter2low_e, rate);
	filt->fast = amp_param_isfast(freq);
	amp_param_set(&filt->param[amp_filt_opt_freq_e], freq);

	return filt;
}

/**
 * Create a 2nd-order butterworth high-pass filter.
 *   @freq: The frequency.
 *   @rate: The sample rate.
 *   &returns: The filter.
 */
struct amp_filt_t *amp_filt_butter2high(struct amp_param_t *freq, double rate)
{
	struct amp_filt_t *filt;

	filt = amp_filt_new(amp_filt_butter2high_e, rate);
	filt->fast = amp_param_isfast(freq);
	amp_param_set(&filt->param[amp_filt_opt_freq_e], freq);

	return filt;
}

/**
 * Create a 3rd-order butterworth low-pass filter.
 *   @freq: The frequency.
 *   @rate: The sample rate.
 *   &returns: The filter.
 */
struct amp_filt_t *amp_filt_butter3low(struct amp_param_t *freq, double rate)
{
	struct amp_filt_t *filt;

	filt = amp_filt_new(amp_filt_butter3low_e, rate);
	filt->fast = amp_param_isfast(freq);
	amp_param_set(&filt->param[amp_filt_opt_freq_e], freq);

	return filt;
}

/**
 * Create a 3rd-order butterworth high-pass filter.
 *   @freq: The frequency.
 *   @rate: The sample rate.
 *   &returns: The filter.
 */
struct amp_filt_t *amp_filt_butter3high(struct amp_param_t *freq, double rate)
{
	struct amp_filt_t *filt;

	filt = amp_filt_new(amp_filt_butter3high_e, rate);
	filt->fast = amp_param_isfast(freq);
	amp_param_set(&filt->param[amp_filt_opt_freq_e], freq);

	return filt;
}

/**
 * Create a 4th-order butterworth low-pass filter.
 *   @freq: The frequency.
 *   @rate: The sample rate.
 *   &returns: The filter.
 */
struct amp_filt_t *amp_filt_butter4low(struct amp_param_t *freq, double rate)
{
	struct amp_filt_t *filt;

	filt = amp_filt_new(amp_filt_butter4low_e, rate);
	filt->fast = amp_param_isfast(freq);
	amp_param_set(&filt->param[amp_filt_opt_freq_e], freq);

	return filt;
}

/**
 * Create a 4th-order butterworth high-pass filter.
 *   @freq: The frequency.
 *   @rate: The sample rate.
 *   &returns: The filter.
 */
struct amp_filt_t *amp_filt_butter4high(struct amp_param_t *freq, double rate)
{
	struct amp_filt_t *filt;

	filt = amp_filt_new(amp_filt_butter4high_e, rate);
	filt->fast = amp_param_isfast(freq);
	amp_param_set(&filt->param[amp_filt_opt_freq_e], freq);

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

	for(i = 0; i < amp_filt_opt_n; i++)
		amp_param_info(filt->param[i], info);
}

/**
 * Process a filter.
 *   @filt: The filter.
 *   @buf: The buffer.
 *   @time: The time.
 *   @len: The length.
 *   @queue: The action queue.
 *   &returns: The continuation flag.
 */
bool amp_filt_proc(struct amp_filt_t *filt, double *buf, struct amp_time_t *time, unsigned int len, struct amp_queue_t *queue)
{
	bool cont = false;
	unsigned int i;

	switch(filt->type) {
	case amp_filt_lpf_e:
		if(!filt->fast) {
			double freq[len];

			cont |= amp_param_proc(filt->param[amp_filt_opt_freq_e], freq, time, len, queue);

			for(i = 0; i < len; i++)
				buf[i] = dsp_lpf_proc(buf[i], dsp_lpf_init(freq[i], filt->rate), filt->s);
		}
		else {
			struct dsp_lpf_t c = dsp_lpf_init(filt->param[amp_filt_opt_freq_e]->flt, filt->rate);

			for(i = 0; i < len; i++)
				buf[i] = dsp_lpf_proc(buf[i], c, filt->s);
		}

		break;

	case amp_filt_hpf_e:
		if(!filt->fast) {
			double freq[len];

			cont |= amp_param_proc(filt->param[amp_filt_opt_freq_e], freq, time, len, queue);

			for(i = 0; i < len; i++)
				buf[i] = dsp_hpf_proc(buf[i], dsp_hpf_init(freq[i], filt->rate), filt->s);
		}
		else {
			struct dsp_hpf_t c = dsp_hpf_init(filt->param[amp_filt_opt_freq_e]->flt, filt->rate);

			for(i = 0; i < len; i++)
				buf[i] = dsp_hpf_proc(buf[i], c, filt->s);
		}

		break;

	case amp_filt_svlpf_e:
		if(!filt->fast) {
		}
		else {
			struct dsp_svf_t c = dsp_svf_init(filt->param[amp_filt_opt_freq_e]->flt, filt->param[amp_filt_opt_res_e]->flt, filt->rate);

			for(i = 0; i < len; i++)
				buf[i] = dsp_svf_low(buf[i], c, filt->s);
		}

		break;

	case amp_filt_svhpf_e:
		if(!filt->fast) {
		}
		else {
			struct dsp_svf_t c = dsp_svf_init(filt->param[amp_filt_opt_freq_e]->flt, filt->param[amp_filt_opt_res_e]->flt, filt->rate);

			for(i = 0; i < len; i++)
				buf[i] = dsp_svf_high(buf[i], c, filt->s);
		}

		break;

	case amp_filt_peak_e:
		if(!filt->fast) {
			double freq[len], gain[len], qual[len], rate = filt->rate;

			cont |= amp_param_proc(filt->param[amp_filt_opt_freq_e], freq, time, len, queue);
			cont |= amp_param_proc(filt->param[amp_filt_opt_gain_e], gain, time, len, queue);
			cont |= amp_param_proc(filt->param[amp_filt_opt_gain_e], qual, time, len, queue);

			for(i = 0; i < len; i++)
				buf[i] = dsp_peak_proc(buf[i], dsp_peak_init(freq[i], gain[i], qual[i], rate), filt->s);
		}
		else {
			struct dsp_peak_t c = dsp_peak_init(filt->param[amp_filt_opt_freq_e]->flt, filt->param[amp_filt_opt_gain_e]->flt, filt->param[amp_filt_opt_qual_e]->flt, filt->rate);

			for(i = 0; i < len; i++)
				buf[i] = dsp_peak_proc(buf[i], c, filt->s);
		}

		break;

	case amp_filt_res_e:
		if(!filt->fast) {
			double freq[len], qual[len], rate = filt->rate;

			cont |= amp_param_proc(filt->param[amp_filt_opt_freq_e], freq, time, len, queue);
			cont |= amp_param_proc(filt->param[amp_filt_opt_qual_e], qual, time, len, queue);

			for(i = 0; i < len; i++)
				buf[i] = dsp_res_proc(buf[i], dsp_res_init(freq[i], qual[i], rate), filt->s);
		}
		else {
			struct dsp_svf_t c = dsp_res_init(filt->param[amp_filt_opt_freq_e]->flt, filt->param[amp_filt_opt_qual_e]->flt, filt->rate);

			for(i = 0; i < len; i++)
				buf[i] = dsp_res_proc(buf[i], c, filt->s);
		}

		break;

	case amp_filt_moog_e:
		if(!filt->fast) {
			double freq[len], res[len];

			cont |= amp_param_proc(filt->param[amp_filt_opt_freq_e], freq, time, len, queue);
			cont |= amp_param_proc(filt->param[amp_filt_opt_res_e], res, time, len, queue);

			for(i = 0; i < len; i++)
				buf[i] = dsp_moog_proc(buf[i], dsp_moog_init(freq[i], res[i], filt->rate), filt->s);
		}
		else {
			struct dsp_moog_t c = dsp_moog_init(filt->param[amp_filt_opt_freq_e]->flt, filt->param[amp_filt_opt_res_e]->flt, filt->rate);

			for(i = 0; i < len; i++)
				buf[i] = dsp_moog_proc(buf[i], c, filt->s);
		}

		break;

	case amp_filt_butter2low_e:
		if(!filt->fast) {
			double freq[len];

			cont |= amp_param_proc(filt->param[amp_filt_opt_freq_e], freq, time, len, queue);

			for(i = 0; i < len; i++)
				buf[i] = dsp_butter2low_proc(buf[i], dsp_butter2low_init(freq[i], filt->rate), filt->s);
		}
		else {
			struct dsp_butter2low_t c = dsp_butter2low_init(filt->param[amp_filt_opt_freq_e]->flt, filt->rate);

			for(i = 0; i < len; i++)
				buf[i] = dsp_butter2low_proc(buf[i], c, filt->s);
		}

		break;

	case amp_filt_butter2high_e:
		if(!filt->fast) {
			double freq[len];

			cont |= amp_param_proc(filt->param[amp_filt_opt_freq_e], freq, time, len, queue);

			for(i = 0; i < len; i++)
				buf[i] = dsp_butter2high_proc(buf[i], dsp_butter2high_init(freq[i], filt->rate), filt->s);
		}
		else {
			struct dsp_butter2high_t c = dsp_butter2high_init(filt->param[amp_filt_opt_freq_e]->flt, filt->rate);

			for(i = 0; i < len; i++)
				buf[i] = dsp_butter2high_proc(buf[i], c, filt->s);
		}

		break;

	case amp_filt_butter3low_e:
		if(!filt->fast) {
			double freq[len];

			cont |= amp_param_proc(filt->param[amp_filt_opt_freq_e], freq, time, len, queue);

			for(i = 0; i < len; i++)
				buf[i] = dsp_butter3low_proc(buf[i], dsp_butter3low_init(freq[i], filt->rate), filt->s);
		}
		else {
			struct dsp_butter3low_t c = dsp_butter3low_init(filt->param[amp_filt_opt_freq_e]->flt, filt->rate);

			for(i = 0; i < len; i++)
				buf[i] = dsp_butter3low_proc(buf[i], c, filt->s);
		}

		break;

	case amp_filt_butter3high_e:
		if(!filt->fast) {
			double freq[len];

			cont |= amp_param_proc(filt->param[amp_filt_opt_freq_e], freq, time, len, queue);

			for(i = 0; i < len; i++)
				buf[i] = dsp_butter3high_proc(buf[i], dsp_butter3high_init(freq[i], filt->rate), filt->s);
		}
		else {
			struct dsp_butter3high_t c = dsp_butter3high_init(filt->param[amp_filt_opt_freq_e]->flt, filt->rate);

			for(i = 0; i < len; i++)
				buf[i] = dsp_butter3high_proc(buf[i], c, filt->s);
		}

		break;

	case amp_filt_butter4low_e:
		if(!filt->fast) {
			double freq[len];

			cont |= amp_param_proc(filt->param[amp_filt_opt_freq_e], freq, time, len, queue);

			for(i = 0; i < len; i++)
				buf[i] = dsp_butter4low_proc(buf[i], dsp_butter4low_init(freq[i], filt->rate), filt->s);
		}
		else {
			struct dsp_butter4low_t c = dsp_butter4low_init(filt->param[amp_filt_opt_freq_e]->flt, filt->rate);

			for(i = 0; i < len; i++)
				buf[i] = dsp_butter4low_proc(buf[i], c, filt->s);
		}

		break;

	case amp_filt_butter4high_e:
		if(!filt->fast) {
			double freq[len];

			cont |= amp_param_proc(filt->param[amp_filt_opt_freq_e], freq, time, len, queue);

			for(i = 0; i < len; i++)
				buf[i] = dsp_butter4high_proc(buf[i], dsp_butter4high_init(freq[i], filt->rate), filt->s);
		}
		else {
			struct dsp_butter4high_t c = dsp_butter4high_init(filt->param[amp_filt_opt_freq_e]->flt, filt->rate);

			for(i = 0; i < len; i++)
				buf[i] = dsp_butter4high_proc(buf[i], c, filt->s);
		}

		break;
	}

	return cont;
}


/**
 * Create a low-pass filter from a value.
 *   @ret: Ref. The returned value.
 *   @value: The value.
 *   @env: The environment.
 *   &returns: Error.
 */
char *amp_lpf_make(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env)
{
#define onexit
	struct amp_param_t *freq;

	chkfail(amp_match_unpack(value, "P", &freq));

	*ret = amp_pack_effect((struct amp_effect_t){ amp_filt_lpf(freq, amp_core_rate(env)), &amp_filt_iface });
	return NULL;
#undef onexit
}

/**
 * Create a high-pass filter from a value.
 *   @ret: Ref. The returned value.
 *   @value: The value.
 *   @env: The environment.
 *   &returns: Error.
 */
char *amp_hpf_make(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env)
{
#define onexit
	struct amp_param_t *freq;

	chkfail(amp_match_unpack(value, "P", &freq));

	*ret = amp_pack_effect((struct amp_effect_t){ amp_filt_hpf(freq, amp_core_rate(env)), &amp_filt_iface });
	return NULL;
#undef onexit
}

/**
 * Create a state-variable low-pass filter from a value.
 *   @ret: Ref. The returned value.
 *   @value: The value.
 *   @env: The environment.
 *   &returns: Error.
 */
char *amp_svlpf_make(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env)
{
#define onexit
	struct amp_param_t *freq, *res;

	chkfail(amp_match_unpack(value, "(P,P)", &freq, &res));

	*ret = amp_pack_effect(amp_filt_effect(amp_filt_svlpf(freq, res, amp_core_rate(env))));
	return NULL;
#undef onexit
}

/**
 * Create a state-variable high-pass filter from a value.
 *   @ret: Ref. The returned value.
 *   @value: The value.
 *   @env: The environment.
 *   &returns: Error.
 */
char *amp_svhpf_make(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env)
{
#define onexit
	struct amp_param_t *freq, *res;

	chkfail(amp_match_unpack(value, "(P,P)", &freq, &res));

	*ret = amp_pack_effect(amp_filt_effect(amp_filt_svhpf(freq, res, amp_core_rate(env))));
	return NULL;
#undef onexit
}

/**
 * Create a peaking filter from a value.
 *   @ret: Ref. The returned value.
 *   @value: The value.
 *   @env: The environment.
 *   &returns: Error.
 */
char *amp_peak_make(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env)
{
#define onexit
	struct amp_param_t *freq, *gain, *qual;

	chkfail(amp_match_unpack(value, "(P,P,P)", &freq, &gain, &qual));

	*ret = amp_pack_effect(amp_filt_effect(amp_filt_peak(freq, gain, qual, amp_core_rate(env))));
	return NULL;
#undef onexit
}

/**
 * Create a resonance filter from a value.
 *   @ret: Ref. The returned value.
 *   @value: The value.
 *   @env: The environment.
 *   &returns: Error.
 */
char *amp_res_make(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env)
{
#define onexit
	struct amp_param_t *freq, *qual;

	chkfail(amp_match_unpack(value, "(P,P)", &freq, &qual));

	*ret = amp_pack_effect(amp_filt_effect(amp_filt_res(freq, qual, amp_core_rate(env))));
	return NULL;
#undef onexit
}

/**
 * Create a Moog VCF from a value.
 *   @ret: Ref. The returned value.
 *   @value: The value.
 *   @env: The environment.
 *   &returns: Error.
 */
char *amp_moog_make(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env)
{
#define onexit
	struct amp_param_t *freq, *res;

	chkfail(amp_match_unpack(value, "(P,P)", &freq, &res));

	*ret = amp_pack_effect(amp_filt_effect(amp_filt_moog(freq, res, amp_core_rate(env))));
	return NULL;
#undef onexit
}


/**
 * Create a butterworth filter from a value.
 *   @value: The value.
 *   @env: The environment.
 *   @err: The error.
 *   &returns: The value or null.
 */
char *amp_butter_make(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env, amp_butter_f func)
{
#define onexit
	struct amp_param_t *freq;

	chkfail(amp_match_unpack(value, "P", &freq));

	*ret = amp_pack_effect(amp_filt_effect(func(freq, amp_core_rate(env))));
	return NULL;
#undef onexit
}
char *amp_butter2low_make(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env)
{
	return amp_butter_make(ret, value, env, amp_filt_butter2low);
}
char *amp_butter2high_make(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env)
{
	return amp_butter_make(ret, value, env, amp_filt_butter2high);
}
char *amp_butter3low_make(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env)
{
	return amp_butter_make(ret, value, env, amp_filt_butter3low);
}
char *amp_butter3high_make(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env)
{
	return amp_butter_make(ret, value, env, amp_filt_butter3high);
}
char *amp_butter4low_make(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env)
{
	return amp_butter_make(ret, value, env, amp_filt_butter4low);
}
char *amp_butter4high_make(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env)
{
	return amp_butter_make(ret, value, env, amp_filt_butter4high);
}


const char *amp_filt_name(enum amp_filt_e type)
{
	switch(type) {
	case amp_filt_lpf_e: return "lpf";
	case amp_filt_hpf_e: return "hpf";
	case amp_filt_svlpf_e: return "svlpf";
	case amp_filt_svhpf_e: return "svhpf";
	case amp_filt_peak_e: return "peak";
	case amp_filt_res_e: return "res";
	case amp_filt_moog_e: return "moog";
	case amp_filt_butter2low_e: return "butter2low";
	case amp_filt_butter2high_e: return "butter2high";
	case amp_filt_butter3low_e: return "butter3low";
	case amp_filt_butter3high_e: return "butter3high";
	case amp_filt_butter4low_e: return "butter4low";
	case amp_filt_butter4high_e: return "butter4high";
	}

	fatal("Invalid filter type.");
}
