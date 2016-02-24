#ifndef SEQ_MERGE_H
#define SEQ_MERGE_H

/*
 * merge declarations
 */

struct amp_merge_t *amp_merge_new(void);
struct amp_merge_t *amp_merge_copy(struct amp_merge_t *merge);
void amp_merge_delete(struct amp_merge_t *merge);

void amp_merge_info(struct amp_merge_t *merge, struct amp_info_t info);
void amp_merge_proc(struct amp_merge_t *merge, struct amp_queue_t *queue, struct amp_time_t *time, unsigned int len);

#endif
