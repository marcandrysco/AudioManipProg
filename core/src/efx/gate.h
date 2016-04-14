#ifndef EFX_GATE_H
#define EFX_GATE_H

/**
 * Gate structure.
 *   @left, right: The left and right effects.
 */
struct amp_gate_t {
	struct amp_effect_t left, right;
};

/*
 * gate declarations
 */
extern const struct amp_effect_i amp_gate_iface;

struct amp_gate_t *amp_gate_new(struct amp_effect_t left, struct amp_effect_t right);
struct amp_gate_t *amp_gate_copy(struct amp_gate_t *gate);
void amp_gate_delete(struct amp_gate_t *gate);

char *amp_gate_make(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env);

void amp_gate_info(struct amp_gate_t *gate, struct amp_info_t info);
bool amp_gate_proc(struct amp_gate_t *gate, double *buf, struct amp_time_t *time, unsigned int len, struct amp_queue_t *queue);

#endif
