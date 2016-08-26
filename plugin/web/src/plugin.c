#include "common.h"


/*
 * global variables
 */
struct web_serv_t *web_serv = NULL;


/**
 * Plugin loading callback.
 *   @core: The core,
 */
void amp_plugin_load(struct amp_core_t *core)
{
	web_serv = web_serv_new();
	ml_env_add(&core->env, strdup("WebMach"), ml_value_eval(web_mach_make, ml_tag_copy(ml_tag_null)));
	ml_env_add(&core->env, strdup("WebPlayer"), ml_value_eval(web_player_make, ml_tag_copy(ml_tag_null)));
}

/**
 * Plugin unloading callback.
 *   @core: The core.
 */
void amp_plugin_unload(struct amp_core_t *core)
{
	web_serv_delete(web_serv);
}
