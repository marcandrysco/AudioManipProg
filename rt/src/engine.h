#ifndef ENGINE_H
#define ENGINE_H

/*
 * exported header
 */
#include "inc.h"

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

/**
 * Engine structure.
 *   @run, toggle: The run and toggle flag.
 *   @core: The core.
 *   @notify: The notifier.
 *   @rev: The revision number.
 *   @lock, sync: The engine lock and synchronizer.
 *   @clock: The clock.
 *   @seq: The sequencer.
 *   @instr: The instrument.
 *   @effect: The effect.
 *   @rt: The AmpRT structure.
 *   @comm: MIDI device communcation.
 *   @watch: The watch list.
 */
struct amp_engine_t {
	bool run, toggle;
	struct amp_core_t *core;
	struct amp_notify_t *notify;

	unsigned int rev;
	sys_mutex_t lock, sync;
	struct amp_clock_t clock;
	struct amp_seq_t seq;
	struct amp_instr_t instr;
	struct amp_effect_t effect[2];

	struct amp_rt_t rt;
	struct amp_comm_t *comm;
	struct amp_watch_t *watch;
};


/*
 * engine declarations
 */

void amp_engine_watch(struct amp_engine_t *engine, amp_watch_f func, void *arg);

#endif
