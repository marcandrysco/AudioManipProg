#ifndef MOD_WARP_H
#define MOD_WARP_H

/*
 * warp declarations
 */
struct amp_warp_t;

extern const struct amp_module_i amp_warp_iface;

struct amp_warp_t *amp_warp_new(struct amp_module_t phase, struct amp_param_t *dist);
struct amp_warp_t *amp_warp_copy(struct amp_warp_t *warp);
void amp_warp_delete(struct amp_warp_t *warp);

char *amp_warp_make(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env);

void amp_warp_info(struct amp_warp_t *warp, struct amp_info_t info);
bool amp_warp_proc(struct amp_warp_t *warp, double *buf, struct amp_time_t *time, unsigned int len, struct amp_queue_t *queue);

struct amp_warp_vel_t *amp_warp_vel(struct amp_warp_t *warp);
void amp_warp_inst(struct amp_warp_vel_t *vel, struct amp_file_t *file);

#endif
