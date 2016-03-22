#include "common.h"

/*
 * local declarations
 */
static void notify(const char *path, void *arg);


/**
 * Create an engine.
 *   @list: Constant. Optional. The file list.
 *   @comm: Consumed. Optional. The communication structure.
 *   &returns: The engine.
 */
struct amp_engine_t *amp_engine_new(char **list, struct amp_comm_t *comm)
{
	struct amp_engine_t *engine;

	engine = malloc(sizeof(struct amp_engine_t));
	engine->rev = 1;
	engine->lock = sys_mutex_init(0);
	engine->sync = sys_mutex_init(0);
	engine->run = engine->toggle = false;
	engine->core = amp_core_new(96000);
	engine->clock = amp_basic_clock(amp_basic_new(120.0, 4.0, 96000));
	engine->seq = amp_seq_null;
	engine->instr = amp_instr_null;
	engine->effect[0] = amp_effect_null;
	engine->effect[1] = amp_effect_null;
	engine->comm = comm ?: amp_comm_new();
	engine->notify = amp_notify_new(list, notify, engine);
	engine->watch = NULL;
	engine->rt = (struct amp_rt_t){ engine, amp_engine_watch };

	ml_env_add(&engine->core->env, strdup("amp.rt"), ml_value_box(amp_box_ref(&engine->rt)));

	return engine;
}

/**
 * Delete an engine.
 *   @engine: The engine.
 */
void amp_engine_delete(struct amp_engine_t *engine)
{
	struct amp_watch_t *watch;

	while(engine->watch != NULL) {
		watch = engine->watch;
		engine->watch = watch->next;

		free(watch);
	}

	amp_notify_delete(engine->notify);
	amp_comm_delete(engine->comm);
	amp_clock_delete(engine->clock);
	amp_seq_erase(engine->seq);
	amp_instr_erase(engine->instr);
	amp_effect_erase(engine->effect[0]);
	amp_effect_erase(engine->effect[1]);
	amp_core_delete(engine->core);
	sys_mutex_destroy(&engine->lock);
	sys_mutex_destroy(&engine->sync);
	free(engine);
}

/**
 * Add a watch to the engine.
 *   @engine: The engine.
 *   @func: The function.
 *   @arg: The argument.
 */
void amp_engine_watch(struct amp_engine_t *engine, amp_watch_f func, void *arg)
{
	struct amp_watch_t *watch;

	watch = malloc(sizeof(struct amp_watch_t));
	watch->func = func;
	watch->arg = arg;
	watch->next = engine->watch;

	engine->watch = watch;
}


/**
 * Handle a change notification.
 *   @path: The path.
 *   @arg: The argument.
 */
static void notify(const char *path, void *arg)
{
	amp_engine_update(arg, path);
}
