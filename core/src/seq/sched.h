#ifndef SEQ_SCHED_H
#define SEQ_SCHED_H

/*
 * schedule declarations
 */
struct amp_sched_t;

extern const struct amp_seq_i amp_sched_iface;

struct amp_sched_t *amp_sched_new(void);
struct amp_sched_t *amp_sched_copy(struct amp_sched_t *sched);
void amp_sched_delete(struct amp_sched_t *sched);

char *amp_sched_make(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env);

void amp_sched_add(struct amp_sched_t *sched, struct amp_time_t time, struct amp_event_t event);

void amp_sched_info(struct amp_sched_t *sched, struct amp_info_t info);
void amp_sched_proc(struct amp_sched_t *sched, struct amp_time_t *time, unsigned int len, struct amp_queue_t *queue);

#endif
