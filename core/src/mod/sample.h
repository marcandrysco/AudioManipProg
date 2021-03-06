#ifndef MOD_SAMPLE_H
#define MOD_SAMPLE_H

/*
 * sample declarations
 */
struct amp_sample_t;

extern const struct amp_module_i amp_sample_iface;

struct amp_sample_t *amp_sample_new(unsigned int n, double decay);
struct amp_sample_t *amp_sample_copy(struct amp_sample_t *sample);
void amp_sample_delete(struct amp_sample_t *sample);

char *amp_sample_make(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env);

void amp_sample_info(struct amp_sample_t *sample, struct amp_info_t info);
bool amp_sample_proc(struct amp_sample_t *sample, double *buf, struct amp_time_t *time, unsigned int len, struct amp_queue_t *queue);

struct amp_sample_vel_t *amp_sample_vel(struct amp_sample_t *sample);
void amp_sample_inst(struct amp_sample_vel_t *vel, struct amp_file_t *file);

#endif
