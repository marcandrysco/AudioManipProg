#ifndef INSTR_MIXER_H
#define INSTR_MIXER_H

/*
 * mixer declarations
 */

struct amp_mixer_t;

extern struct amp_instr_i amp_mixer_iface;

struct amp_mixer_t *amp_mixer_new(void);
struct amp_mixer_t *amp_mixer_copy(struct amp_mixer_t *mixer);
void amp_mixer_delete(struct amp_mixer_t *mixer);

struct ml_value_t *amp_mixer_make(struct ml_value_t *value, struct ml_env_t *env, char **err);

void amp_mixer_prepend(struct amp_mixer_t *mixer, struct amp_instr_t instr);
void amp_mixer_append(struct amp_mixer_t *mixer, struct amp_instr_t instr);

void amp_mixer_info(struct amp_mixer_t *mixer, struct amp_info_t info);
void amp_mixer_proc(struct amp_mixer_t *mixer, double **buf, struct amp_time_t *time, unsigned int len);

#endif
