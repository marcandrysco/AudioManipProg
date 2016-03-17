#ifndef MOD_SHOT_H
#define MOD_SHOT_H

/*
 * shot declarations
 */

struct amp_shot_t;

extern const struct amp_module_i amp_shot_iface;

struct amp_shot_t *amp_shot_new(uint16_t dev, uint16_t key, struct amp_module_t module);
struct amp_shot_t *amp_shot_copy(struct amp_shot_t *shot);
void amp_shot_delete(struct amp_shot_t *shot);

struct ml_value_t *amp_shot_make(struct ml_value_t *value, struct ml_env_t *env, char **err);

void amp_shot_info(struct amp_shot_t *shot, struct amp_info_t info);
bool amp_shot_proc(struct amp_shot_t *shot, double *buf, struct amp_time_t *time, unsigned int len);

#endif
