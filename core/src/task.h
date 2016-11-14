#ifndef TASK_H
#define TASK_H

/*
 * asynchronous declarations
 */
struct amp_async_t;

struct amp_async_t *amp_async_new(unsigned int nbytes);
void amp_async_delete(struct amp_async_t *async);

void amp_async_reset(struct amp_async_t *async);
void amp_async_write(struct amp_async_t *async, const void *buf, unsigned int nbytes);
bool amp_async_read(struct amp_async_t *async, void *buf, unsigned int nbytes);

#endif
