#define _GNU_SOURCE
#include <alsa/asoundlib.h>
#include <pthread.h>
#include "../common.h"


/**
 * The interface structure.
 *   @seq: The sequencer.
 *   @rd, wr: The read and write pipes.
 *   @cnt: The reference count.
 *   @midi: The MIDI device list.
 */
struct iface_t {
	int rd[2], wr[2];
	pthread_t thread;

	unsigned int cnt;
	struct alsa_midi_t *midi;
};

/**
 * The MIDI device structure.
 *   @port: The port.
 *   @conf: The configuration.
 *   @func: The callback function.
 *   @arg: The callback argument.
 *   @next: The next MIDI device.
 */
struct alsa_midi_t {
	int port;

	const char *conf;
	amp_midi_f func;
	void *arg;

	struct alsa_midi_t *next;
};


/*
 * local declarations
 */
static struct iface_t *iface = NULL;

static void *iface_thread(void *arg);

//static void ref_add(struct alsa_midi_t *midi, int client);
//static void *ref_thread(void *arg);


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
	struct alsa_midi_t *midi;

	if(iface == NULL) {
		int err;

		iface = malloc(sizeof(struct iface_t));
		iface->cnt = 1;
		iface->midi = NULL;

		if((pipe(iface->rd) < 0) || (pipe(iface->wr) < 0))
			fatal("Failed to create synchronization pipe. %s (%d).\n", strerror(errno), errno);

		err = pthread_create(&iface->thread, NULL, iface_thread, NULL);
		if(err != 0)
			fatal("Failed to start thread. %s (%d).", strerror(err), err);
	}
	else
		iface->cnt++;

	midi = malloc(sizeof(struct alsa_midi_t));
	midi->conf = conf;
	midi->func = func;
	midi->arg = arg;
	midi->next = NULL;

	if(write(iface->wr[1], &midi, sizeof(void *)) < 0)
		fatal("Failed to write to synchronization pipe. %s (%d).", strerror(errno), errno);

	if(read(iface->rd[0], &midi, sizeof(void *)) < 0)
		fatal("Failed to read from synchronization pipe. %s (%d).", strerror(errno), errno);

	return midi;
}

/**
 * Close an ALSA MIDI device.
 *   @midi: The MIDI device.
 */
void alsa_midi_close(struct alsa_midi_t *midi)
{
	if(write(iface->wr[1], &midi, sizeof(void *)) < 0)
		fatal("Failed to write to synchronization pipe. %s (%d).", strerror(errno), errno);

	if(read(iface->rd[0], &midi, sizeof(void *)) < 0)
		fatal("Failed to read from synchronization pipe. %s (%d).", strerror(errno), errno);

	free(midi);

	if(--iface->cnt > 0)
		return;

	midi = NULL;
	if(write(iface->wr[1], &midi, sizeof(void *)) < 0)
		fatal("Failed to write to synchronization pipe. %s (%d).", strerror(errno), errno);

	pthread_join(iface->thread, NULL);

	close(iface->rd[0]);
	close(iface->rd[1]);
	close(iface->wr[0]);
	close(iface->wr[1]);
	free(iface);
}

/*
 * global variables
 */
struct amp_midi_i alsa_midi_iface = {
	(amp_midi_open_f)alsa_midi_open,
	(amp_midi_close_f)alsa_midi_close
};


/**
 * The interface thread.
 *   @arg: The device argument.
 *   &returns: Always 'NULL'.
 */
