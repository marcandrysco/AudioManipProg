#ifndef MOD_NOISE_H
#define MOD_NOISE_H

/*
 * noise declarations
 */

struct amp_noise_t;

extern const struct amp_module_i amp_noise_iface;

struct amp_noise_t *amp_noise_new(void);
struct amp_noise_t *amp_noise_copy(struct amp_noise_t *noise);
void amp_noise_delete(struct amp_noise_t *noise);

struct ml_value_t *amp_noise_make(struct ml_value_t *value, struct ml_env_t *env, char **err);

void amp_noise_info(struct amp_noise_t *noise, struct amp_info_t info);
bool amp_noise_proc(struct amp_noise_t *noise, double *buf, struct amp_time_t *time, unsigned int len);

#endif
