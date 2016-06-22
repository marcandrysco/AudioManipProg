#ifndef SEQ_REPEAT_H
#define SEQ_REPEAT_H

/*
 * repeat declarations
 */

struct amp_repeat_t;

extern const struct amp_seq_i amp_repeat_iface;

struct amp_repeat_t *amp_repeat_new(int off, int len, struct amp_seq_t seq);
struct amp_repeat_t *amp_repeat_copy(struct amp_repeat_t *repeat);
void amp_repeat_delete(struct amp_repeat_t *repeat);

char *amp_repeat_make(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env);

void amp_repeat_info(struct amp_repeat_t *repeat, struct amp_info_t info);
void amp_repeat_proc(struct amp_repeat_t *repeat, struct amp_time_t *time, unsigned int len, struct amp_queue_t *queue);

#endif
