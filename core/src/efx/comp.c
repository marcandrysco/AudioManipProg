#include "../common.h"


/**
 * Compressor structure.
 *   @fast: The fast flag.
 *   @vol, ctrl, rate: The volume, control, and sample rate.
 *   @atk, rel, thres, ratio: The compressor parameters.
 */
struct amp_comp_t {
	bool fast;

	double vol, ctrl, rate;
	struct amp_param_t *atk, *rel, *thresh, *ratio;
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
 *   @rate; The sample rate.
 *   &returns: The comp.
 */
struct amp_comp_t *amp_comp_new(struct amp_param_t *atk, struct amp_param_t *rel, struct amp_param_t *thresh, struct amp_param_t *ratio, double rate)
{
	struct amp_comp_t *comp;

	comp = malloc(sizeof(struct amp_comp_t));
	comp->fast = amp_param_isfast(atk) || amp_param_isfast(rel) || amp_param_isfast(thresh) || amp_param_isfast(ratio);
	comp->vol = 0.0;
	comp->ctrl = 1.0;
	comp->atk = atk;
	comp->rel = rel;
	comp->thresh = thresh;
	comp->ratio = ratio;
	comp->rate = rate;

	return comp;
}

/**
 * Copy a compressor effect.
 *   @comp: The original compressor.
 *   &returns: The copied compressor.
 */
struct amp_comp_t *amp_comp_copy(struct amp_comp_t *comp)
{
	return amp_comp_new(amp_param_copy(comp->atk), amp_param_copy(comp->rel), amp_param_copy(comp->thresh), amp_param_copy(comp->ratio), comp->rate);
}

/**
 * Delete a compressor effect.
 *   @comp: The compressor.
 */
void amp_comp_delete(struct amp_comp_t *comp)
{
	amp_param_delete(comp->atk);
	amp_param_delete(comp->rel);
	amp_param_delete(comp->thresh);
	amp_param_delete(comp->ratio);
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
	struct amp_param_t *atk, *rel, *thresh, *ratio;

	*err = amp_match_unpack(value, "(P,P,P,P)", &atk, &rel, &thresh, &ratio);
	if(*err != NULL)
		return NULL;

	return amp_pack_effect((struct amp_effect_t){ amp_comp_new(atk, rel, thresh, ratio, amp_core_rate(env)), &amp_comp_iface });
}


/**
 * Handle information on a compressor.
 *   @comp: The compressor.
 *   @info: The information.
 */
void amp_comp_info(struct amp_comp_t *comp, struct amp_info_t info)
{
	amp_param_info(comp->atk, info);
	amp_param_info(comp->rel, info);
	amp_param_info(comp->thresh, info);
	amp_param_info(comp->ratio, info);
}

/**
 * Process a compressor.
 *   @comp: The compressor.
 *   @buf: The buffer.
 *   @time: The time.
 *   @len: The length.
 *   @queue: The action queue.
 *   &returns: The continuation flag.
 */
bool amp_comp_proc(struct amp_comp_t *comp, double *buf, struct amp_time_t *time, unsigned int len, struct amp_queue_t *queue)
{
	double vol, ctrl;
	unsigned int i;

	vol = comp->vol;
	ctrl = comp->ctrl;

	if(comp->fast) {
		struct dsp_comp_t dsp;

		dsp = dsp_comp_init(comp->atk->flt * comp->rate, comp->rel->flt * comp->rate, comp->thresh->flt, comp->ratio->flt);

		for(i = 0; i < len; i++)
			buf[i] = dsp_comp_proc(&dsp, buf[i], &vol, &ctrl);
	}
	else {
	}

	comp->vol = vol;
	comp->ctrl = ctrl;

	return false;
}
