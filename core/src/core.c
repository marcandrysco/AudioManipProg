#include "common.h"
#include <dlfcn.h>


/*
 * local declarations
 */

static void *ref_copy(void *ref);
static void ref_delete(void *ref);

static const struct ml_box_i ref_iface = { "AmpRef", ref_copy, ref_delete };


/**
 * Create a new core.
 *   @rate: The sample rate.
 *   &returns: The core.
 */

struct amp_core_t *amp_core_new(unsigned int rate)
{
	char *err;
	struct amp_core_t *core;

	core = malloc(sizeof(struct amp_core_t));
	core->env = ml_env_new();
	core->cache = amp_cache_new();
	core->plugin = NULL;

	ml_env_add(&core->env, strdup("rate"), ml_value_num(rate));
	ml_env_add(&core->env, strdup("conf.rate"), ml_value_num(rate));
	ml_env_add(&core->env, strdup("conf.core"), ml_value_box((struct ml_box_t){ core, &ref_iface }));
	ml_env_add(&core->env, strdup("conf.cache"), ml_value_box((struct ml_box_t){ core->cache, &ref_iface }));

	/* clocks */
	ml_env_add(&core->env, strdup("Basic"), ml_value_impl(amp_basic_make));

	/* effects */
	ml_env_add(&core->env, strdup("Bitcrush"), ml_value_impl(amp_bitcrush_make));
	ml_env_add(&core->env, strdup("Chain"), ml_value_impl(amp_chain_make));
	ml_env_add(&core->env, strdup("Clip"), ml_value_impl(amp_clip_make));
	ml_env_add(&core->env, strdup("Comp"), ml_value_impl(amp_comp_make));
	ml_env_add(&core->env, strdup("Expcrush"), ml_value_impl(amp_expcrush_make));
	ml_env_add(&core->env, strdup("Gain"), ml_value_impl(amp_gain_make));
	ml_env_add(&core->env, strdup("Gen"), ml_value_impl(amp_gen_make));
	ml_env_add(&core->env, strdup("Sect"), ml_value_impl(amp_sect_make));
	ml_env_add(&core->env, strdup("Synth"), ml_value_impl(amp_synth_make));

	ml_env_add(&core->env, strdup("Ctrl"), ml_value_impl(amp_ctrl_make));

	/* instruments */
	ml_env_add(&core->env, strdup("Mixer"), ml_value_impl(amp_mixer_make));
	ml_env_add(&core->env, strdup("Pan"), ml_value_impl(amp_pan_make));
	ml_env_add(&core->env, strdup("Series"), ml_value_impl(amp_series_make));
	ml_env_add(&core->env, strdup("Splice"), ml_value_impl(amp_splice_make));

	/* modules */
	ml_env_add(&core->env, strdup("ADSR"), ml_value_impl(amp_adsr_make));
	ml_env_add(&core->env, strdup("Mul"), ml_value_impl(amp_mul_make));
	ml_env_add(&core->env, strdup("Noise"), ml_value_impl(amp_noise_make));
	ml_env_add(&core->env, strdup("Osc"), ml_value_impl(amp_osc_make));
	ml_env_add(&core->env, strdup("Patch"), ml_value_impl(amp_patch_make));
	ml_env_add(&core->env, strdup("Sample"), ml_value_impl(amp_sample_make));
	ml_env_add(&core->env, strdup("Shot"), ml_value_impl(amp_shot_make));
	ml_env_add(&core->env, strdup("Sum"), ml_value_impl(amp_sum_make));
	ml_env_add(&core->env, strdup("Trig"), ml_value_impl(amp_trig_make));

	/* sequencers */
	ml_env_add(&core->env, strdup("Repeat"), ml_value_impl(amp_repeat_make));
	ml_env_add(&core->env, strdup("Sched"), ml_value_impl(amp_sched_make));

