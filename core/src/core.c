#include "common.h"


/**
 * Create a new core.
 *   @rate: The sample rate.
 *   &returns: The core.
 */

struct amp_core_t *amp_core_new(unsigned int rate)
{
	struct amp_core_t *core;

	core = malloc(sizeof(struct amp_core_t));
	core->env = ml_env_new();

	ml_env_add(core->env, strdup("rate"), ml_value_num(rate));

	ml_env_add(core->env, strdup("Basic"), ml_value_impl(amp_basic_make));

	ml_env_add(core->env, strdup("Chain"), ml_value_impl(amp_chain_make));
	ml_env_add(core->env, strdup("Clip"), ml_value_impl(amp_clip_make));
	ml_env_add(core->env, strdup("Comp"), ml_value_impl(amp_comp_make));
	ml_env_add(core->env, strdup("Gain"), ml_value_impl(amp_gain_make));

	ml_env_add(core->env, strdup("Ctrl"), ml_value_impl(amp_ctrl_make));

	ml_env_add(core->env, strdup("Mixer"), ml_value_impl(amp_mixer_make));
	ml_env_add(core->env, strdup("Splice"), ml_value_impl(amp_splice_make));
	ml_env_add(core->env, strdup("Synth"), ml_value_impl(amp_synth_make));

	ml_env_add(core->env, strdup("ADSR"), ml_value_impl(amp_adsr_make));
	ml_env_add(core->env, strdup("Mul"), ml_value_impl(amp_mul_make));
	ml_env_add(core->env, strdup("Osc"), ml_value_impl(amp_osc_make));
	ml_env_add(core->env, strdup("Patch"), ml_value_impl(amp_patch_make));
	ml_env_add(core->env, strdup("Sum"), ml_value_impl(amp_sum_make));
	ml_env_add(core->env, strdup("Trig"), ml_value_impl(amp_trig_make));

	ml_env_add(core->env, strdup("Sched"), ml_value_impl(amp_sched_make));

	ml_env_add(core->env, strdup("Lpf"), ml_value_impl(amp_lpf_make));
	ml_env_add(core->env, strdup("Hpf"), ml_value_impl(amp_hpf_make));

	return core;
}

/**
 * Delete a core.
 *   @core: The core.
 */

void amp_core_delete(struct amp_core_t *core)
{
	ml_env_delete(core->env);
	free(core);
}


/**
 * Retrieve the rate from the environment.
 *   @env: The environment.
 *   &returns: The rate.
 */

unsigned int amp_core_rate(struct ml_env_t *env)
{
	struct ml_value_t *value;

	value = ml_env_lookup(env, "rate");
	if(value == NULL)
		fprintf(stderr, "Missing 'rate' variable.\n"), abort();

	if(value->type != ml_value_num_e)
		fprintf(stderr, "Variable 'rate' rebound.\n"), abort();

	return value->data.num;
}


/**
 * Evaluate an expression from file.
 *   @core; The core.
 *   @path: The path.
 *   @err: The error.
 *   &returns: The box or null.
 */

struct ml_env_t *amp_core_eval(struct amp_core_t *core, const char *path, char **err)
{
	struct ml_env_t *env;

	env = ml_env_copy(core->env);

	*err = ml_env_proc(path, env);
	if(*err == NULL)
		return env;

	ml_env_delete(env);

	return NULL;
}


/*
 * global variables
 */

struct ml_box_i amp_box_iface = {
	"Amp",
	(void *(*)(void *))amp_box_copy,
	(void (*)(void *))amp_box_delete
};
