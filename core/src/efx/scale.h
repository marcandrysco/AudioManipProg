#ifndef EFX_SCALE_H
#define EFX_SCALE_H

/**
 * Scale structure.
 *   @fast: The fast flag.
 *   @inlo, inhi, outlo, outhi: The scale parameters.
 */
struct amp_scale_t {
	bool fast;
	struct amp_param_t *inlo, *inhi, *outlo, *outhi;
};

/*
 * scale declarations
 */
extern const struct amp_effect_i amp_scale_iface;

struct amp_scale_t *amp_scale_new(struct amp_param_t *inlo, struct amp_param_t *inhi, struct amp_param_t *outlo, struct amp_param_t *outhi);
struct amp_scale_t *amp_scale_copy(struct amp_scale_t *scale);
void amp_scale_delete(struct amp_scale_t *scale);

char *amp_scale_make(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env);

void amp_scale_info(struct amp_scale_t *scale, struct amp_info_t info);
bool amp_scale_proc(struct amp_scale_t *scale, double *buf, struct amp_time_t *time, unsigned int len, struct amp_queue_t *queue);

#endif
