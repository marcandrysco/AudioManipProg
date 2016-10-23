#ifndef TRAIN_H
#define TRAIN_H

/*
 * trainer declarations
 */
struct web_train_t *web_train_new(struct web_serv_t *serv, const char *id);
void web_train_delete(struct web_train_t *train);

void web_train_info(struct web_train_t *train, struct amp_info_t info);
bool web_train_proc(struct web_train_t *train, struct amp_time_t *time, unsigned int len, struct amp_queue_t *queue);

#endif
