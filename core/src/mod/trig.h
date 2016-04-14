#ifndef MOD_TRIG_H
#define MOD_TRIG_H

/*
 * trigger declarations
 */

struct amp_trig_t;

extern const struct amp_module_i amp_trig_iface;

struct amp_trig_t *amp_trig_new(void);
struct amp_trig_t *amp_trig_copy(struct amp_trig_t *trig);
void amp_trig_delete(struct amp_trig_t *trig);

struct ml_value_t *amp_trig_make(struct ml_value_t *value, struct ml_env_t *env, char **err);

void amp_trig_info(struct amp_trig_t *trig, struct amp_info_t info);
bool amp_trig_proc(struct amp_trig_t *trig, double *buf, struct amp_time_t *time, unsigned int len, struct amp_queue_t *queue);

#endif
