#ifndef EFX_GAIN_H
#define EFX_GAIN_H

/**
 * Gain type enumerator.
 *   @amp_gain_mul_v: Straight multiplication.
 *   @amp_gain_boost_v: Decibel boost.
 *   @amp_gain_cut_v: Decibel cut.
 */
enum amp_gain_e {
	amp_gain_mul_v,
	amp_gain_boost_v,
	amp_gain_cut_v
};


/*
 * gain declarations
 */
extern const struct amp_effect_i amp_gain_iface;

struct amp_gain_t *amp_gain_new(enum amp_gain_e type, struct amp_param_t *scale);
struct amp_gain_t *amp_gain_copy(struct amp_gain_t *gain);
void amp_gain_delete(struct amp_gain_t *gain);

char *amp_gain_make(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env);
char *amp_boost_make(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env);
char *amp_cut_make(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env);

void amp_gain_info(struct amp_gain_t *gain, struct amp_info_t info);
bool amp_gain_proc(struct amp_gain_t *gain, double *buf, struct amp_time_t *time, unsigned int len, struct amp_queue_t *queue);

#endif
