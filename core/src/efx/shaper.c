#include "../common.h"

/**
 * Shaper structure.
 *   @thresh: The threshdold.
 *   @cur, inst: The current and head instances.
 */
struct amp_shaper_t {
	struct amp_param_t *thresh;

	struct amp_shaper_inst_t *cur, *inst;
};

/**
 * Shaper instance.
 *   @idx: The index.
 *   @next: The next.
 */
struct amp_shaper_inst_t {
	unsigned int idx;

	double c, target;
	struct amp_shaper_inst_t *next;
};


/*
 * global variables
 */
const struct amp_effect_i amp_shaper_iface = {
	(amp_info_f)amp_shaper_info,
	(amp_effect_f)amp_shaper_proc,
	(amp_copy_f)amp_shaper_copy,
	(amp_delete_f)amp_shaper_delete
};


/**
 * Create a shaper effect.
 *   @thresh: Consumed. The threshold.
 *   &returns: The shaper.
 */
struct amp_shaper_t *amp_shaper_new(struct amp_param_t *thresh)
{
	struct amp_shaper_t *shaper;

	shaper = malloc(sizeof(struct amp_shaper_t));
	shaper->thresh = thresh;

	return shaper;
}

/**
 * Copy a shaper effect.
 *   @shaper: The original shaper.
 *   &returns: The copied shaper.
 */
struct amp_shaper_t *amp_shaper_copy(struct amp_shaper_t *shaper)
{
	return amp_shaper_new(amp_param_copy(shaper->thresh));
}

/**
 * Delete a shaper effect.
 *   @shaper: The shaper.
 */
void amp_shaper_delete(struct amp_shaper_t *shaper)
{
	amp_param_delete(shaper->thresh);
	free(shaper);
}


/**
 * Create a shaper from a value.
 *   @ret: Ref. The returned value.
 *   @value: The value.
 *   @env: The environment.
 *   &returns: Error.
 */
char *amp_shaper_make(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env)
{
#define onexit
	struct amp_param_t *thresh;

	chkfail(amp_match_unpack(value, "P", &thresh));

	*ret = amp_pack_effect((struct amp_effect_t){ amp_shaper_new(thresh), &amp_shaper_iface });
	return NULL;
#undef onexit
}


/**
 * Handle information on a shaper.
 *   @shaper: The shaper.
 *   @info: The information.
 */
void amp_shaper_info(struct amp_shaper_t *shaper, struct amp_info_t info)
{
	amp_param_info(shaper->thresh, info);
}

/**
 * Process a shaper.
 *   @shaper: The shaper.
 *   @buf: The buffer.
 *   @time: The time.
 *   @len: The length.
 *   @queue: The action queue.
 *   &returns: The continuation flag.
 */
bool amp_shaper_proc(struct amp_shaper_t *shaper, double *buf, struct amp_time_t *time, unsigned int len, struct amp_queue_t *queue)
{

	return false;
}
