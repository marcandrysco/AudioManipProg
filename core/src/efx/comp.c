#include "../common.h"


/**
 * Compressor structure.
 *   @atk, rel, thres, ratio: The compressor parameters.
 */

struct amp_comp_t {
	struct amp_value_t atk, rel, thresh, ratio;
};


/*
 * global variables
 */

const struct amp_effect_i amp_comp_iface = {
	(amp_info_f)amp_comp_info,
	(amp_effect_f)amp_comp_proc,
	(amp_copy_f)amp_comp_copy,
	(amp_delete_f)amp_comp_delete
};


/**
 * Create a compressor effect.
 *   @atk: The attack time.
 *   @rel: The release time.
 *   @thresh: The threshold.
 *   @ratio: The ratio.
 *   &returns: The comp.
 */

struct amp_comp_t *amp_comp_new(struct amp_value_t atk, struct amp_value_t rel, struct amp_value_t thresh, struct amp_value_t ratio)
{
	struct amp_comp_t *comp;

	comp = malloc(sizeof(struct amp_comp_t));
	comp->atk = atk;
	comp->rel = rel;
	comp->thresh = thresh;
	comp->ratio = ratio;

	return comp;
}

/**
 * Copy a compressor effect.
 *   @comp: The original compressor.
 *   &returns: The copied compressor.
 */

struct amp_comp_t *amp_comp_copy(struct amp_comp_t *comp)
{
	return amp_comp_new(amp_value_copy(comp->atk), amp_value_copy(comp->rel), amp_value_copy(comp->thresh), amp_value_copy(comp->ratio));
}

/**
 * Delete a compressor effect.
 *   @comp: The compressor.
 */

void amp_comp_delete(struct amp_comp_t *comp)
{
	amp_value_delete(comp->atk);
	amp_value_delete(comp->rel);
	amp_value_delete(comp->thresh);
	amp_value_delete(comp->ratio);
	free(comp);
}


/**
 * Create a compressor from a value.
 *   @value: The value.
 *   @env: The environment.
 *   @err: The error.
 *   &returns: The value or null.
 */

struct ml_value_t *amp_comp_make(struct ml_value_t *value, struct ml_env_t *env, char **err)
{
	struct amp_value_t atk, rel, thresh, ratio;

	*err = amp_value_scan(value, env, &atk, &rel, &thresh, &ratio, NULL);
	ml_value_delete(value);

	return (*err == NULL) ? amp_pack_effect((struct amp_effect_t){ amp_comp_new(atk, rel, thresh, ratio), &amp_comp_iface }) : NULL;
}


/**
 * Handle information on a compressor.
 *   @comp: The compressor.
 *   @info: The information.
 */

void amp_comp_info(struct amp_comp_t *comp, struct amp_info_t info)
{
	//amp_value_info(&comp->atk, info);
	//amp_value_info(&comp->rel, info);
	//amp_value_info(&comp->thresh, info);
	//amp_value_info(&comp->ratio, info);
}

/**
 * Process a compressor.
 *   @comp: The compressor.
 *   @buf: The buffer.
 *   @time: The time.
 *   @len: The length.
 */

void amp_comp_proc(struct amp_comp_t *comp, double *buf, struct amp_time_t *time, unsigned int len)
{
}
