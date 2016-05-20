#ifndef MOD_RAMP_H
#define MOD_RAMP_H

/*
 * ramp declarations
 */
struct amp_ramp_t;

extern const struct amp_module_i amp_ramp_iface;

struct amp_ramp_t *amp_ramp_new(struct amp_param_t *freq, unsigned int rate);
struct amp_ramp_t *amp_ramp_copy(struct amp_ramp_t *ramp);
void amp_ramp_delete(struct amp_ramp_t *ramp);

char *amp_ramp_make(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env);

void amp_ramp_info(struct amp_ramp_t *ramp, struct amp_info_t info);
bool amp_ramp_proc(struct amp_ramp_t *ramp, double *buf, struct amp_time_t *time, unsigned int len, struct amp_queue_t *queue);

struct amp_ramp_vel_t *amp_ramp_vel(struct amp_ramp_t *ramp);
void amp_ramp_inst(struct amp_ramp_vel_t *vel, struct amp_file_t *file);

#endif
