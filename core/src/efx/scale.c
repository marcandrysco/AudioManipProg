#include "../common.h"

/*
 * global variables
 */
const struct amp_effect_i amp_scale_iface = {
	(amp_info_f)amp_scale_info,
	(amp_effect_f)amp_scale_proc,
	(amp_copy_f)amp_scale_copy,
	(amp_delete_f)amp_scale_delete
};


/**
 * Create a scale effect.
 *   @inlo: Consumed. The input low.
 *   @inhi: Consumed. The input high.
 *   @outlo: Consumed. The output low.
 *   @outhi: Consumed. The output high.
 *   &returns: The scale.
 */
struct amp_scale_t *amp_scale_new(struct amp_param_t *inlo, struct amp_param_t *inhi, struct amp_param_t *outlo, struct amp_param_t *outhi)
{
	struct amp_scale_t *scale;

	scale = malloc(sizeof(struct amp_scale_t));
	scale->inlo = inlo;
	scale->inhi = inhi;
	scale->outlo = outlo;
	scale->outhi = outhi;
	scale->fast = amp_param_isfast(scale->inlo) && amp_param_isfast(scale->inhi) && amp_param_isfast(scale->outlo) && amp_param_isfast(scale->outhi);

	return scale;
}

/**
 * Copy a scale effect.
 *   @scale: The original scale.
 *   &returns: The copied scale.
 */
struct amp_scale_t *amp_scale_copy(struct amp_scale_t *scale)
{
	return amp_scale_new(amp_param_copy(scale->inlo), amp_param_copy(scale->inhi), amp_param_copy(scale->outlo), amp_param_copy(scale->outhi));
}

/**
 * Delete a scale effect.
 *   @scale: The scale.
 */
void amp_scale_delete(struct amp_scale_t *scale)
{
	amp_param_delete(scale->inlo);
	amp_param_delete(scale->inhi);
	amp_param_delete(scale->outlo);
	amp_param_delete(scale->outhi);
	free(scale);
}


/**
 * Create a scale from a value.
 *   @ret: Ref. The returned value.
 *   @value: The value.
 *   @env: The environment.
 *   &returns: Error.
 */
char *amp_scale_make(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env)
{
#define onexit
	struct amp_param_t *inlo, *inhi, *outlo, *outhi;

	chkfail(amp_match_unpack(value, "(P,P,P,P)", &inlo, &inhi, &outlo, &outhi));

	*ret = amp_pack_effect((struct amp_effect_t){ amp_scale_new(inlo, inhi, outlo, outhi), &amp_scale_iface });
	return NULL;
#undef onexit
}


/**
 * Handle information on a scale.
 *   @scale: The scale.
 *   @info: The information.
 */
void amp_scale_info(struct amp_scale_t *scale, struct amp_info_t info)
{
	amp_param_info(scale->inlo, info);
	amp_param_info(scale->inhi, info);
	amp_param_info(scale->outlo, info);
	amp_param_info(scale->outhi, info);
}

/**
 * Process a scale.
 *   @scale: The scale.
 *   @buf: The buffer.
 *   @time: The time.
 *   @len: The length.
 *   &returns: The continuation flag.
 */
bool amp_scale_proc(struct amp_scale_t *scale, double *buf, struct amp_time_t *time, unsigned int len)
{
	unsigned int i;

	if(scale->fast) {
		double add, mul, div;

		// m = Oh - Ol
		// a = Ol * Ih - Oh * Il
		// d = Ih - Il
		mul = scale->outhi->flt - scale->outlo->flt;
		add = scale->outlo->flt * scale->inhi->flt - scale->outhi->flt * scale->inlo->flt;
		div = 1.0 / (scale->inhi->flt - scale->inlo->flt);

		for(i = 0; i < len; i++)
			buf[i] = div * (mul * buf[i] + add);
	}

	return false;
}
