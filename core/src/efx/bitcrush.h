#ifndef EFX_BITCRUSH_H
#define EFX_BITCRUSH_H

/*
 * bitcrusher declarations
 */

struct amp_bitcrush_t *amp_bitcrush_new(struct amp_param_t *bits);
struct amp_bitcrush_t *amp_bitcrush_copy(struct amp_bitcrush_t *crush);
void amp_bitcrush_delete(struct amp_bitcrush_t *crush);

struct ml_value_t *amp_bitcrush_make(struct ml_value_t *value, struct ml_env_t *env, char **err);

void amp_bitcrush_info(struct amp_bitcrush_t *crush, struct amp_info_t info);
bool amp_bitcrush_proc(struct amp_bitcrush_t *crush, double *buf, struct amp_time_t *time, unsigned int len, struct amp_queue_t *queue);

#endif
