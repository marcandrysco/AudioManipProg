#ifndef DEFS_H
#define DEFS_H

/*
 * prerequisite headers
 */
#include <acw.h>


/**
 * Time structure.
 *   @idx: The index.
 *   @bar, beat: The bar and beat.
 */
struct amp_time_t {
	int idx;
	double bar, beat;
};
static inline struct amp_time_t amp_time(double bar, double beat)
{
	return (struct amp_time_t){ 0, bar, beat };
}

/**
 * Location structure.
 *   @bar: The bar.
 *   @beat: The beat.
 */
struct amp_loc_t {
	int bar;
	double beat;
};
static inline struct amp_loc_t amp_loc(int bar, double beat) {
	return (struct amp_loc_t){ bar, beat };
}

/**
 * Compare two loations.
 *   @left: The left location.
 *   @right: The right location.
 *   &returns: Their order.
 */
static inline int amp_loc_cmp(struct amp_loc_t left, struct amp_loc_t right)
{
	if(left.bar > right.bar)
		return 2;
	else if(left.bar < right.bar)
		return -2;
	else if(left.beat > right.beat)
		return 1;
	else if(left.beat < right.beat)
		return -1;
	else
		return 0;
}

/**
 * Check if two locations are near eachother, close enough to be considered
 * the "same".
 *   @left; The left location.
 *   @right: The right location.
 *   &returns: True if near.
 */
static inline bool amp_loc_near(struct amp_loc_t left, struct amp_loc_t right)
{
	if(left.bar != right.bar)
		return false;

	return fabs(left.beat - right.beat) < 1e-5;
}

/**
 * Compute a location using an index and bar information.
 *   @idx: The index.
 *   @ndivs: The number of divisoins.
 *   @nbeats: The number of beats per bar.
 *   &returns: The location.
 */
static inline struct amp_loc_t amp_loc_idx(unsigned int idx, unsigned int ndivs, double nbeats)
{
	struct amp_loc_t loc;

	loc.bar = idx / (ndivs * nbeats);
	loc.beat = (double)(idx - (ndivs * nbeats) * loc.bar) / (double)ndivs;

	return loc;
}


/**
 * Copy function.
 *   @ref: The reference.
 *   &returns: The copy.
 */
typedef void *(*amp_copy_f)(void *ref);

/**
 * Deletion function.
 *   @ref: The reference.
 */
typedef void (*amp_delete_f)(void *ref);


/**
 * MIDI function.
 *   @key: The key.
 *   @val: The value.
 *   @arg: The argument.
 */
typedef void (*amp_midi_f)(uint16_t key, uint16_t val, void *arg);


/**
 * Identifier structure.
 *   @dev, key: The device and key identifier.
 */
struct amp_id_t {
	uint16_t dev, key;
};

/**
 * Event structure.
 *   @dev, key, val: The device, key, and value.
 */
struct amp_event_t {
	uint16_t dev, key, val;
};
static inline struct amp_event_t amp_event(uint16_t dev, uint16_t key, uint16_t val)
{
	return (struct amp_event_t){ dev, key, val };
}

/**
 * Action structure.
 *   @delay: The delay.
 *   @event: The event.
 *   @queue: The queue.
 */
struct amp_action_t {
	unsigned int delay;
	struct amp_event_t event;
	struct amp_queue_t *queue;
};
static inline struct amp_action_t amp_action(unsigned int delay, struct amp_event_t event, struct amp_queue_t *queue)
{
	return (struct amp_action_t){ delay, event, queue };
}

/**
 * Event function.
 *   @ref: The reference.
 *   @event: The event.
 *   @delay: The delay.
 */
typedef void (*amp_event_f)(void *ref, struct amp_event_t event, unsigned int delay);


/**
 * Note structure.
 *   @init; The init flag.
 *   @delay: The deley.
 *   @key: The key.
 *   @freq, vel: The frequency and velocity.
 */
struct amp_note_t {
	bool init;
	unsigned int delay;
	uint16_t key;
	double freq, vel;
};

/**
 * Seek structure.
 *   @idx: The index.
 *   @time: The time.
 */
struct amp_seek_t {
	int idx;
	struct amp_time_t time;
};

