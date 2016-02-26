#ifndef MOD_SAMPLER_H
#define MOD_SAMPLER_H

/*
 * sampler declarations
 */

struct amp_sampler_t;

struct amp_sampler_t *amp_sampler_new(void);
struct amp_sampler_t *amp_sampler_copy(struct amp_sampler_t *sampler);
void amp_sampler_delete(struct amp_sampler_t *sampler);

struct ml_value_t *amp_sampler_make(struct ml_value_t *value, struct ml_env_t *env, char **err);

void amp_sampler_info(struct amp_sampler_t *sampler, struct amp_info_t info);
bool amp_sampler_proc(struct amp_sampler_t *sampler, double *buf, struct amp_time_t *time, unsigned int len);

#endif
