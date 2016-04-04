#include "../common.h"

/*
 * global variables
 */
const struct amp_effect_i amp_mix_iface = {
	(amp_info_f)amp_mix_info,
	(amp_effect_f)amp_mix_proc,
	(amp_copy_f)amp_mix_copy,
	(amp_delete_f)amp_mix_delete
};


/**
 * Create a mix effect.
 *   @ratio: Consumed. The ratio.
 *   @effect: Consumed. The effect.
 *   &returns: The mix.
 */
struct amp_mix_t *amp_mix_new(struct amp_param_t *ratio, struct amp_effect_t effect)
{
	struct amp_mix_t *mix;

	mix = malloc(sizeof(struct amp_mix_t));
	mix->ratio = ratio;
	mix->effect = effect;

	return mix;
}

/**
 * Copy a mix effect.
 *   @mix: The original mix.
 *   &returns: The copied mix.
 */
struct amp_mix_t *amp_mix_copy(struct amp_mix_t *mix)
{
	return amp_mix_new(amp_param_copy(mix->ratio), amp_effect_copy(mix->effect));
}

/**
 * Delete a mix effect.
 *   @mix: The mix.
 */
void amp_mix_delete(struct amp_mix_t *mix)
{
	amp_param_delete(mix->ratio);
	amp_effect_delete(mix->effect);
	free(mix);
}


/**
 * Create a mix from a value.
 *   @ret: Ref. The returned value.
 *   @value: The value.
 *   @env: The environment.
 *   &returns: Error.
 */
char *amp_mix_make(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env)
{
#define onexit
	struct amp_param_t *ratio;
	struct amp_effect_t effect;

	chkfail(amp_match_unpack(value, "(P,E)", &ratio, &effect));

	*ret = amp_pack_effect((struct amp_effect_t){ amp_mix_new(ratio, effect), &amp_mix_iface });
	return NULL;
#undef onexit
}


/**
 * Handle information on a mix.
 *   @mix: The mix.
 *   @info: The information.
 */
void amp_mix_info(struct amp_mix_t *mix, struct amp_info_t info)
{
	amp_param_info(mix->ratio, info);
	amp_effect_info(mix->effect, info);
}

/**
 * Process a mix.
 *   @mix: The mix.
 *   @buf: The buffer.
 *   @time: The time.
 *   @len: The length.
 *   &returns: The continuation flag.
 */
bool amp_mix_proc(struct amp_mix_t *mix, double *buf, struct amp_time_t *time, unsigned int len)
{
	unsigned int i;
	double tmp[len];
	bool cont = false;

	dsp_copy_d(tmp, buf, len);
	cont |= amp_effect_proc(mix->effect, tmp, time, len);

	if(amp_param_isfast(mix->ratio)) {
		double ratio = dsp_clamp(mix->ratio->flt, 0.0, 1.0);

		for(i = 0; i < len; i++)
			buf[i] = ratio * tmp[i] + (1.0 - ratio) * buf[i];
	}
	else {
		double t, ratio[len];

		cont |= amp_param_proc(mix->ratio, ratio, time, len);

		for(i = 0; i < len; i++) {
			t = dsp_clamp(ratio[i], 0.0, 1.0);
			buf[i] = t * tmp[i] + (1.0 - t) * buf[i];
		}
	}

	return cont;
}
