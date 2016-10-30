#ifndef AUDIT_H
#define AUDIT_H

/*
 * audit declarations
 */
struct web_audit_t *web_audit_new(struct web_serv_t *serv, const char *id);
char *web_audit_make(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env);
void web_audit_delete(struct web_audit_t *audit);

void web_audit_info(struct web_audit_t *audit, struct amp_info_t info);
bool web_audit_proc(struct web_audit_t *audit, double *buf, struct amp_time_t *time, unsigned int len, struct amp_queue_t *queue);

void web_audit_print(struct web_audit_t *audit, struct io_file_t file);
struct io_chunk_t web_audit_chunk(struct web_audit_t *audit);

#endif
