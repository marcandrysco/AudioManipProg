#ifndef SEQ_REPEAT_H
#define SEQ_REPEAT_H

/*
 * repeat declarations
 */

struct amp_repeat_t;

struct amp_repeat_t *amp_repeat_new(void);
struct amp_repeat_t *amp_repeat_copy(struct amp_repeat_t *repeat);
void amp_repeat_delete(struct amp_repeat_t *repeat);

void amp_repeat_info(struct amp_repeat_t *repeat, struct amp_info_t info);
void amp_repeat_proc(struct amp_repeat_t *repeat, struct amp_queue_t *queue, struct amp_time_t *time, unsigned int len);

#endif
