#define _GNU_SOURCE
#include <errno.h>
#include <poll.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include <pulse/pulseaudio.h>
#include "../common.h"


/**
 * Pulse connection structure.
 *   @quit: The quit flag.
 *   @pipe: The communication pipe.
 *   @thread: The thread.
 *   @width, lat: The width and latency.
 *   @conf: The configuration.
 *   @rd, wr: The read and write indices.
 *   @loop: The main loop.
 *   @api: The API.
 *   @playback, record: The playback and record streams.
 *   @func: The ready function.
 *   @arg: The argument.
 *   @startup: The startup length.
 *   @context: The context.
 */

struct pulse_conn_t {
	uint8_t quit;
	int pipe[2];
	pthread_t thread;
	
	unsigned int width, lat;
	struct pulse_conf_t conf;
	unsigned int rd, wr, nrd, nwr;

	pa_mainloop *loop;
	pa_mainloop_api *api;
	pa_context *context;
	pa_stream *playback, *record;

	amp_audio_f func;
	void *arg;

	sig_atomic_t reset[2];
	unsigned int startup;
	float **buf;
};


/*
 * local function declarations
 */

static void *conn_thread(void *arg);

static int conn_poll(struct pollfd *ufds, unsigned long nfds, int timeout, void *arg);
static void conn_state(pa_context *context, void *arg);

static void conn_record(pa_stream *stream, size_t nbytes, void *arg);
static void conn_playback(pa_stream *stream, size_t nbytes, void *arg);
static void conn_overflow(pa_stream *stream, void *arg);
static void conn_underflow(pa_stream *stream, void *arg);


/**
 * Create a new connection.
 *   @name: The stream name.
 *   @conf: The configuration options.
 *   @func: The ready function.
 *   @arg: The arguemtn.
 *   &returns: The connection.
 */

struct pulse_conn_t *pulse_conn_open(const char *name, amp_audio_f func, void *arg, const struct pulse_conf_t *conf)
{
	int err;
	unsigned int i;
	pthread_attr_t attr;
	struct sched_param param;
	struct pulse_conn_t *conn;

	conn = malloc(sizeof(struct pulse_conn_t));
	conn->quit = 0;
	conn->width = (conf->in > conf->out) ? conf->in : conf->out;
	conn->lat = conf->rate * (conf->lat / 1000.0f);
	conn->conf = *conf;
	conn->func = func;
	conn->arg = arg;
	conn->rd = 0;
	conn->wr = conn->lat;
	conn->startup = 2;
	conn->reset[0] = conn->reset[1] = 1;
	conn->nrd = conn->nwr = conf->rate;

	//printf("width: %u\n", conn->width);

	conn->buf = malloc(conn->width * sizeof(void *));
	for(i = 0; i < conn->width; i++)
		memset(conn->buf[i] = malloc(2 * conn->lat * sizeof(float)), 0x00, 2 * conn->lat * sizeof(float));

	if(pipe(conn->pipe) < 0)
		fprintf(stderr, "Cannot create pipe."), exit(1);

	conn->loop = pa_mainloop_new();
	conn->api = pa_mainloop_get_api(conn->loop);
	pa_mainloop_set_poll_func(conn->loop, conn_poll, conn);

	conn->context = pa_context_new(conn->api, name);
	pa_context_set_state_callback(conn->context, conn_state, conn);
	pa_context_connect(conn->context, NULL, 0, NULL);

	err = pthread_attr_init(&attr);
	if(err != 0)
		fprintf(stderr, "Failed to initialize thread attributes. %s.", strerror(err)), exit(1);

	err = pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED);
	if(err != 0)
		fprintf(stderr, "Failed to set schedular inheritance. %s.", strerror(err)), exit(1);

	err = pthread_attr_setschedpolicy(&attr, SCHED_FIFO);
	if(err != 0)
		fprintf(stderr, "Failed to set schedular policy. %s.", strerror(err)), exit(1);

	param.sched_priority = 99;
	err = pthread_attr_setschedparam(&attr, &param);
	if(err != 0)
		fprintf(stderr, "Failed to set schedular parameter. %s.", strerror(err)), exit(1);

	err = pthread_create(&conn->thread, &attr, conn_thread, conn);
	if(err != 0) {
		err = pthread_create(&conn->thread, NULL, conn_thread, conn);
		if(err != 0)
			fprintf(stderr, "Failed to start thread. %s.", strerror(err)), exit(1);
	}

	pthread_attr_destroy(&attr);
	if(err != 0)
		fprintf(stderr, "Failed to destroy thread attributes. %s.", strerror(err)), exit(1);

	return conn;
}

