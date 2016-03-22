#ifndef EFX_GAIN_H
#define EFX_GAIN_H

/**
 * Gain structure.
 *   @scale: The scale.
 */
struct amp_gain_t {
	struct amp_param_t *scale;
};

/*
 * gain declarations
 */
extern const struct amp_effect_i amp_gain_iface;

struct amp_gain_t *amp_gain_new(struct amp_param_t *scale);
struct amp_gain_t *amp_gain_copy(struct amp_gain_t *gain);
void amp_gain_delete(struct amp_gain_t *gain);

struct ml_value_t *amp_gain_make(struct ml_value_t *value, struct ml_env_t *env, char **err);

void amp_gain_info(struct amp_gain_t *gain, struct amp_info_t info);
bool amp_gain_proc(struct amp_gain_t *gain, double *buf, struct amp_time_t *time, unsigned int len);

#endif
