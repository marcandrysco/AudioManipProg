#include "../common.h"


/**
 * Generator structure.
 *   @thru: The passthrough level.
 *   @module: The module.
 */

struct amp_gen_t {
	double thru;
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
 *   @thru: The passthrough level.
 *   @module: Consumed. The module.
 *   &returns: The gen.
 */

struct amp_gen_t *amp_gen_new(double thru, struct amp_module_t module)
{
	struct amp_gen_t *gen;

	gen = malloc(sizeof(struct amp_gen_t));
	gen->thru = thru;
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
	return amp_gen_new(gen->thru, amp_module_copy(gen->module));
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
 *   @value: The value.
 *   @env: The environment.
 *   @err: The error.
 *   &returns: The value or null.
 */

struct ml_value_t *amp_gen_make(struct ml_value_t *value, struct ml_env_t *env, char **err)
{
	double thru;
	struct amp_module_t module;

	*err = amp_match_unpack(value, "(f,M)", &thru, &module);
	if(*err != NULL)
		return NULL;

	return amp_pack_effect((struct amp_effect_t){ amp_gen_new(thru, module), &amp_gen_iface });
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
 *   &returns: The continue flag.
 */

bool amp_gen_proc(struct amp_gen_t *gen, double *buf, struct amp_time_t *time, unsigned int len)
{
	bool cont;
	unsigned int i;
	double thru, tmp[len];

	thru = gen->thru;

	dsp_zero_d(tmp, len);
	cont = amp_module_proc(gen->module, tmp, time, len);

	for(i = 0; i < len; i++)
		buf[i] = tmp[i] + thru * buf[i];

	return cont;
}