static void *iface_thread(void *arg)
{
	int err;
	snd_seq_t *seq;

	err = snd_seq_open(&seq, "default", SND_SEQ_OPEN_INPUT, 0);
	if(err < 0)
		fatal("Failed to open ALSA sequencer. %s (%d).", strerror(-err), -err);

	err = snd_seq_set_client_name(seq, "AmpRT");
	if(err < 0)
		fatal("Failed to rename sequencer. %s (%d).", strerror(-err), -err);

	{
		unsigned short revents;
		snd_seq_event_t *event;
		unsigned int nfds = snd_seq_poll_descriptors_count(seq, POLLIN) + 1;
		struct pollfd fdset[nfds];

		snd_seq_poll_descriptors(seq, fdset, nfds, POLLIN);
		fdset[nfds-1].fd = iface->wr[0];
		fdset[nfds-1].events = POLLIN;

		while(true) {
			poll(fdset, nfds, -1);

			if(fdset[nfds-1].revents) {
				struct alsa_midi_t *midi, **ref;

				if(read(iface->wr[0], &midi, sizeof(void *)) < 0)
					fatal("Failed to read from synchronization pipe. %s (%d).", strerror(errno), errno);

				if(midi == NULL)
					break;
 
				for(ref = &iface->midi; *ref != NULL; ref = &(*ref)->next) {
					if(*ref == midi)
						break;
				}

				if(*ref == NULL) {
					*ref = midi;
					midi->port = snd_seq_create_simple_port(seq, midi->conf, SND_SEQ_PORT_CAP_WRITE | SND_SEQ_PORT_CAP_SUBS_WRITE, SND_SEQ_PORT_TYPE_MIDI_GENERIC | SND_SEQ_PORT_TYPE_APPLICATION);
					if(midi->port < 0)
						fatal("Failed to create port. %s (%d).", strerror(-midi->port), -midi->port);

					err = snd_seq_connect_from(seq, midi->port, alsa_midi_find(midi->conf), 0);
					if(err < 0)
						fatal("Could not connect to MIDI device. %s (%d).\n", strerror(-err), -err);
				}
				else {
					snd_seq_delete_simple_port(seq, midi->port);
					*ref = (*ref)->next;
				}

				if(write(iface->rd[1], &midi, sizeof(void *)) < 0)
					fatal("Failed to read from synchronization pipe. %s (%d).", strerror(errno), errno);
			}

			snd_seq_poll_descriptors_revents(seq, fdset, nfds-1, &revents);
			if(revents & POLLIN) {
				uint16_t key, val;
				struct alsa_midi_t *midi;

				snd_seq_event_input(seq, &event);

				for(midi = iface->midi; midi != NULL; midi = midi->next) {
					if(midi->port == event->dest.port)
						break;
				}

				if(midi == NULL)
					fatal("Invalid event.");

				switch(event->type) {
				case SND_SEQ_EVENT_NOTEON:
					key = (uint16_t)event->data.note.note;
					val = (uint16_t)event->data.note.velocity << 9;
					printf("on: %d\n", key);
					midi->func(key, val, midi->arg);
					break;

				case SND_SEQ_EVENT_NOTEOFF:
					key = (uint16_t)event->data.note.note;
					midi->func(key, 0, midi->arg);
					break;

				case SND_SEQ_EVENT_CONTROLLER:
					key = (uint16_t)event->data.control.param + 0x100;
					val = (uint16_t)event->data.control.value << 9;

					if(key == 320)
						midi->func(128, val, midi->arg);
					else
						midi->func(key, val, midi->arg);
					break;

				case SND_SEQ_EVENT_PGMCHANGE:
					key = (uint16_t)event->data.control.value + 0x200;
					val = UINT16_MAX;
					midi->func(key, val, midi->arg);
					break;
				}
			}
		}
	}

	snd_seq_close(seq);

	return NULL;
}


#if 0

/**
 * MIDI device structure.
 *   @port: The port number.
 *   @func: The callback function.
 *   @arg: The argument.
 *   @next: The next device.
 */
struct alsa_midi_t {
	int port;

	amp_midi_f func;
	void *arg;

	struct alsa_midi_t *next;
};

/**
 * Thread reference structure.
 *   @seq: The sequencer reference.
 *   @pipe: The synchronization pipe.
 *   @thread: The thread.
 *   @id: The current identifier.
 *   @list: The device list.
 *   @cnt: The reference count.
 */
struct ref_t {
	snd_seq_t *seq;

	int pipe[2];
	pthread_t thread;

	int id;
	struct alsa_midi_t *list;
	unsigned int cnt;
};


/*
 * local declarations
 */
static struct ref_t *midi_ref = NULL;

static void ref_add(struct alsa_midi_t *midi, int client);
static void *ref_thread(void *arg);

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
	int client;
	struct alsa_midi_t *midi;

	client = alsa_midi_find(conf);
	if(client < 0)
		fprintf(stderr, "No such device '%s'.\n", conf);

	midi = malloc(sizeof(struct alsa_midi_t));
	midi->func = func;
	midi->arg = arg;
	ref_add(midi, client);

	return midi;
}

/**
 * Close an ALSA MIDI device.
 *   @midi: The MIDI device.
 */
void alsa_midi_close(struct alsa_midi_t *midi)
{
	int client;
	struct alsa_midi_t **ref = &midi_ref->list;

	while(*ref != midi)
		ref = &(*ref)->next;

	*ref = (*ref)->next;
	free(midi);

	if(--midi_ref->cnt > 0)
		return;

	client = 0;
	if(write(midi_ref->pipe[1], &client, sizeof(client)) < 0)
		fatal("Failed to write to synchronization pipe. %s (%d).", strerror(errno), errno);

	pthread_join(midi_ref->thread, NULL);

	close(midi_ref->pipe[0]);
	close(midi_ref->pipe[1]);
	snd_seq_close(midi_ref->seq);
	free(midi_ref);

	midi_ref = NULL;
}


