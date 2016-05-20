#ifndef EFX_GEN_H
#define EFX_GEN_H

/*
 * generator declarations
 */
struct amp_gen_t *amp_gen_new(struct amp_module_t module);
struct amp_gen_t *amp_gen_copy(struct amp_gen_t *gen);
void amp_gen_delete(struct amp_gen_t *gen);

char *amp_gen_make(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env);

void amp_gen_info(struct amp_gen_t *gen, struct amp_info_t info);
bool amp_gen_proc(struct amp_gen_t *gen, double *buf, struct amp_time_t *time, unsigned int len, struct amp_queue_t *queue);

#endif
