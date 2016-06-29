#include "../common.h"


/**
 * Bitcrusher structure.
 *   @type: The type.
 *   @bits: The bits.
 */
struct amp_crush_t {
	enum amp_crush_e type;
	struct amp_param_t *bits;
};


/*
 * global variables
 */
const struct amp_effect_i amp_crush_iface = {
	(amp_info_f)amp_crush_info,
	(amp_effect_f)amp_crush_proc,
	(amp_copy_f)amp_crush_copy,
	(amp_delete_f)amp_crush_delete
};


/**
 * Create a crusher effect.
 *   @bits: The bits.
 *   &returns: The crusher.
 */
struct amp_crush_t *amp_crush_new(enum amp_crush_e type, struct amp_param_t *bits)
{
	struct amp_crush_t *crush;

	crush = malloc(sizeof(struct amp_crush_t));
	crush->bits = bits;

	return crush;
}

/**
 * Copy a crusher effect.
 *   @crush: The original crusher.
 *   &returns: The copied crusher.
 */
struct amp_crush_t *amp_crush_copy(struct amp_crush_t *crush)
{
	return amp_crush_new(crush->type, amp_param_copy(crush->bits));
}

/**
 * Delete a crusher effect.
 *   @crush: The crusher.
 */
void amp_crush_delete(struct amp_crush_t *crush)
{
	amp_param_delete(crush->bits);
	free(crush);
}


/**
 * Create a crusher from a value.
 *   @ret: Ref. The returned value.
 *   @type: The crusher type.
 *   @value: The value.
 *   &returns: Error.
 */
char *amp_crush_make(struct ml_value_t **ret, enum amp_crush_e type, struct ml_value_t *value)
{
#define onexit
	struct amp_param_t *bits;

	chkfail(amp_match_unpack(value, "P", &bits));

	*ret = amp_pack_effect((struct amp_effect_t){ amp_crush_new(type, bits), &amp_crush_iface });
	return NULL;
#undef onexit
}
char *amp_bitcrush_make(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env)
{
	return amp_crush_make(ret, amp_crush_bit_v, value);
}
char *amp_expcrush_make(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env)
{
	return amp_crush_make(ret, amp_crush_exp_v, value);
}


/**
 * Handle information on a crusher.
 *   @crush: The crusher.
 *   @info: The information.
 */

void amp_crush_info(struct amp_crush_t *crush, struct amp_info_t info)
{
	amp_param_info(crush->bits, info);
}

/**
 * Process a crusher.
 *   @crush: The crusher.
 *   @buf: The buffer.
 *   @time: The time.
 *   @len: The length.
 *   @queue: The action queue.
 *   &returns: The continuation flag.
 */
bool amp_crush_proc(struct amp_crush_t *crush, double *buf, struct amp_time_t *time, unsigned int len, struct amp_queue_t *queue)
{
	bool cont = false;
	unsigned int i;

	switch(crush->type) {
	case amp_crush_bit_v:
		if(!amp_param_isfast(crush->bits)) {
			double t, inc[len];

			cont = amp_param_proc(crush->bits, inc, time, len, queue);

			for(i = 0; i < len; i++) {
				t = dsp_pow_f(2.0f, fmax(inc[i], 1e-10));
				buf[i] = round(buf[i] * t) / t;
			}
		}
		else {
			double inc = dsp_pow_f(2.0f, crush->bits->flt);

			for(i = 0; i < len; i++)
				buf[i] = round(buf[i] * inc) / inc;
		}
		break;

	case amp_crush_exp_v:
		if(!amp_param_isfast(crush->bits)) {
			double bits[len];

			cont = amp_param_proc(crush->bits, bits, time, len, queue);

			for(i = 0; i < len; i++)
				buf[i] = amp_expcrush(buf[i], bits[i]);
		}
		else {
			double bits = crush->bits->flt;

			for(i = 0; i < len; i++)
				buf[i] = amp_expcrush(buf[i], bits);
		}
		break;

	case amp_crush_freq_v:
		break;
	}

	return cont;
}
