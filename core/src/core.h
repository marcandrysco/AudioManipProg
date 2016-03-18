#ifndef CORE_H
#define CORE_H


/**
 * Core structure.
 *   @env: The environemnt.
 *   @cache: The file cache.
 *   @plugin: The plugin list.
 */
struct amp_core_t {
	struct ml_env_t *env;
	struct amp_cache_t *cache;
	struct amp_plugin_t *plugin;
};

/**
 * Plugin structure.
 *   @ref: The library reference.
 *   @next: The next plugin.
 */
struct amp_plugin_t {
	void *ref;
	struct amp_plugin_t *next;
};

/**
 * Plugin function declaration.
 *   @core; The core.
 */
typedef void (*amp_plugin_f)(struct amp_core_t *core);


/*
 * core declarations
 */
struct amp_core_t *amp_core_new(unsigned int rate);
void amp_core_delete(struct amp_core_t *core);

struct ml_env_t *amp_core_eval(struct amp_core_t *core, const char *path, char **err);
char *amp_core_plugin(struct amp_core_t *core, const char *path);

unsigned int amp_core_rate(struct ml_env_t *env);
struct amp_cache_t *amp_core_cache(struct ml_env_t *env);

struct ml_box_t amp_box_ref(void *ref);

#endif