/**
 * Information enumeration.
 *   @amp_info_init_e: Initialize components.
 *   @amp_info_commit_e: Commit data.
 *   @amp_info_action_e: Event action.
 *   @amp_info_note_e: Note setup.
 *   @amp_info_tell_e: Tell.
 *   @amp_info_loc_v: Current location.
 *   @amp_info_seek_e: Seek.
 *   @amp_info_start_e: Start the clock.
 *   @amp_info_stop_e: Stop the clock.
 */
enum amp_info_e {
	amp_info_init_e,
	amp_info_commit_e,
	amp_info_action_e,
	amp_info_note_e,
	amp_info_tell_e,
	amp_info_loc_v,
	amp_info_seek_e,
	amp_info_start_e,
	amp_info_stop_e,
};

/**
 * Information union.
 *   @loc: The location.
 *   @action: The action.
 *   @note: The note.
 *   @seek: The seek information.
 *   @num: Integer number.
 *   @flt: Floating-point number.
 */
union amp_info_u {
	struct amp_loc_t *loc;
	struct amp_action_t *action;
	struct amp_note_t *note;
	struct amp_seek_t *seek;
	int *num;
	double *flt;
};

/**
 * Information structure.
 *   @type: The type.
 *   @data: The data.
 */
struct amp_info_t {
	enum amp_info_e type;
	union amp_info_u data;
};

/**
 * Information function.
 *   @ref: The reference.
 *   @info: The information structure.
 */
typedef void (*amp_info_f)(void *ref, struct amp_info_t info);


/**
 * Create an init information strucure.
 *   &returns: The information structure.
 */
static inline struct amp_info_t amp_info_init(void)
{
	return (struct amp_info_t){ amp_info_init_e, (union amp_info_u){ } };
}

/**
 * Create a commit information strucure.
 *   &returns: The information structure.
 */
static inline struct amp_info_t amp_info_commit(void)
{
	return (struct amp_info_t){ amp_info_commit_e, (union amp_info_u){ } };
}

/**
 * Create an action information structure.
 *   @action: The action.
 *   &returns: The information structure.
 */
static inline struct amp_info_t amp_info_action(struct amp_action_t *action)
{
	return (struct amp_info_t){ amp_info_action_e, (union amp_info_u){ .action = action } };
}

/**
 * Create a note information structure.
 *   @note: The note.
 *   &returns: The information structure.
 */
static inline struct amp_info_t amp_info_note(struct amp_note_t *note)
{
	return (struct amp_info_t){ amp_info_note_e, (union amp_info_u){ .note = note } };
}

/**
 * Create a tell information structure.
 *   @bar: The returned bar.
 *   &returns: The information structure.
 */
static inline struct amp_info_t amp_info_tell(double *bar)
{
	return (struct amp_info_t){ amp_info_tell_e, (union amp_info_u){ .flt = bar } };
}

/**
 * Create a location information structure.
 *   @loc: Ref. The location.
 *   &returns: The information structure.
 */
static inline struct amp_info_t amp_info_loc(struct amp_loc_t *loc)
{
	return (struct amp_info_t){ amp_info_loc_v, (union amp_info_u){ .loc = loc } };
}

/**
 * Create a seek information structure.
 *   @bar: The target bar.
 *   &returns: The information structure.
 */
static inline struct amp_info_t amp_info_seek(double *bar)
{
	return (struct amp_info_t){ amp_info_seek_e, (union amp_info_u){ .flt = bar } };
}

/**
 * Create a start information structure.
 *   @seek: The seek.
 *   &returns: The information structure.
 */
static inline struct amp_info_t amp_info_start(struct amp_seek_t *seek)
{
	return (struct amp_info_t){ amp_info_start_e, (union amp_info_u){ .seek = seek } };
}

/**
 * Create a stop information structure.
 *   @seek: The seek.
 *   &returns: The information structure.
 */
static inline struct amp_info_t amp_info_stop(struct amp_seek_t *seek)
{
	return (struct amp_info_t){ amp_info_stop_e, (union amp_info_u){ .seek = seek } };
}


/**
 * Compute a velocity from a value.
 *   @val: The value.
 *   &returns: The velocity.
 */
static inline double amp_event_vel(uint16_t val)
{
	return (double)val / (double)UINT16_MAX;
}

#endif
