#ifndef MOD_ADSR_H
#define MOD_ADSR_H

/*
 * adsr declarations
 */

struct amp_adsr_t;

extern const struct amp_module_i amp_adsr_iface;

struct amp_adsr_t *amp_adsr_new(double min, double max, double atk, double decay, double sus, double rel, unsigned int rate);
struct amp_adsr_t *amp_adsr_copy(struct amp_adsr_t *adsr);
void amp_adsr_delete(struct amp_adsr_t *adsr);

struct ml_value_t *amp_adsr_make(struct ml_value_t *value, struct ml_env_t *env, char **err);

void amp_adsr_info(struct amp_adsr_t *adsr, struct amp_info_t info);
bool amp_adsr_proc(struct amp_adsr_t *adsr, double *buf, struct amp_time_t *time, unsigned int len);

#endif
