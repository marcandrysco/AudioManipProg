#ifndef EFX_MATH_H
#define EFX_MATH_H

/**
 * Math function enumerator.
 *   @amp_math_exp_v: Exponential.
 *   @amp_math_hz2sec_v: Hertz to seconds.
 */
enum amp_math_e {
	amp_math_exp_v,
	amp_math_hz2sec_v,
};


/*
 * math declarations
 */
struct amp_math_t *amp_math_new(enum amp_math_e type, struct amp_param_t *param, unsigned int rate);
struct amp_math_t *amp_math_copy(struct amp_math_t *math);
void amp_math_delete(struct amp_math_t *math);

char *amp_exp_make(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env);
char *amp_hz2sec_make(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env);

void amp_math_info(struct amp_math_t *math, struct amp_info_t info);
bool amp_math_proc(struct amp_math_t *math, double *buf, struct amp_time_t *time, unsigned int len, struct amp_queue_t *queue);

#endif