/**
 * Close a connection.
 *   @conn: The connection.
 */

void pulse_conn_close(struct pulse_conn_t *conn)
{
	int err;
	unsigned int i;
	uint8_t cmd;

	cmd = 1;
	err = write(conn->pipe[1], &cmd, 1);
	if(err < 0)
		fprintf(stderr, "Failed to write to descriptor. %s.", strerror(errno)), exit(1);

	pthread_join(conn->thread, NULL);

	pa_context_disconnect(conn->context);
	pa_context_unref(conn->context);
	pa_mainloop_free(conn->loop);

	for(i = 0; i < conn->width; i++)
		free(conn->buf[i]);

	free(conn->buf);
	close(conn->pipe[0]);
	close(conn->pipe[1]);
	free(conn);
}


/**
 * Connection processing thread.
 *   @arg: The argument.
 *   &returns: Always null.
 */

static void *conn_thread(void *arg)
{
	struct pulse_conn_t *conn = arg;

	while(true) {
		pa_mainloop_prepare(conn->loop, -1);
		pa_mainloop_poll(conn->loop);

		if(conn->quit)
			break;

		pa_mainloop_dispatch(conn->loop);
	}

	return NULL;
}


/**
 * Handle a poll request from pulse.
 *   @ufds: The file descriptor array.
 *   @nfds: The number of file descriptors.
 *   @timeout: The timeout.
 *   @arg: The argument.
 *   &returns: The number of returned descriptors.
 */

static int conn_poll(struct pollfd *ufds, unsigned long nfds, int timeout, void *arg)
{
	int ret, err;
	struct pulse_conn_t *conn = arg;
	struct pollfd fdset[nfds+1];

	memcpy(fdset, ufds, nfds * sizeof(struct pollfd));
	fdset[nfds].fd = conn->pipe[0];
	fdset[nfds].events = POLLIN;

	ret = poll(fdset, nfds+1, timeout);

	if(fdset[nfds].revents & POLLIN) {
		ret--;

		err = read(conn->pipe[0], &conn->quit, 1);
		if(err < 0)
			fprintf(stderr, "Failed to read from sychronization pipe. %s.", strerror(errno)), exit(1);
	}

	memcpy(ufds, fdset, nfds * sizeof(struct pollfd));

	return ret;
}


/**
 * Connection state.
 *   @context: The context.
 *   @arg: The argument.
 */

static void conn_state(pa_context *context, void *arg)
{
	int err;
	struct pulse_conn_t *conn = arg;
	pa_context_state_t state = pa_context_get_state(context);

	switch(state) {
	case PA_CONTEXT_UNCONNECTED:
	case PA_CONTEXT_CONNECTING: break;
	case PA_CONTEXT_AUTHORIZING: break;
	case PA_CONTEXT_SETTING_NAME: break;

	case PA_CONTEXT_FAILED:
	case PA_CONTEXT_TERMINATED:
		break;

	case PA_CONTEXT_READY:
		{
			pa_sample_spec spec;
			pa_buffer_attr attr;

			spec.rate = conn->conf.rate;
			spec.format = PA_SAMPLE_FLOAT32NE;

			if(conn->conf.in > 0) {
				spec.channels = conn->conf.in;
				conn->record = pa_stream_new(conn->context, "Record", &spec, NULL);
				pa_stream_set_read_callback(conn->record, conn_record, conn);
				pa_stream_set_overflow_callback(conn->record, conn_overflow, conn);
			}
			else
				conn->record = NULL;

			if(conn->conf.out > 0) {
				spec.channels = conn->conf.out;
				conn->playback = pa_stream_new(conn->context, "Playback", &spec, NULL);
				pa_stream_set_write_callback(conn->playback, conn_playback, conn);
				pa_stream_set_underflow_callback(conn->playback, conn_underflow, conn);
			}
			else
				conn->playback = NULL;

			attr.fragsize = sizeof(float) * conn->lat;
			attr.maxlength = (uint32_t)-1;
			attr.maxlength = sizeof(float) * conn->lat;
			attr.minreq = 0;
			attr.prebuf = 0;
			attr.tlength = sizeof(float) * conn->lat;

			if(conn->record != NULL) {
				err = pa_stream_connect_record(conn->record, NULL, &attr, PA_STREAM_INTERPOLATE_TIMING | PA_STREAM_ADJUST_LATENCY | PA_STREAM_AUTO_TIMING_UPDATE);
				if(err < 0)
					fprintf(stderr, "Failed to connect to recorder."), exit(1);
			}

			if(conn->playback != NULL) {
				err = pa_stream_connect_playback(conn->playback, NULL, &attr, PA_STREAM_INTERPOLATE_TIMING | PA_STREAM_ADJUST_LATENCY | PA_STREAM_AUTO_TIMING_UPDATE, NULL, NULL);
				if(err < 0)
					fprintf(stderr, "Failed to connect to playback."), exit(1);
			}
		}

		break;
	}
}


