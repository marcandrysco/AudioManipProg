#include "common.h"


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
