#ifndef EFX_COMP_H
#define EFX_COMP_H

/*
 * comp declarations
 */
struct amp_comp_t *amp_comp_new(struct amp_param_t *atk, struct amp_param_t *rel, struct amp_param_t *thresh, struct amp_param_t *ratio, double rate);
struct amp_comp_t *amp_comp_copy(struct amp_comp_t *comp);
void amp_comp_delete(struct amp_comp_t *comp);

struct ml_value_t *amp_comp_make(struct ml_value_t *value, struct ml_env_t *env, char **err);

void amp_comp_info(struct amp_comp_t *comp, struct amp_info_t info);
bool amp_comp_proc(struct amp_comp_t *comp, double *buf, struct amp_time_t *time, unsigned int len, struct amp_queue_t *queue);

#endif
