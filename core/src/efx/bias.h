#ifndef EFX_BIAS_H
#define EFX_BIAS_H

/**
 * Bias structure.
 *   @value: The value.
 */
struct amp_bias_t {
	struct amp_param_t *value;
};

/*
 * bias declarations
 */
extern const struct amp_effect_i amp_bias_iface;

struct amp_bias_t *amp_bias_new(struct amp_param_t *val);
struct amp_bias_t *amp_bias_copy(struct amp_bias_t *bias);
void amp_bias_delete(struct amp_bias_t *bias);

char *amp_bias_make(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env);

void amp_bias_info(struct amp_bias_t *bias, struct amp_info_t info);
bool amp_bias_proc(struct amp_bias_t *bias, double *buf, struct amp_time_t *time, unsigned int len, struct amp_queue_t *queue);

#endif
