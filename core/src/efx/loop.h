#ifndef EFX_LOOP_H
#define EFX_LOOP_H

/*
 * loop declarations
 */
extern const struct amp_effect_i amp_loop_iface;

struct amp_loop_t *amp_loop_new(char *path, struct amp_time_t mod, unsigned int len, struct amp_id_t rec);
struct amp_loop_t *amp_loop_copy(struct amp_loop_t *loop);
void amp_loop_delete(struct amp_loop_t *loop);

char *amp_loop_make(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env);

void amp_loop_info(struct amp_loop_t *loop, struct amp_info_t info);
bool amp_loop_proc(struct amp_loop_t *loop, double *buf, struct amp_time_t *time, unsigned int len, struct amp_queue_t *queue);

#endif
