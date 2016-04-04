#ifndef EFX_MIX_H
#define EFX_MIX_H

/**
 * Mix structure.
 *   @ratio: The ratio.
 *   @effect: The effect.
 */
struct amp_mix_t {
	struct amp_param_t *ratio;
	struct amp_effect_t effect;
};

/*
 * mix declarations
 */
extern const struct amp_effect_i amp_mix_iface;

struct amp_mix_t *amp_mix_new(struct amp_param_t *ratio, struct amp_effect_t effect);
struct amp_mix_t *amp_mix_copy(struct amp_mix_t *mix);
void amp_mix_delete(struct amp_mix_t *mix);

char *amp_mix_make(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env);

void amp_mix_info(struct amp_mix_t *mix, struct amp_info_t info);
bool amp_mix_proc(struct amp_mix_t *mix, double *buf, struct amp_time_t *time, unsigned int len);

#endif
