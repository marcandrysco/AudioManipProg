#ifndef EFX_EXPCRUSH_H
#define EFX_EXPCRUSH_H

/*
 * expcrusher declarations
 */

struct amp_expcrush_t *amp_expcrush_new(struct amp_param_t *exps);
struct amp_expcrush_t *amp_expcrush_copy(struct amp_expcrush_t *crush);
void amp_expcrush_delete(struct amp_expcrush_t *crush);

char *amp_expcrush_make(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env);

void amp_expcrush_info(struct amp_expcrush_t *crush, struct amp_info_t info);
bool amp_expcrush_proc(struct amp_expcrush_t *crush, double *buf, struct amp_time_t *time, unsigned int len, struct amp_queue_t *queue);

#endif
