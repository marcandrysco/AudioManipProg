#ifndef SEQ_ENABLE_H
#define SEQ_ENABLE_H

/*
 * enable declarations
 */
struct amp_enable_t;

extern const struct amp_seq_i amp_enable_iface;

struct amp_enable_t *amp_enable_new(struct amp_id_t id, struct amp_seq_t seq);
struct amp_enable_t *amp_enable_copy(struct amp_enable_t *enable);
void amp_enable_delete(struct amp_enable_t *enable);

char *amp_enable_make(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env);

void amp_enable_info(struct amp_enable_t *enable, struct amp_info_t info);
void amp_enable_proc(struct amp_enable_t *enable, struct amp_time_t *time, unsigned int len, struct amp_queue_t *queue);

#endif
