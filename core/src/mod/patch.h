#ifndef MOD_PATCH_H
#define MOD_PATCH_H

/*
 * patch declarations
 */

struct amp_patch_t;

extern const struct amp_module_i amp_patch_iface;

struct amp_patch_t *amp_patch_new(struct amp_module_t input, struct amp_effect_t effect);
struct amp_patch_t *amp_patch_copy(struct amp_patch_t *patch);
void amp_patch_delete(struct amp_patch_t *patch);

char *amp_patch_make(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env);

void amp_patch_info(struct amp_patch_t *patch, struct amp_info_t info);
bool amp_patch_proc(struct amp_patch_t *patch, double *buf, struct amp_time_t *time, unsigned int len, struct amp_queue_t *queue);

#endif
