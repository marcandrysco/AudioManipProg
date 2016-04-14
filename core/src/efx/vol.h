#ifndef EFX_VOL_H
#define EFX_VOL_H

/**
 * Volume structure.
 *   @s: The state.
 *   @lpf: The low-pass filter frequency.
 *   @rate: The sample rate.
 */
struct amp_vol_t {
	double s;
	struct amp_param_t *lpf;
	unsigned int rate;
};

/*
 * vol declarations
 */
extern const struct amp_effect_i amp_vol_iface;

struct amp_vol_t *amp_vol_new(struct amp_param_t *lpf, unsigned int rate);
struct amp_vol_t *amp_vol_copy(struct amp_vol_t *vol);
void amp_vol_delete(struct amp_vol_t *vol);

char *amp_vol_make(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env);

void amp_vol_info(struct amp_vol_t *vol, struct amp_info_t info);
bool amp_vol_proc(struct amp_vol_t *vol, double *buf, struct amp_time_t *time, unsigned int len, struct amp_queue_t *queue);

#endif
