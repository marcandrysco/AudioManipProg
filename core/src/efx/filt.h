#ifndef EFX_FILT_H
#define EFX_FILT_H

/**
 * Filter type enumerator.
 *   @amp_filt_lpf_e: Simple low-pass filter.
 *   @amp_filt_hpf_e: Simple high-pass filter.
 *   @amp_filt_peak_e: Peaking filter.
 *   @amp_filt_res_e: Resonance filter.
 *   @amp_filt_moog_e: Moog VCF.
 */

enum amp_filt_e {
	amp_filt_lpf_e,
	amp_filt_hpf_e,
	amp_filt_peak_e,
	amp_filt_res_e,
	amp_filt_moog_e,
};


/*
 * filter declarations
 */

struct amp_filt_t;

extern const struct amp_effect_i amp_filt_iface;

struct amp_filt_t *amp_filt_new(enum amp_filt_e type, double rate);
struct amp_filt_t *amp_filt_copy(struct amp_filt_t *filt);
void amp_filt_delete(struct amp_filt_t *filt);

struct amp_filt_t *amp_filt_lpf(struct amp_param_t *freq, double rate);
struct amp_filt_t *amp_filt_hpf(struct amp_param_t *freq, double rate);
struct amp_filt_t *amp_filt_peak(struct amp_param_t *freq, struct amp_param_t *gain, struct amp_param_t *qual, double rate);
struct amp_filt_t *amp_filt_res(struct amp_param_t *freq, struct amp_param_t *qual, double rate);
struct amp_filt_t *amp_filt_moog(struct amp_param_t *freq, struct amp_param_t *res, double rate);

void amp_filt_info(struct amp_filt_t *filt, struct amp_info_t info);
bool amp_filt_proc(struct amp_filt_t *filt, double *buf, struct amp_time_t *time, unsigned int len);

struct ml_value_t *amp_lpf_make(struct ml_value_t *value, struct ml_env_t *env, char **err);
struct ml_value_t *amp_hpf_make(struct ml_value_t *value, struct ml_env_t *env, char **err);
struct ml_value_t *amp_peak_make(struct ml_value_t *value, struct ml_env_t *env, char **err);
struct ml_value_t *amp_res_make(struct ml_value_t *value, struct ml_env_t *env, char **err);
struct ml_value_t *amp_moog_make(struct ml_value_t *value, struct ml_env_t *env, char **err);


/**
 * Cast a filter to an effect.
 *   @filt: The filter.
 *   &returns: The effect.
 */

static inline struct amp_effect_t amp_filt_effect(struct amp_filt_t *filt)
{
	return (struct amp_effect_t){ filt, &amp_filt_iface };
}

#endif
