#ifndef MACH_H
#define MACH_H

/**
 * Machine type enumerator.
 *   @web_mach_drum_v: Drum.
 *   @web_mach_note_v: Note.
 *   @web_mach_ctrl_v: Control..
 */
enum web_mach_e {
	web_mach_drum_v,
	web_mach_note_v,
	web_mach_ctrl_v
};

/*
 * machine declarations
 */
struct web_mach_t;
struct web_mach_inst_t;

struct web_mach_t *web_mach_new(const char *id);
char *web_mach_make(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env);
void web_mach_delete(struct web_mach_t *mach);

void web_mach_info(struct web_mach_t *mach, struct amp_info_t info);
bool web_mach_proc(struct web_mach_t *mach, struct amp_time_t *time, unsigned int len, struct amp_queue_t *queue);

unsigned int web_mach_len(struct web_mach_t *mach);
void web_mach_set(struct web_mach_t *mach, unsigned int sel, struct web_mach_inst_t *inst, unsigned int off, uint16_t key, uint16_t vel);

void web_mach_print(struct web_mach_t *mach, struct io_file_t file);
bool web_mach_req(struct web_mach_t *mach, struct http_args_t *args, struct json_t *json);

char *web_mach_save(struct web_mach_t *mach);
char *web_mach_load(struct web_mach_t *mach);

const char *web_mach_type(enum web_mach_e type);
enum web_mach_e web_mach_enum(const char *str);

#endif
