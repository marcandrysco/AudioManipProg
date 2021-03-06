#ifndef MULREC_H
#define MULREC_H

/*
 * multitrack recorder declarations
 */
struct web_mulrec_t;

struct web_mulrec_t *web_mulrec_new(const char *id);
void web_mulrec_delete(struct web_mulrec_t *rec);

void web_mulrec_info(struct web_mulrec_t *rec, struct amp_info_t info);
bool web_mulrec_proc(struct web_mulrec_t *rec, double **buf, struct amp_time_t *time, unsigned int len, struct amp_queue_t *queue);

void web_mulrec_print(struct web_mulrec_t *rec, struct io_file_t file);

char *web_mulrec_load(struct web_mulrec_t *rec);
char *web_mulrec_save(struct web_mulrec_t *rec);

#endif