/**
 * Add a device to the reference.
 *   @midi: The device to insert.
 */
static void ref_add(struct alsa_midi_t *midi, int client)
{
	if(midi_ref == NULL) {
		int err;

		midi_ref = malloc(sizeof(struct alsa_midi_t));
		midi_ref->cnt = 1;
		midi_ref->list = NULL;
		midi_ref->id = 1;
		snd_seq_open(&midi_ref->seq, "default", SND_SEQ_OPEN_INPUT, 0);

		if(pipe(midi_ref->pipe) < 0)
			fprintf(stderr, "Failed to create synchronization pipe. %s.\n", strerror(errno)), exit(1);

		err = snd_seq_set_client_name(midi_ref->seq, "AmpRT");
		if(err != 0)
			fprintf(stderr, "Failed to rename sequencer. %s.\n", strerror(-err)), exit(1);

		err = snd_seq_create_simple_port(midi_ref->seq, "AmpRT-Input", SND_SEQ_PORT_CAP_WRITE | SND_SEQ_PORT_CAP_SUBS_WRITE, SND_SEQ_PORT_TYPE_MIDI_GENERIC | SND_SEQ_PORT_TYPE_APPLICATION);
		if(err != 0)
			fprintf(stderr, "Failed to create port. %s.\n", strerror(-err)), exit(1);

		err = pthread_create(&midi_ref->thread, NULL, ref_thread, midi_ref);
		if(err != 0)
			fprintf(stderr, "Failed to start thread. %s.\n", strerror(err)), exit(1);
	}
	else
		midi_ref->cnt++;

	midi->port = midi_ref->id++;
	midi->next = midi_ref->list;
	midi_ref->list = midi;

	if(write(midi_ref->pipe[1], &client, sizeof(int)) < 0)
		fatal("Failed to write to synchronization pipe. %s (%d).", strerror(errno), errno);

	if(write(midi_ref->pipe[1], &midi->port, sizeof(int)) < 0)
		fatal("Failed to write to synchronization pipe. %s (%d).", strerror(errno), errno);
}


/**
 * The PCM device thread.
 *   @arg: The device argument.
 *   &returns: Always 'NULL'.
 */
static void *ref_thread(void *arg)
{
	struct ref_t *ref = arg;
	unsigned int nfds = snd_seq_poll_descriptors_count(ref->seq, POLLIN) + 1;
	struct pollfd fdset[nfds];
	snd_seq_event_t *event;
	unsigned short revents;

	snd_seq_poll_descriptors(ref->seq, fdset, nfds, POLLIN);
	fdset[nfds-1].fd = ref->pipe[0];
	fdset[nfds-1].events = POLLIN;

	while(true) {
		poll(fdset, nfds, -1);

		if(fdset[nfds-1].revents) {
			int err, client, port;

			if(read(ref->pipe[0], &client, sizeof(int)) < 0)
				fatal("Failed to read from synchronization pipe. %s (%d).", strerror(errno), errno);

			if(client == 0)
				break;

			if(read(ref->pipe[0], &port, sizeof(int)) < 0)
				fatal("Failed to read from synchronization pipe. %s (%d).", strerror(errno), errno);

			err = snd_seq_connect_from(midi_ref->seq, port, client, 0);
			if(err != 0)
				fprintf(stderr, "Could not connect to MIDI device. %s.\n", strerror(-err)), exit(1);
		}

		snd_seq_poll_descriptors_revents(ref->seq, fdset, nfds-1, &revents);
		if(revents & POLLIN) {
			uint16_t key, val;

			snd_seq_event_input(ref->seq, &event);

			switch(event->type) {
			case SND_SEQ_EVENT_NOTEON:
			case SND_SEQ_EVENT_NOTEOFF:
				key = (uint16_t)event->data.note.note;
				val = (uint16_t)event->data.note.velocity << 9;
				//ref->func(key, val, ref->arg);
				break;

			case SND_SEQ_EVENT_CONTROLLER:
				key = (uint16_t)event->data.control.param + 0x100;
				val = (uint16_t)event->data.control.value << 9;
				//ref->func(key, val, ref->arg);
				break;

			case SND_SEQ_EVENT_PGMCHANGE:
				key = (uint16_t)event->data.control.value + 0x200;
				val = UINT16_MAX;
				//ref->func(key, val, ref->arg);
				break;
			}

			if(key&&val);
		}
	}

	return NULL;
}


#endif
