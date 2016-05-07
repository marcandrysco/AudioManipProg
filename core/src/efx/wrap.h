#ifndef EFX_WRAP_H
#define EFX_WRAP_H

/**
 * Wrap structure.
 *   @limit: The limit.
 */
struct amp_wrap_t {
	struct amp_param_t *limit;
};

/*
 * wrap declarations
 */
extern const struct amp_effect_i amp_wrap_iface;

struct amp_wrap_t *amp_wrap_new(struct amp_param_t *limit);
struct amp_wrap_t *amp_wrap_copy(struct amp_wrap_t *wrap);
void amp_wrap_delete(struct amp_wrap_t *wrap);

char *amp_wrap_make(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env);

void amp_wrap_info(struct amp_wrap_t *wrap, struct amp_info_t info);
bool amp_wrap_proc(struct amp_wrap_t *wrap, double *buf, struct amp_time_t *time, unsigned int len, struct amp_queue_t *queue);

#endif
