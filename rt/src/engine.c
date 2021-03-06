#include "common.h"

/*
 * local declarations
 */
static void notify_proc(struct sys_change_t *change, void *arg);


/**
 * Create an engine.
 *   @path: Constant. Optional. The source file.
 *   @comm: Consumed. Optional. The communication structure.
 *   @audio: The audio interface.
 *   &returns: The engine.
 */
struct amp_engine_t *amp_engine_new(const char *path, struct amp_comm_t *comm, struct amp_audio_t audio)
{
	const char *iface;
	struct amp_engine_t *engine;

	engine = malloc(sizeof(struct amp_engine_t));
	engine->rev = 1;
	engine->lock = sys_mutex_init(0);
	engine->sync = sys_mutex_init(0);
	engine->run = false;
	engine->core = amp_core_new(amp_audio_info(audio).rate);
	engine->clock = amp_basic_clock(amp_basic_new(120.0, 4.0, amp_audio_info(audio).rate));
	engine->instr = amp_instr_null;
	engine->comm = comm ?: amp_comm_new();
	engine->notify = sys_notify_async1(path, notify_proc, engine);
	engine->watch = NULL;
	engine->rt = (struct amp_rt_t){ engine, amp_export_watch, amp_export_status, amp_export_start, amp_export_stop, amp_export_seek };

	ml_env_add(&engine->core->env, strdup("amp.rt"), ml_value_box(amp_box_ref(&engine->rt), ml_tag_copy(ml_tag_null)));

	iface = "unk";
	
	if(audio.iface == &dummy_audio_iface)
		iface = "dummy";
#if ALSA
	else if(audio.iface == &alsa_audio_iface)
		iface = "alsa";
#endif
#if PULSE
	else if(audio.iface == &pulse_audio_iface)
		iface = "pulse";
#endif

	ml_env_add(&engine->core->env, strdup("amp.audio"), ml_value_str(strdup(iface), ml_tag_copy(ml_tag_null)));

	return engine;
}

/**
 * Delete an engine.
 *   @engine: The engine.
 */
void amp_engine_delete(struct amp_engine_t *engine)
{
	struct amp_watch_t *watch;

	if(engine->run)
		amp_engine_stop(engine);

	sys_task_delete(engine->notify);

	while(engine->watch != NULL) {
		watch = engine->watch;
		engine->watch = watch->next;

		free(watch);
	}

	amp_comm_delete(engine->comm);
	amp_clock_delete(engine->clock);
	amp_instr_erase(engine->instr);
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
 *   @change: The change.
 *   @arg: The argument.
 */
static void notify_proc(struct sys_change_t *change, void *arg)
{
	//amp_engine_update(arg, path);
}


/**
 * Check the status of the engine.
 *   @engine: The engine.
 *   &returns: True if running.
 */
bool amp_engine_status(struct amp_engine_t *engine)
{
	return engine->run;
}

/**
 * Start the engine.
 *   @engine: The engine.
 */
void amp_engine_start(struct amp_engine_t *engine)
{
	struct amp_seek_t seek;

	engine->run = true;
	amp_clock_info(engine->clock, amp_info_start(&seek));
	amp_instr_info(engine->instr, amp_info_start(&seek));
}

/**
 * Stop the engine.
 *   @engine: The engine.
 */
void amp_engine_stop(struct amp_engine_t *engine)
{
	struct amp_seek_t seek;

	engine->run = false;
	amp_clock_info(engine->clock, amp_info_stop(&seek));
	amp_instr_info(engine->instr, amp_info_stop(&seek));
}

/**
 * Seek the engine.
 *   @engine: The engine.
 *   @bar: The bar.
 */
void amp_engine_seek(struct amp_engine_t *engine, double bar)
{
	struct amp_seek_t seek;

	if(engine->run)
		amp_clock_info(engine->clock, amp_info_stop(&seek));

	amp_clock_info(engine->clock, amp_info_seek(&bar));
	amp_instr_info(engine->instr, amp_info_seek(&bar));

	if(engine->run)
		amp_clock_info(engine->clock, amp_info_start(&seek));
}
