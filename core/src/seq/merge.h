#ifndef SEQ_MERGE_H
#define SEQ_MERGE_H

/*
 * merge declarations
 */
extern const struct amp_seq_i amp_merge_iface;

struct amp_merge_t *amp_merge_new(void);
struct amp_merge_t *amp_merge_copy(struct amp_merge_t *merge);
void amp_merge_delete(struct amp_merge_t *merge);

char *amp_merge_make(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env);

void amp_merge_prepend(struct amp_merge_t *merge, struct amp_seq_t seq);
void amp_merge_append(struct amp_merge_t *merge, struct amp_seq_t seq);

void amp_merge_info(struct amp_merge_t *merge, struct amp_info_t info);
void amp_merge_proc(struct amp_merge_t *merge, struct amp_time_t *time, unsigned int len, struct amp_queue_t *queue);

#endif
