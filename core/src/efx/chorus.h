#ifndef EFX_CHORUS_H
#define EFX_CHORUS_H

/**
 * Chorus structure.
 *   @osc, feedback: The oscillator and feedback parameters.
 *   @ring: The ring buffer.
 */
struct amp_chorus_t {
	struct amp_param_t *osc, *feedback;
	struct dsp_ring_t *ring;
};

/*
 * chorus declarations
 */
extern const struct amp_effect_i amp_chorus_iface;

struct amp_chorus_t *amp_chorus_new(unsigned int len, struct amp_param_t *osc, struct amp_param_t *feedback);
struct amp_chorus_t *amp_chorus_copy(struct amp_chorus_t *chorus);
void amp_chorus_delete(struct amp_chorus_t *chorus);

char *amp_chorus_make(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env);

void amp_chorus_info(struct amp_chorus_t *chorus, struct amp_info_t info);
bool amp_chorus_proc(struct amp_chorus_t *chorus, double *buf, struct amp_time_t *time, unsigned int len, struct amp_queue_t *queue);

#endif