/**
 * Record callback.
 *   @stream: The stream.
 *   @nbytes: The number of bytes available.
 *   @arg: The argument.
 */

static void conn_record(pa_stream *stream, size_t nbytes, void *arg)
{
	struct pulse_conn_t *conn = arg;
	unsigned int i, j, cnt;
	const float *data;

	pa_stream_peek(stream, (const void **)&data, &nbytes);
	cnt = nbytes / (conn->conf.in * sizeof(float));

	if(conn->reset[0])
		conn->wr = conn->lat, conn->reset[0] = 0;

	for(i = 0; i < cnt; i++) {
		for(j = 0; j < conn->conf.in; j++)
			conn->buf[j][conn->wr] = *data++;

		for(; j < conn->width; j++)
			conn->buf[j][conn->wr] = 0.0f;

		conn->wr = (conn->wr + 1) % (2 * conn->lat);

		if(conn->wr == conn->rd)
			conn->reset[0] = conn->reset[1] = 1;
	}

	pa_stream_drop(stream);
}

/**
 * Playback callback.
 *   @stream: The stream.
 *   @nbytes: The number of bytes available.
 *   @arg: The argument.
 */

static void conn_playback(pa_stream *stream, size_t nbytes, void *arg)
{
	struct pulse_conn_t *conn = arg;
	float buf[nbytes / sizeof(float)], *data = buf;
	unsigned int i, j, cnt = nbytes / (conn->conf.out * sizeof(float));

	if(conn->reset[1])
		conn->rd = 0, conn->reset[1] = 0;

	{
		unsigned int rd;
		double arr[conn->width][cnt], *ptr[conn->width];

		for(j = 0; j < conn->width; j++)
			ptr[j] = arr[j];

		for(j = 0; j < conn->conf.out; j++) {
			for(i = 0, rd = conn->rd; i < cnt; i++, rd = (rd + 1) % (2 * conn->lat))
				arr[j][i] = conn->buf[j][rd];
		}

		conn->func(ptr, cnt, conn->arg);

		for(j = 0; j < conn->conf.out; j++) {
			for(i = 0, rd = conn->rd; i < cnt; i++, rd = (rd + 1) % (2 * conn->lat))
				conn->buf[j][rd] = arr[j][i];
		}
	}

	for(i = 0; i < cnt; i++) {
		for(j = 0; j < conn->conf.out; j++)
			*data++ = conn->buf[j][conn->rd];

		conn->rd = (conn->rd + 1) % (2 * conn->lat);

		if(conn->rd == conn->wr)
			conn->reset[0] = conn->reset[1] = 1;
	}

	pa_stream_write(stream, buf, nbytes, NULL, 0LL, PA_SEEK_RELATIVE);
}

/**
 * Overflow callback.
 *   @stream: The stream.
 *   @arg: The argument.
 */

static void conn_overflow(pa_stream *stream, void *arg)
{
	struct pulse_conn_t *conn = arg;

	conn->func(NULL, 0, conn->arg);
}

/**
 * Underflow callback.
 *   @stream: The stream.
 *   @arg: The argument.
 */

static void conn_underflow(pa_stream *stream, void *arg)
{
	struct pulse_conn_t *conn = arg;

	conn->func(NULL, 0, conn->arg);
}
