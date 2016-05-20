#ifndef EFX_CHAIN_H
#define EFX_CHAIN_H

/**
 * Chain structure.
 *   @head, tail: The head and tail.
 */
struct amp_chain_t {
	struct amp_chain_inst_t *head, *tail;
};

/**
 * Instance structure.
 *   @effect: The effect.
 *   @prev, next: The previous and next instances.
 */
struct amp_chain_inst_t {
	struct amp_effect_t effect;
	struct amp_chain_inst_t *prev, *next;
};

/*
 * chain declarations
 */
extern const struct amp_effect_i amp_chain_iface;

struct amp_chain_t *amp_chain_new(void);
struct amp_chain_t *amp_chain_copy(struct amp_chain_t *chain);
void amp_chain_delete(struct amp_chain_t *chain);

void amp_chain_prepend(struct amp_chain_t *chain, struct amp_effect_t effect);
void amp_chain_append(struct amp_chain_t *chain, struct amp_effect_t effect);

char *amp_chain_make(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env);

void amp_chain_info(struct amp_chain_t *chain, struct amp_info_t info);
bool amp_chain_proc(struct amp_chain_t *chain, double *buf, struct amp_time_t *time, unsigned int len, struct amp_queue_t *queue);

#endif
