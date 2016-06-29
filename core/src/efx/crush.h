#ifndef EFX_CRUSH_H
#define EFX_CRUSH_H

/**
 * Crusher type enumerator.
 *   @amp_crush_bit_v: Bitcrusher.
 *   @amp_crush_exp_v: Exponential bitcrusher.
 *   @amp_crush_freq_v: Freqency crusher.
 */
enum amp_crush_e {
	amp_crush_bit_v,
	amp_crush_exp_v,
	amp_crush_freq_v
};

/*
 * crusher declarations
 */
struct amp_crush_t *amp_crush_new(enum amp_crush_e type, struct amp_param_t *bits);
struct amp_crush_t *amp_crush_copy(struct amp_crush_t *crush);
void amp_crush_delete(struct amp_crush_t *crush);

char *amp_crush_make(struct ml_value_t **ret, enum amp_crush_e type, struct ml_value_t *value);
char *amp_bitcrush_make(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env);
char *amp_expcrush_make(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env);

void amp_crush_info(struct amp_crush_t *crush, struct amp_info_t info);
bool amp_crush_proc(struct amp_crush_t *crush, double *buf, struct amp_time_t *time, unsigned int len, struct amp_queue_t *queue);


/**
 * Process a single number.
 *   @x: The input.
 *   @bits: The number of bits
 *   &returns: The processed number.
 */
static inline float amp_expcrush(float x, float bits)
{
	if(x > 0.0)
		return expf(roundf(bits * logf(x)) / bits);
	else if(x < 0.0)
		return -expf(roundf(bits * logf(-x)) / bits);
	else
		return 0;
}

#endif
