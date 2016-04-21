#ifndef DEFS_H
#define DEFS_H

/**
 * Constant definitions
 *   @AMP_COMM_LEN: MIDI communications event ring length.
 */

#define AMP_COMM_LEN 32


/**
 * Format enumerator.
 *   @am_format_8bit_e: 8-bit.
 *   @am_format_16bit_e: 16-bit.
 *   @am_format_24bit_e: 24-bit.
 */
enum amp_format_e {
	amp_format_8bit_e,
	amp_format_16bit_e,
	amp_format_24bit_e
};


/**
 * Audio information structure.
 *   @rate: The sample rate.
 */
struct amp_audio_info_t {
	unsigned int rate;
};

/**
 * Audio processing function.
 *   @buf: The buffer.
 *   @len: The length.
 *   @arg: The argument.
 */
typedef void (*amp_audio_f)(double **buf, unsigned int len, void *arg);


/**
 * Audio execute function.
 *   @ref: The reference.
 *   @func: The function.
 *   @arg: The argument.
 */

typedef void (*amp_audio_exec_f)(void *ref, amp_audio_f func, void *arg);


/**
 * Audio open function.
 *   @conf: The configuration.
 *   &returns: The reference.
 */

typedef void *(*amp_audio_open_f)(const char *conf);

/**
 * Audio close function.
 *   @ref: The reference.
 */

typedef void *(*amp_audio_close_f)(void *ref);

/**
 * Audio execute function.
 *   @ref: The reference.
 *   @func: The callback function.
 *   @arg: The callback argument.
 */
typedef void (*amp_audio_exec_f)(void *ref, amp_audio_f func, void *arg);

/**
 * Audio halt function.
 *   @ref: The reference.
 *   @func: The function.
 *   @arg: The argument.
 */
typedef void (*amp_audio_halt_f)(void *ref);

/**
 * Retrieve Audio information.
 *   @ref: The reference.
 *   &returns: The information structure.
 */
typedef struct amp_audio_info_t (*amp_audio_info_f)(void *ref);

/**
 * Audio interface.
 *   @open: Open.
 *   @close: Close.
 *   @exec: Execute.
 *   @halt: Halt.
 */
struct amp_audio_i {
	amp_audio_open_f open;
	amp_audio_close_f close;
	amp_audio_exec_f exec;
	amp_audio_halt_f halt;
	amp_audio_info_f info;
};

/**
 * Audio device structure.
 *   @ref: The reference.
 *   @iface; The interface.
 */
struct amp_audio_t {
	void *ref;
	const struct amp_audio_i *iface;
};

/**
 * Open an audio device.
 *   @conf: The configuration.
 *   @iface: The interface.
 *   &returns: The audio device.
 */
static inline struct amp_audio_t amp_audio_open(const char *conf, const struct amp_audio_i *iface)
{
	return (struct amp_audio_t){ iface->open(conf), iface };
}

/**
 * Close an audio device.
 *   @audio: The audio device.
 */
static inline void amp_audio_close(struct amp_audio_t audio)
{
	audio.iface->close(audio.ref);
}

/**
 * Execute  an audio device.
 *   @audio: The audio device.
 *   @func: The function.
 *   @arg: The argument.
 */
static inline void amp_audio_exec(struct amp_audio_t audio, amp_audio_f func, void *arg)
{
	audio.iface->exec(audio.ref, func, arg);
}

/**
 * Halt an audio device.
 *   @audio: The audio device.
 */
static inline void amp_audio_halt(struct amp_audio_t audio)
{
	audio.iface->halt(audio.ref);
}

/**
 * Retrieve audio information from a device.
 *   @audio: The audio device.
 *   &returns: The information structure.
 */
static inline struct amp_audio_info_t amp_audio_info(struct amp_audio_t audio)
{
	return audio.iface->info(audio.ref);
}


/**
 * MIDI open function.
 *   @conf: The configuration.
 *   @func: The callack.
 *   @arg: The argument.
 *   &returns: The reference.
 */

typedef void *(*amp_midi_open_f)(const char *conf, amp_midi_f func, void *arg);

/**
 * MIDI close function.
 *   @ref: The reference.
 */

typedef void *(*amp_midi_close_f)(void *ref);

/**
 * MIDI interface.
 *   @open: Open.
 *   @close: Close.
 */

struct amp_midi_i {
	amp_midi_open_f open;
	amp_midi_close_f close;
};

/**
 * MIDI device structure.
 *   @ref: The reference.
 *   @iface; The interface.
 */

struct amp_midi_t {
	void *ref;
	const struct amp_midi_i *iface;
};

/**
 * Open a MIDI device.
 *   @conf: The configuration.
 *   @iface: The interface.
 *   @func: The callback function.
 *   @arg: The callback argument.
 *   &returns: The MIDI device.
 */

static inline struct amp_midi_t amp_midi_open(const char *conf, amp_midi_f func, void *arg, const struct amp_midi_i *iface)
{
	return (struct amp_midi_t){ iface->open(conf, func, arg), iface };
}

/**
 * Close a MIDI device.
 *   @midi: The MIDI device.
 */

static inline void amp_midi_close(struct amp_midi_t midi)
{
	midi.iface->close(midi.ref);
}

#endif
