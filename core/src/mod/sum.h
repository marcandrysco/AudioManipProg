#ifndef MOD_SUM_H
#define MOD_SUM_H

/*
 * sum declarations
 */

struct amp_sum_t;

extern const struct amp_module_i amp_sum_iface;

struct amp_sum_t *amp_sum_new(void);
struct amp_sum_t *amp_sum_copy(struct amp_sum_t *sum);
void amp_sum_delete(struct amp_sum_t *sum);

struct ml_value_t *amp_sum_make(struct ml_value_t *value, struct ml_env_t *env, char **err);

void amp_sum_prepend(struct amp_sum_t *sum, struct amp_param_t *param);
void amp_sum_append(struct amp_sum_t *sum, struct amp_param_t *param);

void amp_sum_info(struct amp_sum_t *sum, struct amp_info_t info);
bool amp_sum_proc(struct amp_sum_t *sum, double *buf, struct amp_time_t *time, unsigned int len);

#endif
