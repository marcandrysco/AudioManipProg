#ifndef INC_H
#define INC_H

/*
 * structure prototypes
 */
struct amp_engine_t;

/**
 * Watch function callback.
 *   @engine: The engine.
 *   @arg: The argument.
 */
typedef void (*amp_watch_f)(struct amp_engine_t *engine, void *arg);

/**
 * RT structure.
 *   @engine: The engine.
 *   @watch: The watch function.
 */
struct amp_rt_t {
	struct amp_engine_t *engine;
	void (*watch)(struct amp_engine_t *, amp_watch_f, void *);
};

/**
 * Add a watch function to AmpRT.
 *   @rt: AmpRT.
 *   @func: The function.
 *   @arg: The argument.
 */
static inline void amp_rt_watch(struct amp_rt_t *rt, amp_watch_f func, void *arg)
{
	rt->watch(rt->engine, func, arg);
}

/**
 * Retrieve AmpRT from the core.
 *   @core: The core.
 *   &returns: The AmpRT reference, or null if not run from the AmpRT binary.
 */
static inline struct amp_rt_t *amp_rt_get(struct amp_core_t *core)
{
	struct ml_value_t *value;

	value = ml_env_lookup(core->env, "amp.rt");
	if(value == NULL)
		return NULL;

	if(value->type != ml_value_box_e)
		return NULL;

	return value->data.box.ref;
}

#endif
