#define _GNU_SOURCE
#include <alsa/asoundlib.h>
#include <pthread.h>
#include "../common.h"


/**
 * MIDI device structure.
 *   @seq: The sequencer reference.
 *   @pipe: The synchronization pipe.
 *   @thread: The thread.
 *   @func: The callback function.
 *   @arg: The argument.
 *   @task: The processing task.
 */

struct alsa_midi_t {
	snd_seq_t *seq;

	int pipe[2];
	pthread_t thread;

	amp_midi_f func;
	void *arg;
};


/*
 * local declarations
 */

static void *midi_thread(void *arg);

/*
 * global variables
 */

struct amp_midi_i alsa_midi_iface = {
	(amp_midi_open_f)alsa_midi_open,
	(amp_midi_close_f)alsa_midi_close
};


/**
 * Find a MIDI client given an identifier.
 *   @id: The identifier.
 *   &returns: The client number if found, negative otherwise.
 */

int alsa_midi_find(const char *id)
{
	int client = -1;

	snd_seq_t *seq;
	snd_seq_client_info_t *info;

	snd_seq_client_info_alloca(&info);
	snd_seq_client_info_set_client(info, -1);

	snd_seq_open(&seq, "default", SND_SEQ_OPEN_INPUT, 0);

	while(true) {
		if(snd_seq_query_next_client(seq, info) < 0)
			break;

		if(strcmp(snd_seq_client_info_get_name(info), id) != 0)
			continue;

		client = snd_seq_client_info_get_client(info);

		break;
	}

	snd_seq_close(seq);

	return client;
}


/**
 * Open a ALSA MIDI device.
 *   @conf: The configuration.
 *   @func: The callback function.
 *   @arg: The callback argument.
 *   &returns: The MIDI device.
 */

struct alsa_midi_t *alsa_midi_open(const char *conf, amp_midi_f func, void *arg)
{
	int err, client;
	struct alsa_midi_t *midi;

	client = alsa_midi_find(conf);
	if(client < 0)
		fprintf(stderr, "No such device '%s'.\n", conf);

	midi = malloc(sizeof(struct alsa_midi_t));
	midi->func = func;
	midi->arg = arg;
	snd_seq_open(&midi->seq, "default", SND_SEQ_OPEN_INPUT, 0);

	if(pipe(midi->pipe) < 0)
		fprintf(stderr, "Failed to create synchronization pipe. %s.\n", strerror(errno)), exit(1);

	err = snd_seq_set_client_name(midi->seq, "AmpRT");
	if(err != 0)
		fprintf(stderr, "Failed to rename sequencer. %s.\n", strerror(-err)), exit(1);

	err = snd_seq_create_simple_port(midi->seq, "AmpRT-Input", SND_SEQ_PORT_CAP_WRITE | SND_SEQ_PORT_CAP_SUBS_WRITE, SND_SEQ_PORT_TYPE_MIDI_GENERIC | SND_SEQ_PORT_TYPE_APPLICATION);
	if(err != 0)
		fprintf(stderr, "Failed to create port. %s.\n", strerror(-err)), exit(1);

	err = snd_seq_connect_from(midi->seq, 0, client, 0);
	if(err != 0)
		fprintf(stderr, "Could not connect to MIDI device. %s.\n", strerror(-err)), exit(1);

	err = pthread_create(&midi->thread, NULL, midi_thread, midi);
	if(err != 0)
		fprintf(stderr, "Failed to start thread. %s.\n", strerror(err)), exit(1);

	return midi;
}

/**
 * Close an ALSA MIDI device.
 *   @midi: The MIDI device.
 */

void alsa_midi_close(struct alsa_midi_t *midi)
{
	int err;
	uint8_t cmd;

	cmd = 1;
	err = write(midi->pipe[1], &cmd, 1);
	if(err < 0)
		fprintf(stderr, "Failed to write to descriptor. %s.\n", strerror(errno)), exit(1);

	pthread_join(midi->thread, NULL);

	close(midi->pipe[0]);
	close(midi->pipe[1]);
	snd_seq_close(midi->seq);
	free(midi);
}


/**
 * The PCM device thread.
 *   @arg: The device argument.
 *   &returns: Always 'NULL'.
 */

static void *midi_thread(void *arg)
{
	struct alsa_midi_t *midi = arg;
	unsigned int nfds = snd_seq_poll_descriptors_count(midi->seq, POLLIN) + 1;
	struct pollfd fdset[nfds];
	snd_seq_event_t *event;
	unsigned short revents;

	snd_seq_poll_descriptors(midi->seq, fdset, nfds, POLLIN);
	fdset[nfds-1].fd = midi->pipe[0];
	fdset[nfds-1].events = POLLIN;

	while(true) {
		poll(fdset, nfds, -1);

		if(fdset[nfds-1].revents)
			break;

		snd_seq_poll_descriptors_revents(midi->seq, fdset, nfds-1, &revents);
		if(revents & POLLIN) {
			uint16_t key, val;

			snd_seq_event_input(midi->seq, &event);

			switch(event->type) {
			case SND_SEQ_EVENT_NOTEON:
			case SND_SEQ_EVENT_NOTEOFF:
				key = (uint16_t)event->data.note.note;
				val = (uint16_t)event->data.note.velocity << 9;

				midi->func(key, val, midi->arg);

				break;

			case SND_SEQ_EVENT_CONTROLLER:
				key = (uint16_t)event->data.control.param + 0x100;
				val = (uint16_t)event->data.control.value << 9;

				midi->func(key, val, midi->arg);

				break;
			}
		}
	}

	return NULL;
}
