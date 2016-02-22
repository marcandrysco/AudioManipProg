#ifndef EFX_SYNTH_H
#define EFX_SYNTH_H

/*
 * synthesizer declarations
 */

struct amp_synth_t;

extern struct amp_effect_i amp_synth_iface;

struct amp_synth_t *amp_synth_new(uint16_t dev, unsigned int n, struct amp_module_t module);
struct amp_synth_t *amp_synth_copy(struct amp_synth_t *synth);
void amp_synth_delete(struct amp_synth_t *synth);

struct ml_value_t *amp_synth_make(struct ml_value_t *value, struct ml_env_t *env, char **err);

void amp_synth_info(struct amp_synth_t *synth, struct amp_info_t info);
void amp_synth_proc(struct amp_synth_t *synth, double *buf, struct amp_time_t *time, unsigned int len);

#endif
