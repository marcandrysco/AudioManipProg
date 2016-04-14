#ifndef MOD_OSC_H
#define MOD_OSC_H

/**
 * Oscillator type.
 *   @amp_osc_sine_e: Sine.
 *   @amp_osc_triangle_e: Triangle.
 *   @amp_osc_square_e: Square.
 */

enum amp_osc_e {
	amp_osc_sine_e,
	amp_osc_tri_e,
	amp_osc_square_e
};


/*
 * oscillator declarations
 */

struct amp_osc_t;

extern const struct amp_module_i amp_osc_iface;

struct amp_osc_t *amp_osc_new(enum amp_osc_e type, struct amp_param_t *freq, struct amp_param_t *warp, unsigned int rate);
struct amp_osc_t *amp_osc_copy(struct amp_osc_t *osc);
void amp_osc_delete(struct amp_osc_t *osc);

struct ml_value_t *amp_osc_make(struct ml_value_t *value, struct ml_env_t *env, char **err);

void amp_osc_info(struct amp_osc_t *osc, struct amp_info_t info);
bool amp_osc_proc(struct amp_osc_t *osc, double *buf, struct amp_time_t *time, unsigned int len, struct amp_queue_t *queue);

int amp_osc_type(const char *str);

#endif
