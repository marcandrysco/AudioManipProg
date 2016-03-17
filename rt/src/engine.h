#ifndef ENGINE_H
#define ENGINE_H

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
 *   @comm: MIDI device communcation.
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

	struct amp_comm_t *comm;
};

#endif
