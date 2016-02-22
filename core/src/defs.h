#ifndef DEFS_H
#define DEFS_H

/**
 * Time structure.
 *   @bar: The bar.
 *   @beat: The beat.
 */

struct amp_time_t {
	int bar;
	double beat;
};


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
 * Event structure.
 *   @dev, key, val: The device, key, and value.
 */

struct amp_event_t {
	uint16_t dev, key, val;
};

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
 *   @key: The key.
 *   @freq, vel: The frequency and velocity.
 */

struct amp_note_t {
	bool init;
	uint16_t key;
	double freq, vel;
};

/**
 * Action structure.
 *   @delay: The delay.
 *   @event: The event.
 */

struct amp_action_t {
	unsigned int delay;
	struct amp_event_t event;
};

/**
 * Information enumeration.
 *   @amp_action_e: Event action.
 *   @amp_start_e: Start the clock.
 *   @amp_stop_e: Stop the clock.
 */

enum amp_info_e {
	amp_info_action_e,
	amp_info_note_e,
	amp_info_start_e,
	amp_info_stop_e,
};

/**
 * Information union.
 *   @action: The action.
 *   @note: The note.
 *   @time: The start/stop time.
 */

union amp_info_u {
	struct amp_action_t *action;
	struct amp_note_t *note;
	struct amp_time_t *time;
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
 * Retrieve a frequency for a note as a float.
 *   @note: The note.
 *   &returns: The frequency.
 */

static inline double amp_key_freq_f(int16_t note)
{
	return 27.5f * powf(2.0f, ((float)note - 9.0f) / 12.0f);
}

#endif
