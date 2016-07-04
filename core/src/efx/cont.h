#ifndef MOD_CONT_H
#define MOD_CONT_H

/*
 * continue declarations
 */
struct amp_cont_t;

extern const struct amp_effect_i amp_cont_iface;

struct amp_cont_t *amp_cont_new(void);
struct amp_cont_t *amp_cont_copy(struct amp_cont_t *cont);
void amp_cont_delete(struct amp_cont_t *cont);

char *amp_cont_make(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env);

void amp_cont_info(struct amp_cont_t *cont, struct amp_info_t info);
bool amp_cont_proc(struct amp_cont_t *cont, double *buf, struct amp_time_t *time, unsigned int len, struct amp_queue_t *queue);

#endif
