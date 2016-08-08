#ifndef MOD_FOLD_H
#define MOD_FOLD_H

/**
 * Fold operation enumerator.
 *   @amp_fold_add_v: Addition.
 *   @amp_fold_mul_v: Multiplication..
 */
enum amp_fold_e {
	amp_fold_add_v,
	amp_fold_mul_v
};


/*
 * fold declarations
 */
struct amp_fold_t;

extern const struct amp_module_i amp_fold_iface;

struct amp_fold_t *amp_fold_new(enum amp_fold_e type);
struct amp_fold_t *amp_fold_copy(struct amp_fold_t *fold);
void amp_fold_delete(struct amp_fold_t *fold);

char *amp_fold_make(struct ml_value_t **ret, struct ml_value_t *value, enum amp_fold_e type);
char *amp_add_make(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env);
char *amp_mul_make(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env);

void amp_fold_prepend(struct amp_fold_t *fold, struct amp_param_t *param);
void amp_fold_append(struct amp_fold_t *fold, struct amp_param_t *param);

void amp_fold_info(struct amp_fold_t *fold, struct amp_info_t info);
bool amp_fold_proc(struct amp_fold_t *fold, double *buf, struct amp_time_t *time, unsigned int len, struct amp_queue_t *queue);

#endif
