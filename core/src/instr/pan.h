#ifndef INSTR_PAN_H
#define INSTR_PAN_H

/*
 * pan declarations
 */

struct amp_pan_t;

extern struct amp_instr_i amp_pan_iface;

struct amp_pan_t *amp_pan_new(void);
struct amp_pan_t *amp_pan_copy(struct amp_pan_t *pan);
void amp_pan_delete(struct amp_pan_t *pan);

struct ml_value_t *amp_pan_make(struct ml_value_t *value, struct ml_env_t *env, char **err);

void amp_pan_info(struct amp_pan_t *pan, struct amp_info_t info);
void amp_pan_proc(struct amp_pan_t *pan, double **buf, struct amp_time_t *time, unsigned int len);

#endif
