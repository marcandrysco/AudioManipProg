#include "../common.h"
#include "hack.h"
#include "conn.h"


#define BUFLEN (4096)

/**
 * Buffer structure.
 *   @reset: The reset flags.
 *   @rd, wr: The read and write positions.
 *   @arr: The data array.
 */

struct buf_t {
	sig_atomic_t reset[2];

	unsigned int rd, wr;
	double arr[2][2*BUFLEN];
};

/**
 * Hack structure.
 *   @conn: The connection.
 *   @in, out: The input and output buffers.
 */

struct pulse_hack_t {
	struct pulse_conn_t *conn;

	struct buf_t in, out;
};


/*
 * local declarations
 */

static void onaudio(double **buf, unsigned int len, void *arg);
static void bufread(struct buf_t *hack, double **buf, unsigned int len);
static void bufwrite(struct buf_t *hack, double **buf, unsigned int len);


/**
 * Create a hack.
 *   @rate: The sample rate.
 *   &returns: The hack.
 */

struct pulse_hack_t *pulse_hack_new(unsigned int rate)
{
	struct pulse_hack_t *hack;
	struct pulse_conf_t conf = { 2, 2, rate, 150.0f };

	hack = malloc(sizeof(struct pulse_hack_t));
	hack->conn = pulse_conn_open("default", onaudio, hack, &conf);
	hack->in.reset[0] = hack->in.reset[1] = 1;
	hack->out.reset[0] = hack->out.reset[1] = 1;
	dsp_zero_d(hack->in.arr[0], 2 * BUFLEN);
	dsp_zero_d(hack->in.arr[1], 2 * BUFLEN);
	dsp_zero_d(hack->out.arr[0], 2 * BUFLEN);
	dsp_zero_d(hack->out.arr[1], 2 * BUFLEN);

	return hack;
}

/**
 * Delete a hack.
 *   @hack: The hack.
 */

void pulse_hack_delete(struct pulse_hack_t *hack)
{
	pulse_conn_close(hack->conn);
	free(hack);
}


/**
 * Reset the hack.
 *   @hack: The hack.
 */

void pulse_hack_reset(struct pulse_hack_t *hack)
{
	// ha! I should do this :"(
}

/**
 * Process data on the hack.
 *   @hack: The hack.
 *   @buf: The buffer.
 *   @len: The length.
 */

void pulse_hack_proc(struct pulse_hack_t *hack, double **buf, unsigned int len)
{
	double data[2][len], *tmp[2];

	tmp[0] = data[0];
	tmp[1] = data[1];

	bufwrite(&hack->out, buf, len);
	bufread(&hack->in, tmp, len);

	dsp_scale_d(tmp[0], 0.5, len);
	dsp_scale_d(tmp[1], 0.5, len);
	dsp_add_d(buf[0], tmp[0], len);
	dsp_add_d(buf[1], tmp[1], len);
}

/**
 * Handle audio data on the hack.
 *   @buf: The buffer.
 *   @len: The length.
 *   @arg: The argument.
 */

static void onaudio(double **buf, unsigned int len, void *arg)
{
	struct pulse_hack_t *hack = arg;

	if(len == 0)
		return;

	bufwrite(&hack->in, buf, len);

	dsp_zero_d(buf[0], len);
	dsp_zero_d(buf[1], len);

	bufread(&hack->out, buf, len);
}


/**
 * Read from a hack buffer.
 *   @hack: The hack buffer.
 *   @buf: The input buffer.
 *   @len: The number of samples.
 */

static void bufread(struct buf_t *hack, double **buf, unsigned int len)
{
	unsigned int i, rd, wr;

	if(!hack->reset[0]) {
		rd = hack->rd;
		wr = hack->wr;
	}
	else {
		hack->reset[0] = 0;
		rd = 0;
		wr = BUFLEN;
	}

	for(i = 0; i < len; i++) {
		buf[0][i] = hack->arr[0][rd];
		buf[1][i] = hack->arr[1][rd];
		rd = (rd + 1) % (2 * BUFLEN);

		if(rd == wr) {
			hack->reset[1] = 1;
			rd = 0;
		}
	}

	hack->rd = rd;
}

/**
 * Write from a hack buffer.
 *   @hack: The hack buffer.
 *   @buf: The write buffer.
 *   @len: The number of samples.
 */

static void bufwrite(struct buf_t *hack, double **buf, unsigned int len)
{
	unsigned int i, wr, rd;

	wr = hack->wr;
	rd = hack->rd;

	if(!hack->reset[1]) {
		rd = hack->rd;
		wr = hack->wr;
	}
	else {
		hack->reset[1] = 0;
		rd = 0;
		wr = BUFLEN;
	}

	for(i = 0; i < len; i++) {
		hack->arr[0][wr] = buf[0][i];
		hack->arr[1][wr] = buf[1][i];
		wr = (wr + 1) % (2 * BUFLEN);

		if(wr == rd) {
			hack->reset[0] = 1;
			wr = BUFLEN;
		}
	}

	hack->wr = wr;
}
