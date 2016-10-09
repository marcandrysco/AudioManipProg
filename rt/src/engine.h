#ifndef ENGINE_H
#define ENGINE_H

/*
 * exported header
 */
#include "export.h"

/**
 * Watch structure.
 *   @func: The function.
 *   @arg: The argument.
 *   @next: The next watch.
 */
struct amp_watch_t {
	amp_watch_f func;
	void *arg;

	struct amp_watch_t *next;
};


/*
 * engine declarations
 */
struct amp_audio_t;

struct amp_engine_t *amp_engine_new(const char *file, struct amp_comm_t *comm, struct amp_audio_t audio);
void amp_engine_delete(struct amp_engine_t *engine);

void amp_engine_update(struct amp_engine_t *engine, const char *path);

void amp_engine_watch(struct amp_engine_t *engine, amp_watch_f func, void *arg);

bool amp_engine_status(struct amp_engine_t *engine);
void amp_engine_start(struct amp_engine_t *engine);
void amp_engine_stop(struct amp_engine_t *engine);
void amp_engine_seek(struct amp_engine_t *engine, double bar);

#endif
