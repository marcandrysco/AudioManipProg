#include "common.h"


void amp_export_watch(struct amp_engine_t *engine, amp_watch_f func, void *arg)
{
	amp_engine_watch(engine, func, arg);
}

/**
 * Status of the engine.
 *   @engine: The engine.
 *   &returns: The status.
 */
bool amp_export_status(struct amp_engine_t *engine)
{
	bool run;

	sys_mutex_lock(&engine->lock);
	run = amp_engine_status(engine);
	sys_mutex_unlock(&engine->lock);

	return run;
}

/**
 * Start the engine.
 *   @engine: The engine.
 */
void amp_export_start(struct amp_engine_t *engine)
{
	sys_mutex_lock(&engine->lock);
	amp_engine_start(engine);
	sys_mutex_unlock(&engine->lock);
}

/**
 * Stop the engine.
 *   @engine: The engine.
 */
void amp_export_stop(struct amp_engine_t *engine)
{
	sys_mutex_lock(&engine->lock);
	amp_engine_stop(engine);
	sys_mutex_unlock(&engine->lock);
}

/**
 * Seek the engine.
 *   @engine: The engine.
 *   @bar: The bar.
 */
void amp_export_seek(struct amp_engine_t *engine, double bar)
{
	sys_mutex_lock(&engine->lock);

	if(engine->run) {
		amp_engine_stop(engine);
		amp_engine_seek(engine, bar);
		amp_engine_start(engine);
	}
	else
		amp_engine_seek(engine, bar);

	sys_mutex_unlock(&engine->lock);
}
