#ifndef DEFS_H
#define DEFS_H

/**
 * Time structure.
 *   @idx, bar: The index and bar.
 *   @beat: The beat.
 */
struct amp_time_t {
	int idx, bar;
	double beat;
};

static inline struct amp_time_t amp_time(int bar, double beat)
{
	return (struct amp_time_t){ 0, bar, beat };
}

/**
 * Repeat a time.
 *   @time: The time.
 *   @off: The offset.
 *   @len: The length.
 *   &returns: The repeated time.
 */
static inline struct amp_time_t amp_time_repeat(struct amp_time_t time, int off, unsigned int len)
{
	time.bar = ((time.bar + off) % len + len) % len;

	return time;
}

/**
 * Modulus a time.
 *   @time: The time.
 *   @mod: The modulus.
 */
static inline struct amp_time_t amp_time_mod(struct amp_time_t time, struct amp_time_t mod)
{
	if(mod.idx > 0)
		time.idx %= mod.idx;

	if(mod.bar > 0)
		time.bar %= mod.bar;

	if(mod.beat > 0)
		time.beat = fmod(time.beat, mod.beat);

	return time;
}

/**
 * Compare two times.
 *   @left: The left time.
 *   @right: The right time.
 *   &returns: Their order.
 */
static inline int amp_time_cmp(struct amp_time_t left, struct amp_time_t right)
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
 * Check if a time falls between two other times.
 *   @time: The time.
 *   @left: The left time.
 *   @right: The right time.
 *   &returns: True if between.
 */
static inline bool amp_time_between(struct amp_time_t time, struct amp_time_t left, struct amp_time_t right)
{
	if(amp_time_cmp(left, right) < 0)
		return (amp_time_cmp(left, time) <= 0) && (amp_time_cmp(time, right) < 0);
	else
		return (amp_time_cmp(left, time) <= 0) || (amp_time_cmp(time, right) < 0);
}

static inline bool amp_time_chkbeat(struct amp_time_t time, struct amp_time_t left, struct amp_time_t right)
{
	if(amp_time_cmp(left, right) < 0)
		return (amp_time_cmp(left, time) <= 0) && (amp_time_cmp(time, right) < 0);
	else
		return (amp_time_cmp(left, time) <= 0) || (amp_time_cmp(time, right) < 0);
}

/**
 * Calculate a time given an index and beat parameters.
 *   @idx: The index.
 *   @bpm: The beats-per-minute.
 *   @nbeats: The beats-per-measure.
 *   @rate: The sample rate.
 *   &returns: The time.
 */
static inline struct amp_time_t amp_time_calc(int idx, double bpm, double nbeats, unsigned int rate)
{
	double beat;
	struct amp_time_t time;
	
	beat = (idx * bpm) / (rate * 60);

	time.idx = idx;
	time.bar = beat / nbeats;
	time.beat = beat - time.bar * nbeats;

	if(time.beat < 0) {
		time.beat += nbeats;
		time.bar -= 1;
	}

	return time;
}

static inline bool amp_time_isequal(struct amp_time_t left, struct amp_time_t right)
{
	return (left.bar == right.bar) && (left.beat == right.beat);
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
 *   @amp_info_seek_e: Seek.
 *   @amp_info_start_e: Start the clock.
 *   @amp_info_stop_e: Stop the clock.
 */
enum amp_info_e {
	amp_info_init_e,
	amp_info_commit_e,
	amp_info_action_e,
	amp_info_note_e,
	amp_info_seek_e,
	amp_info_start_e,
	amp_info_stop_e,
};

/**
 * Information union.
 *   @action: The action.
 *   @note: The note.
 *   @seek: The seek information.
 */
union amp_info_u {
	struct amp_action_t *action;
	struct amp_note_t *note;
	struct amp_seek_t *seek;
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
 * Create a seek information structure.
 *   @seek: The seek.
 *   &returns: The information structure.
 */

static inline struct amp_info_t amp_info_seek(struct amp_seek_t *seek)
{
	return (struct amp_info_t){ amp_info_seek_e, (union amp_info_u){ .seek = seek } };
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

/*
static inline int8_t ml_key_octave(int16_t key)
{
	return key - ;
}
*/

#endif
