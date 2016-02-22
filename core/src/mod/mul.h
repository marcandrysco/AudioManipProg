#ifndef MOD_MUL_H
#define MOD_MUL_H

/*
 * multiply declarations
 */

struct amp_mul_t;

extern const struct amp_module_i amp_mul_iface;

struct amp_mul_t *amp_mul_new(struct amp_param_t *left, struct amp_param_t *right);
struct amp_mul_t *amp_mul_copy(struct amp_mul_t *mul);
void amp_mul_delete(struct amp_mul_t *mul);

struct ml_value_t *amp_mul_make(struct ml_value_t *value, struct ml_env_t *env, char **err);

void amp_mul_info(struct amp_mul_t *mul, struct amp_info_t info);
bool amp_mul_proc(struct amp_mul_t *mul, double *buf, struct amp_time_t *time, unsigned int len);

#endif
