#ifndef EFX_INJECT_H
#define EFX_INJECT_H

/**
 * Inject structure.
 *   @seq: The sequencer.
 *   @instr: The instrument.
 */
struct amp_inject_t {
	struct amp_seq_t seq;
	struct amp_instr_t instr;
};

/*
 * inject declarations
 */
extern const struct amp_instr_i amp_inject_iface;

struct amp_inject_t *amp_inject_new(struct amp_seq_t seq, struct amp_instr_t instr);
struct amp_inject_t *amp_inject_copy(struct amp_inject_t *inject);
void amp_inject_delete(struct amp_inject_t *inject);

char *amp_inject_make(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env);

void amp_inject_info(struct amp_inject_t *inject, struct amp_info_t info);
bool amp_inject_proc(struct amp_inject_t *inject, double **buf, struct amp_time_t *time, unsigned int len, struct amp_queue_t *queue);

#endif
