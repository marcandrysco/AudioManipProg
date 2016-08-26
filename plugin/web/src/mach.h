#ifndef MACH_H
#define MACH_H

/*
 * machine declarations
 */
struct web_mach_t *web_mach_new(const char *id);
char *web_mach_make(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env);
void web_mach_delete(struct web_mach_t *mach);

void web_mach_info(struct web_mach_t *mach, struct amp_info_t info);
bool web_mach_proc(struct web_mach_t *mach, struct amp_time_t *time, unsigned int len, struct amp_queue_t *queue);

unsigned int web_mach_len(struct web_mach_t *mach);
void web_mach_set(struct web_mach_t *mach, unsigned int idx, uint16_t key, uint16_t vel);

void web_mach_print(struct web_mach_t *mach, struct io_file_t file);
struct io_chunk_t web_mach_chunk(struct web_mach_t *mach);

void web_mach_save(struct web_mach_t *mach, const char *path);
char *web_mach_load(struct web_mach_t *mach, const char *path);

bool web_mach_req(struct web_mach_t *mach, const char *path, struct http_args_t *args);

#endif
