#ifndef SEQ_PIANO_H
#define SEQ_PIANO_H

/*
 * piano declarations
 */
struct amp_piano_t *amp_piano_new(uint16_t dev, uint16_t pedal, uint16_t n, uint16_t len);
struct amp_piano_t *amp_piano_copy(struct amp_piano_t *piano);
void amp_piano_delete(struct amp_piano_t *piano);

void amp_piano_info(struct amp_piano_t *piano, struct amp_info_t info);
void amp_piano_proc(struct amp_piano_t *piano, struct amp_time_t *time, unsigned int len, struct amp_queue_t *queue);

#endif
