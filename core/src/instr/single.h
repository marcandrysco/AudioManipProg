#ifndef INSTR_SINGLE_H
#define INSTR_SINGLE_H

/**
 * Single structure.
 *   @idx: The selection index.
 *   @effect: The effect.
 */
struct amp_single_t {
	unsigned int idx;
	struct amp_effect_t effect;
};

/*
 * single declarations
 */
extern struct amp_instr_i amp_single_iface;

struct amp_single_t *amp_single_new(unsigned int idx, struct amp_effect_t effect);
struct amp_single_t *amp_single_copy(struct amp_single_t *single);
void amp_single_delete(struct amp_single_t *single);

struct ml_value_t *amp_single_make(struct ml_value_t *value, struct ml_env_t *env, char **err);

void amp_single_prepend(struct amp_single_t *single, struct amp_instr_t instr);
void amp_single_append(struct amp_single_t *single, struct amp_instr_t instr);

void amp_single_info(struct amp_single_t *single, struct amp_info_t info);
void amp_single_proc(struct amp_single_t *single, double **buf, struct amp_time_t *time, unsigned int len);

#endif
