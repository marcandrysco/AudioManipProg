#ifndef EFX_OCTAVE_H
#define EFX_OCTAVE_H

/*
 * octave declarations
 */
extern const struct amp_effect_i amp_octave_iface;

struct amp_octave_t *amp_octave_new(int mode, double thresh, unsigned int rate);
struct amp_octave_t *amp_octave_copy(struct amp_octave_t *octave);
void amp_octave_delete(struct amp_octave_t *octave);

char *amp_octave_make(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env);

void amp_octave_info(struct amp_octave_t *octave, struct amp_info_t info);
bool amp_octave_proc(struct amp_octave_t *octave, double *buf, struct amp_time_t *time, unsigned int len, struct amp_queue_t *queue);

#endif
