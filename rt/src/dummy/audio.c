#include "../common.h"


/**
 * Dummy audio structure.
 *   @rate: The sample rate.
 *   @func: The processing function.
 *   @arg: The argument.
 *   @task: The task.
 */
struct dummy_audio_t {
	unsigned int rate;

	amp_audio_f func;
	void *arg;

	struct sys_task_t *task;
};


/*
 * global variables.
 */
const struct amp_audio_i dummy_audio_iface = {
	(amp_audio_open_f)dummy_audio_open,
	(amp_audio_close_f)dummy_audio_close,
	(amp_audio_exec_f)dummy_audio_exec,
	(amp_audio_halt_f)dummy_audio_halt,
	(amp_audio_info_f)dummy_audio_info
};


static void audio_task(sys_fd_t fd, void *arg)
{
	struct dummy_audio_t *audio = arg;
	unsigned int len;
	
	len = audio->rate / 20;

	while(sys_poll1(fd, sys_poll_in_e, 1000 / 20) == 0) {
		double data[2][len], *buf[2];

		buf[0] = data[0];
		buf[1] = data[1];

		dsp_zero_d(buf[0], len);
		dsp_zero_d(buf[1], len);
		audio->func(buf, len, audio->arg);
	}
}


/**
 * Open an audio device.
 *   @conf: The configuration.
 *   &returns: The device or null on error.
 */
struct dummy_audio_t *dummy_audio_open(const char *conf)
{
	struct dummy_audio_t *audio;

	audio = malloc(sizeof(struct dummy_audio_t));
	audio->rate = 96000;
	audio->func = NULL;

	return audio;
}

/**
 * Close an audio device.
 *   @audio: The audio.
 */
void dummy_audio_close(struct dummy_audio_t *audio)
{
	if(audio->func)
		dummy_audio_close(audio);

	free(audio);
}

/**
 * Begin executing the dummy audio device.
 *   @audio: The audio device.
 *   @func: The ready function.
 *   @arg: The argument.
 */
void dummy_audio_exec(struct dummy_audio_t *audio, amp_audio_f func, void *arg)
{
	audio->func = func;
	audio->arg = arg;
	audio->task = sys_task_new(audio_task, audio);
}

/**
 * Halt the dummy audio device.
 *  @audio: The audio device.
 */
void dummy_audio_halt(struct dummy_audio_t *audio)
{
	sys_task_delete(audio->task);
	audio->func = NULL;
}


/**
 * Retrieve audio information.
 *   @audio: The audio device.
 *   &returns: The information structure.
 */
struct amp_audio_info_t dummy_audio_info(struct dummy_audio_t *audio)
{
	return (struct amp_audio_info_t){ audio->rate };
}