	/* filters */
	ml_env_add(&core->env, strdup("Lpf"), ml_value_impl(amp_lpf_make));
	ml_env_add(&core->env, strdup("Hpf"), ml_value_impl(amp_hpf_make));
	ml_env_add(&core->env, strdup("Moog"), ml_value_impl(amp_moog_make));
	ml_env_add(&core->env, strdup("Peak"), ml_value_impl(amp_peak_make));
	ml_env_add(&core->env, strdup("Res"), ml_value_impl(amp_res_make));

	/* reverberators */
	ml_env_add(&core->env, strdup("Allpass"), ml_value_impl(amp_allpass_make));
	ml_env_add(&core->env, strdup("Comb"), ml_value_impl(amp_comb_make));
	ml_env_add(&core->env, strdup("Delay"), ml_value_impl(amp_delay_make));
	ml_env_add(&core->env, strdup("Lpcf"), ml_value_impl(amp_lpcf_make));

	/* helper functions */
	ml_env_add(&core->env, strdup("str2key"), ml_value_impl(amp_key_eval));

	err = ml_env_proc(SHAREDIR "/core.ml", &core->env);
	if(err != NULL)
		fprintf(stderr, "Failed to parse 'core.ml'. %s\n", err);

	return core;
}

/**
 * Delete a core.
 *   @core: The core.
 */

void amp_core_delete(struct amp_core_t *core)
{
	struct amp_plugin_t *cur, *next;

	for(cur = core->plugin; cur != NULL; cur = next) {
		next = cur->next;

		dlclose(cur->ref);
		free(cur);
	}

	ml_env_delete(core->env);
	amp_cache_delete(core->cache);
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

	value = ml_env_lookup(env, "conf.rate");
	if(value == NULL)
		fprintf(stderr, "Missing 'conf.rate' variable.\n"), abort();

	if(value->type != ml_value_num_e)
		fprintf(stderr, "Variable 'conf.rate' rebound.\n"), abort();

	return value->data.num;
}

/**
 * Retrieve the cache from the environment.
 *   @env: The environment.
 *   &returns: The cache.
 */

struct amp_cache_t *amp_core_cache(struct ml_env_t *env)
{
	struct ml_value_t *value;

	value = ml_env_lookup(env, "conf.cache");
	if(value == NULL)
		fprintf(stderr, "Missing 'conf.cache' variable.\n"), abort();

	if((value->type != ml_value_box_e) || (value->data.box.iface != &ref_iface))
		fprintf(stderr, "Variable 'conf.cache' rebound.\n"), abort();

	return value->data.box.ref;
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

	*err = ml_env_proc(path, &env);
	if(*err == NULL)
		return env;

	ml_env_delete(env);

	return NULL;
}

/**
 * Load a plugin into the core.
 *   @core: The core.
 *   @path: The path.
 *   &returns: Error.
 */

char *amp_core_plugin(struct amp_core_t *core, const char *path)
{
#define onexit if(lib != NULL) dlclose(lib);
	void *lib;
	amp_plugin_f func;
	struct amp_plugin_t *plugin;

	lib = dlopen(path, RTLD_NOW | RTLD_LOCAL);
	if(lib == NULL)
		fail("Failed to open plugin '%s'. %s.", path, dlerror());

	func = dlsym(lib, "amp_plugin");
	if(func == NULL)
		fail("Failed to open plugin '%s'. Library missing 'amp_plugin' symbol.", path);

	func(core);

	plugin = malloc(sizeof(struct amp_plugin_t));
	plugin->ref = lib;
	plugin->next = core->plugin;

	core->plugin = plugin;

	return NULL;
#undef onexit
}


/*
 * global variables
 */

struct ml_box_i amp_box_iface = {
	"Amp",
	(void *(*)(void *))amp_box_copy,
	(void (*)(void *))amp_box_delete
};


/**
 * Copy a reference.
 *   @ref: The reference.
 *   &returns: The copy.
 */

static void *ref_copy(void *ref)
{
	return ref;
}

/**
 * Delete a reference.
 *   @ref: The reference.
 */

static void ref_delete(void *ref)
{
}
