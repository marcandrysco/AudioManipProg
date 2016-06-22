#include "common.h"
#include <dlfcn.h>


/**
 * Evaluation pair structure.
 *   @id: The identifier.
 *   @func: The function.
 */

struct pair_t {
	const char *id;
	ml_eval_f func;
};

/*
 * local declarations
 */
static void *ref_copy(void *ref);
static void ref_delete(void *ref);

static const struct ml_box_i ref_iface = { ref_copy, ref_delete };

/*
 * global variables
 */
struct ml_box_i amp_box_iface = {
	(void *(*)(void *))amp_box_copy,
	(void (*)(void *))amp_box_delete
};

/*
 * evaluation list
 */
static const struct pair_t list[] = {
	/* effects */
	{ "Bias",     amp_bias_make },
	{ "Bitcrush", amp_bitcrush_make },
	{ "Chorus",   amp_chorus_make },
	{ "Expcrush", amp_expcrush_make },
	/* filters */
	{ "Lpf",   amp_lpf_make },
	{ "Hpf",   amp_hpf_make },
	{ "Moog",  amp_moog_make },
	{ "Peak",  amp_peak_make },
	{ "Res",   amp_res_make },
	{ "Svlpf", amp_svlpf_make },
	{ "Svhpf", amp_svhpf_make },
	/* filter - butterworth */
	{ "ButterLow2",   amp_butter2low_make },
	{ "ButterHigh2",  amp_butter2high_make },
	{ "ButterLow3",   amp_butter3low_make },
	{ "ButterHigh3",  amp_butter3high_make },
	{ "ButterLow4",   amp_butter4low_make },
	{ "ButterHigh4",  amp_butter4high_make },
	/* modules */
	{ "Beat",  amp_beat_make },
	{ "Noise", amp_noise_make },
	{ "Ramp",  amp_ramp_make },
	/* reverberators */
	{ "Allpass", amp_allpass_make },
	{ "Comb",    amp_comb_make },
	{ "Delay",   amp_delay_make },
	{ "Lpcf",    amp_lpcf_make },
	/* standard functions */
	{ "vel",    amp_eval_vel },
	{ "val",    amp_eval_val },
	{ "amp2db", amp_eval_amp2db },
	{ "db2amp", amp_eval_db2amp },
	{ "human",  amp_eval_human },
	{ NULL    , NULL }
};


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
	core->io = amp_io_new();
	core->cache = amp_cache_new();
	core->plugin = NULL;

	ml_env_add(&core->env, strdup("amp.rate"), ml_value_num(rate, ml_tag_copy(ml_tag_null)));
	ml_env_add(&core->env, strdup("amp.core"), ml_value_box((struct ml_box_t){ core, &ref_iface }, ml_tag_copy(ml_tag_null)));
	ml_env_add(&core->env, strdup("amp.cache"), ml_value_box((struct ml_box_t){ core->cache, &ref_iface }, ml_tag_copy(ml_tag_null)));

	/* polymorphic */
	ml_env_add(&core->env, strdup("Shot"), ml_value_eval(amp_shot_make, ml_tag_copy(ml_tag_null)));

	/* clocks */
	//ml_env_add(&core->env, strdup("Basic"), ml_value_impl(amp_basic_make));

	/* effects */
	//ml_env_add(&core->env, strdup("Bitcrush"), ml_value_impl(amp_bitcrush_make));
	ml_env_add(&core->env, strdup("Chain"), ml_value_eval(amp_chain_make, ml_tag_copy(ml_tag_null)));
	ml_env_add(&core->env, strdup("Clip"), ml_value_eval(amp_clip_make, ml_tag_copy(ml_tag_null)));
	ml_env_add(&core->env, strdup("Comp"), ml_value_eval(amp_comp_make, ml_tag_copy(ml_tag_null)));
	//ml_env_add(&core->env, strdup("Expcrush"), ml_value_impl(amp_expcrush_make));
	ml_env_add(&core->env, strdup("Gain"), ml_value_eval(amp_gain_make, ml_tag_copy(ml_tag_null)));
	ml_env_add(&core->env, strdup("Gate"), ml_value_eval(amp_gate_make, ml_tag_copy(ml_tag_null)));
	ml_env_add(&core->env, strdup("Gen"), ml_value_eval(amp_gen_make, ml_tag_copy(ml_tag_null)));
	ml_env_add(&core->env, strdup("Loop"), ml_value_eval(amp_loop_make, ml_tag_copy(ml_tag_null)));
	ml_env_add(&core->env, strdup("Mix"), ml_value_eval(amp_mix_make, ml_tag_copy(ml_tag_null)));
	ml_env_add(&core->env, strdup("Octave"), ml_value_eval(amp_octave_make, ml_tag_copy(ml_tag_null)));
	ml_env_add(&core->env, strdup("Sect"), ml_value_eval(amp_sect_make, ml_tag_copy(ml_tag_null)));
	ml_env_add(&core->env, strdup("Scale"), ml_value_eval(amp_scale_make, ml_tag_copy(ml_tag_null)));
	ml_env_add(&core->env, strdup("Wrap"), ml_value_eval(amp_wrap_make, ml_tag_copy(ml_tag_null)));
	ml_env_add(&core->env, strdup("Vol"), ml_value_eval(amp_vol_make, ml_tag_copy(ml_tag_null)));

	//ml_env_add(&core->env, strdup("Ctrl"), ml_value_impl(amp_ctrl_make));

	/* instruments */
	ml_env_add(&core->env, strdup("Inject"), ml_value_eval(amp_inject_make, ml_tag_copy(ml_tag_null)));
	ml_env_add(&core->env, strdup("Mixer"), ml_value_eval(amp_mixer_make, ml_tag_copy(ml_tag_null)));
	ml_env_add(&core->env, strdup("Series"), ml_value_eval(amp_series_make, ml_tag_copy(ml_tag_null)));
	ml_env_add(&core->env, strdup("Single"), ml_value_eval(amp_single_make, ml_tag_copy(ml_tag_null)));
	ml_env_add(&core->env, strdup("Splice"), ml_value_eval(amp_splice_make, ml_tag_copy(ml_tag_null)));

	/* modules */
	ml_env_add(&core->env, strdup("ADSR"), ml_value_eval(amp_adsr_make, ml_tag_copy(ml_tag_null)));
	ml_env_add(&core->env, strdup("Mul"), ml_value_eval(amp_mul_make, ml_tag_copy(ml_tag_null)));
	ml_env_add(&core->env, strdup("Patch"), ml_value_eval(amp_patch_make, ml_tag_copy(ml_tag_null)));
	ml_env_add(&core->env, strdup("Sample"), ml_value_eval(amp_sample_make, ml_tag_copy(ml_tag_null)));
	ml_env_add(&core->env, strdup("Sum"), ml_value_eval(amp_sum_make, ml_tag_copy(ml_tag_null)));
	ml_env_add(&core->env, strdup("Synth"), ml_value_eval(amp_synth_make, ml_tag_copy(ml_tag_null)));
	ml_env_add(&core->env, strdup("Trig"), ml_value_eval(amp_trig_make, ml_tag_copy(ml_tag_null)));
	ml_env_add(&core->env, strdup("Warp"), ml_value_eval(amp_warp_make, ml_tag_copy(ml_tag_null)));

	ml_env_add(&core->env, strdup("Sine"), ml_value_eval(amp_sine_make, ml_tag_copy(ml_tag_null)));
	ml_env_add(&core->env, strdup("Tri"), ml_value_eval(amp_tri_make, ml_tag_copy(ml_tag_null)));
	ml_env_add(&core->env, strdup("Square"), ml_value_eval(amp_square_make, ml_tag_copy(ml_tag_null)));

	/* sequencers */
	ml_env_add(&core->env, strdup("Enable"), ml_value_eval(amp_enable_make, ml_tag_copy(ml_tag_null)));
	ml_env_add(&core->env, strdup("Merge"), ml_value_eval(amp_merge_make, ml_tag_copy(ml_tag_null)));
	ml_env_add(&core->env, strdup("Sched"), ml_value_eval(amp_sched_make, ml_tag_copy(ml_tag_null)));
	//ml_env_add(&core->env, strdup("Player"), ml_value_impl(amp_player_make0));
	ml_env_add(&core->env, strdup("Repeat"), ml_value_eval(amp_repeat_make, ml_tag_copy(ml_tag_null)));
	ml_env_add(&core->env, strdup("Snap"), ml_value_eval(amp_snap_make, ml_tag_copy(ml_tag_null)));
	ml_env_add(&core->env, strdup("Toggle"), ml_value_eval(amp_toggle_make, ml_tag_copy(ml_tag_null)));

	//ml_env_add(&core->env, strdup("Butter2low"), ml_value_impl(amp_butter2low_make));
	//ml_env_add(&core->env, strdup("Butter2high"), ml_value_impl(amp_butter2high_make));
	//ml_env_add(&core->env, strdup("Butter3low"), ml_value_impl(amp_butter3low_make));
	//ml_env_add(&core->env, strdup("Butter3high"), ml_value_impl(amp_butter3high_make));
	//ml_env_add(&core->env, strdup("Butter4low"), ml_value_impl(amp_butter4low_make));
	//ml_env_add(&core->env, strdup("Butter4high"), ml_value_impl(amp_butter4high_make));

	const struct pair_t *cur;

	for(cur = list; cur->id != NULL; cur++)
		ml_env_add(&core->env, strdup(cur->id), ml_value_eval(cur->func, ml_tag_copy(ml_tag_null)));
	
	err = ml_parse_file(&core->env, SHAREDIR "/amp/core.ml");
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
	amp_plugin_f func;
	struct amp_plugin_t *cur, *next;

	for(cur = core->plugin; cur != NULL; cur = next) {
		next = cur->next;

		func = dlsym(cur->ref, "amp_plugin_unload");
		if(func != NULL)
			func(core);

		dlclose(cur->ref);
		free(cur);
	}

	ml_env_delete(core->env);
	amp_cache_delete(core->cache);
	amp_io_delete(core->io);
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

	value = ml_env_lookup(env, "amp.rate");
	if(value == NULL)
		fprintf(stderr, "Missing 'amp.rate' variable.\n"), abort();

	if(value->type != ml_value_num_v)
		fprintf(stderr, "Variable 'amp.rate' rebound.\n"), abort();

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

	value = ml_env_lookup(env, "amp.cache");
	if(value == NULL)
		fprintf(stderr, "Missing 'amp.cache' variable.\n"), abort();

	if((value->type != ml_value_box_v) || (value->data.box.iface != &ref_iface))
		fprintf(stderr, "Variable 'amp.cache' rebound.\n"), abort();

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

	*err = ml_parse_file(&env, path);
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

	func = dlsym(lib, "amp_plugin_load");
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


/**
 * Create a boxed reference.
 *   @ref: The reference.
 *   &returns: The boxed object.
 */
struct ml_box_t amp_box_ref(void *ref)
{
	return (struct ml_box_t){ ref, &ref_iface };
}

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
