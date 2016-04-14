#ifndef SEQ_TOGGLE_H
#define SEQ_TOGGLE_H

/*
 * toggle declarations
 */
struct amp_toggle_t;

extern const struct amp_seq_i amp_toggle_iface;

struct amp_toggle_t *amp_toggle_new(void);
struct amp_toggle_t *amp_toggle_copy(struct amp_toggle_t *toggle);
void amp_toggle_delete(struct amp_toggle_t *toggle);

char *amp_toggle_make(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env);

void amp_toggle_add(struct amp_toggle_t *toggle, struct amp_id_t id);
void amp_toggle_info(struct amp_toggle_t *toggle, struct amp_info_t info);
void amp_toggle_proc(struct amp_toggle_t *toggle, struct amp_time_t *time, unsigned int len, struct amp_queue_t *queue);

#endif
