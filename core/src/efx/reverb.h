#ifndef EFX_REVERB_H
#define EFX_REVERB_H

/**
 * Reverb type enumerator.
 *   @amp_reverb_delay_e: Pure delay.
 *   @amp_reverb_allpass_e: Allpass filter.
 *   @amp_reverb_comb_e: Comb-feedback filter.
 *   @amp_reverb_lpcf_e: Low-pass comb-feedback filter.
 *   @amp_reverb_bpcf_e: Band-pass comb-feedback filter.
 */
enum amp_reverb_e {
	amp_reverb_delay_e,
	amp_reverb_allpass_e,
	amp_reverb_comb_e,
	amp_reverb_lpcf_e,
	amp_reverb_bpcf_e,
	amp_reverb_bpcf2_e
};


/*
 * reverb declarations
 */
struct amp_reverb_t;

extern const struct amp_effect_i amp_reverb_iface;

struct amp_reverb_t *amp_reverb_new(enum amp_reverb_e type, double len, struct amp_param_t *vary, double rate);
struct amp_reverb_t *amp_reverb_copy(struct amp_reverb_t *reverb);
void amp_reverb_delete(struct amp_reverb_t *reverb);

struct amp_reverb_t *amp_reverb_delay(double len, struct amp_param_t *vary, struct amp_param_t *gain, double rate);
struct amp_reverb_t *amp_reverb_allpass(double len, struct amp_param_t *vary, struct amp_param_t *gain, double rate);
struct amp_reverb_t *amp_reverb_comb(double len, struct amp_param_t *vary, struct amp_param_t *gain, double rate);
struct amp_reverb_t *amp_reverb_lpcf(double len, struct amp_param_t *vary, struct amp_param_t *gain, struct amp_param_t *freq, double rate);
struct amp_reverb_t *amp_reverb_bpcf(double len, struct amp_param_t *vary, struct amp_param_t *gain, struct amp_param_t *freqlo, struct amp_param_t *freqhi, double rate);
struct amp_reverb_t *amp_reverb_bpcf2(double len, struct amp_param_t *vary, struct amp_param_t *gain, struct amp_param_t *freqlo, struct amp_param_t *freqhi, double rate);

void amp_reverb_info(struct amp_reverb_t *reverb, struct amp_info_t info);
bool amp_reverb_proc(struct amp_reverb_t *reverb, double *buf, struct amp_time_t *time, unsigned int len, struct amp_queue_t *queue);

char *amp_delay_make(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env);
char *amp_allpass_make(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env);
char *amp_comb_make(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env);
char *amp_lpcf_make(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env);
char *amp_bpcf_make(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env);
char *amp_bpcf2_make(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env);


/**
 * Cast a reverber to an effect.
 *   @reverb: The reverber.
 *   &returns: The effect.
 */
static inline struct amp_effect_t amp_reverb_effect(struct amp_reverb_t *reverb)
{
	return (struct amp_effect_t){ reverb, &amp_reverb_iface };
}

#endif
