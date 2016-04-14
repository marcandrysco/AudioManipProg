#define _GNU_SOURCE
#include <pulse/pulseaudio.h>
#include <pthread.h>
#include "../common.h"
#include "audio.h"


/**
 * Pulse audio structure.
 *   @conn: The connection.
 *   @conf: The configuration.
 *   @func: The audio function.
 *   @arg: The function argument.
 */
struct pulse_audio_t {
	struct pulse_conn_t *conn;
	struct pulse_conf_t conf;

	amp_audio_f func;
	void *arg;
};


/*
 * local declarations
 */
static void onaudio(double **buf, unsigned int len, void *arg);

/*
 * global declarations
 */
const struct amp_audio_i pulse_audio_iface = {
	(amp_audio_open_f)pulse_audio_open,
	(amp_audio_close_f)pulse_audio_close,
	(amp_audio_exec_f)pulse_audio_exec,
	(amp_audio_halt_f)pulse_audio_halt
};



/**
 * Open an audio device.
 *   @conf: The configuration.
 *   &returns: The device or null on error.
 */
struct pulse_audio_t *pulse_audio_open(const char *conf)
{
	struct pulse_audio_t *audio;

	audio = malloc(sizeof(struct pulse_audio_t));
	audio->conf = (struct pulse_conf_t){ 2, 2, 48000, 150.0f };
	//audio->conf;

	return audio;
}

/**
 * Close a pulse audio device.
 *   @audio: The audio device.
 */
void pulse_audio_close(struct pulse_audio_t *audio)
{
	free(audio);
}


/**
 * Start executing a pulse audio device.
 *   @audio: The audio device.
 *   @func: The audio function.
 *   @arg: The argument.
 */
void pulse_audio_exec(struct pulse_audio_t *audio, amp_audio_f func, void *arg)
{
	audio->func = func;
	audio->arg = arg;
	audio->conn = pulse_conn_open("AMP-RealTime", onaudio, audio, &audio->conf);
}

/**
 * Hlat execution a pulse audio device.
 *   @audio: The audio device.
 */
void pulse_audio_halt(struct pulse_audio_t *audio)
{
	pulse_conn_close(audio->conn);
}


/**
 * Callback when data is ready.
 *   @buf: The buffer.
 *   @len: The length.
 *   @arg: The argument.
 */
static void onaudio(double **buf, unsigned int len, void *arg)
{
	struct pulse_audio_t *audio = arg;

	audio->func(buf, len, audio->arg);
}
