#ifndef LOOP_H
#define LOOP_H

/*
 * looper declarations
 */
struct web_loop_t *web_loop_new(struct web_serv_t *serv, const char *id);
void web_loop_delete(struct web_loop_t *loop);

void web_loop_info(struct web_loop_t *loop, struct amp_info_t info);
bool web_loop_proc(struct web_loop_t *loop, double *buf, struct amp_time_t *time, unsigned int len, struct amp_queue_t *queue);

void web_loop_print(struct web_loop_t *loop, struct io_file_t file);
struct io_chunk_t web_loop_chunk(struct web_loop_t *loop);

#endif
