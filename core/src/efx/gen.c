#include "../common.h"


/**
 * Generator structure.
 *   @module: The module.
 */
struct amp_gen_t {
	struct amp_module_t module;
};


/*
 * global variables
 */
const struct amp_effect_i amp_gen_iface = {
	(amp_info_f)amp_gen_info,
	(amp_effect_f)amp_gen_proc,
	(amp_copy_f)amp_gen_copy,
	(amp_delete_f)amp_gen_delete
};


/**
 * Create a generator effect.
 *   @module: Consumed. The module.
 *   &returns: The gen.
 */
struct amp_gen_t *amp_gen_new(struct amp_module_t module)
{
	struct amp_gen_t *gen;

	gen = malloc(sizeof(struct amp_gen_t));
	gen->module = module;

	return gen;
}

/**
 * Copy a generator effect.
 *   @gen: The original generator.
 *   &returns: The copied generator.
 */
struct amp_gen_t *amp_gen_copy(struct amp_gen_t *gen)
{
	return amp_gen_new(amp_module_copy(gen->module));
}

/**
 * Delete a generator effect.
 *   @gen: The generator.
 */
void amp_gen_delete(struct amp_gen_t *gen)
{
	amp_module_delete(gen->module);
	free(gen);
}


/**
 * Create a generator from a value.
 *   @ret: Ref. The returned value.
 *   @value: The value.
 *   @env: The environment.
 *   &returns: Error.
 */
char *amp_gen_make(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env)
{
#define onexit
	struct amp_module_t module;

	chkfail(amp_match_unpack(value, "M", &module));
	*ret = amp_pack_effect((struct amp_effect_t){ amp_gen_new(module), &amp_gen_iface });

	return NULL;
#undef onexit
}


/**
 * Handle information on a generator.
 *   @gen: The generator.
 *   @info: The information.
 */
void amp_gen_info(struct amp_gen_t *gen, struct amp_info_t info)
{
	amp_module_info(gen->module, info);
}

/**
 * Process a generator.
 *   @gen: The generator.
 *   @buf: The buffer.
 *   @time: The time.
 *   @len: The length.
 *   @queue: The action queue.
 *   &returns: The continue flag.
 */
bool amp_gen_proc(struct amp_gen_t *gen, double *buf, struct amp_time_t *time, unsigned int len, struct amp_queue_t *queue)
{
	bool cont;
	double tmp[len];

	cont = amp_module_proc(gen->module, tmp, time, len, queue);
	dsp_add_d(buf, tmp, len);

	return cont;
}
