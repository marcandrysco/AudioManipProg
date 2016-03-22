#ifndef INSTR_SPLICE_H
#define INSTR_SPLICE_H

/**
 * Splice structure.
 *   @effect: The effect.
 */
struct amp_splice_t {
	struct amp_effect_t effect;
};

/*
 * splice declarations
 */
extern struct amp_instr_i amp_splice_iface;

struct amp_splice_t *amp_splice_new(struct amp_effect_t effect);
struct amp_splice_t *amp_splice_copy(struct amp_splice_t *splice);
void amp_splice_delete(struct amp_splice_t *splice);

struct ml_value_t *amp_splice_make(struct ml_value_t *value, struct ml_env_t *env, char **err);

void amp_splice_prepend(struct amp_splice_t *splice, struct amp_instr_t instr);
void amp_splice_append(struct amp_splice_t *splice, struct amp_instr_t instr);

void amp_splice_info(struct amp_splice_t *splice, struct amp_info_t info);
void amp_splice_proc(struct amp_splice_t *splice, double **buf, struct amp_time_t *time, unsigned int len);

#endif
