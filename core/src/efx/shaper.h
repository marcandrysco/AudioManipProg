#ifndef EFX_SHAPER_H
#define EFX_SHAPER_H

/*
 * shaper declarations
 */
extern const struct amp_effect_i amp_shaper_iface;

struct amp_shaper_t *amp_shaper_new(struct amp_param_t *thresh);
struct amp_shaper_t *amp_shaper_copy(struct amp_shaper_t *shaper);
void amp_shaper_delete(struct amp_shaper_t *shaper);

char *amp_shaper_make(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env);

void amp_shaper_info(struct amp_shaper_t *shaper, struct amp_info_t info);
bool amp_shaper_proc(struct amp_shaper_t *shaper, double *buf, struct amp_time_t *time, unsigned int len, struct amp_queue_t *queue);

#endif
