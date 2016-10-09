#ifndef INC_H
#define INC_H

/**
 * Watch function callback.
 *   @env: The environment.
 *   @arg: The argument.
 */
typedef void (*amp_watch_f)(struct ml_env_t *env, void *arg);

/**
 * RT structure.
 *   @engine: The engine.
 *   @watch: The watch function.
 *   @status: The status.
 *   @start: Start the engine.
 *   @stop: Stop the engine.
 *   @seek: Seek to a new time.
 */
struct amp_rt_t {
	struct amp_engine_t *engine;
	void (*watch)(struct amp_engine_t *, amp_watch_f, void *);
	bool (*status)(struct amp_engine_t *);
	void (*start)(struct amp_engine_t *);
	void (*stop)(struct amp_engine_t *);
	void (*seek)(struct amp_engine_t *, double bar);
};

/**
 * Engine structure.
 *   @run: The run flag.
 *   @core: The core.
 *   @notify: The notifier.
 *   @rev: The revision number.
 *   @lock, sync: The engine lock and synchronizer.
 *   @clock: The clock.
 *   @seq: The sequencer.
 *   @instr: The instrument.
 *   @rt: The AmpRT structure.
 *   @comm: MIDI device communcation.
 *   @watch: The watch list.
 */
struct amp_engine_t {
	bool run;
	struct amp_core_t *core;
	struct amp_notify_t *notify;

	unsigned int rev;
	sys_mutex_t lock, sync;
	struct amp_clock_t clock;
	struct amp_instr_t instr;

	struct amp_rt_t rt;
	struct amp_comm_t *comm;
	struct amp_watch_t *watch;
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

	if(value->type != ml_value_box_v)
		return NULL;

	return value->data.box.ref;
}

/**
 * Retrieve the status of the RT core.
 *   @rt: The RT core.
 *   &returns: The status.
 */
static inline bool amp_rt_status(struct amp_rt_t *rt)
{
	return rt->status(rt->engine);
}

/**
 * Start the RT core.
 *   @rt: The RT core.
 */
static inline void amp_rt_start(struct amp_rt_t *rt)
{
	rt->start(rt->engine);
}

/**
 * Stop the RT core.
 *   @rt: The RT core.
 */
static inline void amp_rt_stop(struct amp_rt_t *rt)
{
	rt->stop(rt->engine);
}

/**
 * Seek the RT core.
 *   @rt: The RT core.
 */
static inline void amp_rt_seek(struct amp_rt_t *rt, double bar)
{
	rt->seek(rt->engine, bar);
}


/*
 * export functions
 */
void amp_export_watch(struct amp_engine_t *engine, amp_watch_f func, void *arg);
bool amp_export_status(struct amp_engine_t *engine);
void amp_export_start(struct amp_engine_t *engine);
void amp_export_stop(struct amp_engine_t *engine);
void amp_export_seek(struct amp_engine_t *engine, double bar);

#endif
