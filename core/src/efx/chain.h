#ifndef EFX_CHAIN_H
#define EFX_CHAIN_H

/*
 * chain declarations
 */

struct amp_chain_t *amp_chain_new(void);
struct amp_chain_t *amp_chain_copy(struct amp_chain_t *chain);
void amp_chain_delete(struct amp_chain_t *chain);

void amp_chain_prepend(struct amp_chain_t *chain, double mix, struct amp_effect_t effect);
void amp_chain_append(struct amp_chain_t *chain, double mix, struct amp_effect_t effect);

struct ml_value_t *amp_chain_make(struct ml_value_t *value, struct ml_env_t *env, char **err);

void amp_chain_info(struct amp_chain_t *chain, struct amp_info_t info);
void amp_chain_proc(struct amp_chain_t *chain, double *buf, struct amp_time_t *time, unsigned int len);

#endif
