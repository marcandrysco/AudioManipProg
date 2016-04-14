#include "common.h"


/**
 * Create a buffer.
 *   @len: The length.
 *   &returns: The buffer.
 */
struct dsp_buf_t *dsp_buf_new(unsigned int len)
{
	struct dsp_buf_t *buf;

	buf = malloc(sizeof(struct dsp_buf_t) + len * sizeof(double));
	buf->len = len;
	dsp_zero_f(buf->arr, len);

	return buf;
}

/**
 * Load a buffer from a path.
 *   @path: The path.
 *   @chan: The requested channel.
 *   &returns: The buffer.
 */
struct dsp_buf_t *dsp_buf_load(const char *path, unsigned int chan, unsigned int rate)
{
	unsigned int i, len;
	SNDFILE *file;
	SF_INFO info;
	float *data, *tmp;
	struct dsp_buf_t *buf;

	info.format = 0;

	file = sf_open(path, SFM_READ, &info);
	if(file == NULL)
		return NULL;

	if(chan >= info.channels) {
		sf_close(file);
		return NULL;
	}

	len = dsp_rerate(info.frames, rate, info.samplerate);

	buf = malloc(sizeof(struct dsp_buf_t) + len * sizeof(float));
	buf->len = info.frames;

	data = malloc(info.frames * info.channels * sizeof(float));

	if(rate != info.samplerate)
		tmp = malloc(info.frames * sizeof(float));
	else
		tmp = buf->arr;

	sf_readf_float(file, data, info.frames);
	for(i = 0; i < info.frames; i++)
		tmp[i] = data[i * info.channels + chan];

	if(rate != info.samplerate) {
		dsp_rerate_f(buf->arr, len, rate, tmp, info.frames, info.samplerate);
		free(tmp);
	}

	free(data);
	sf_close(file);

	return buf;
}

/**
 * Delete a buffer.
 *   @buf: The buffer.
 */
void dsp_buf_delete(struct dsp_buf_t *buf)
{
	free(buf);
}


/**
 * Save a buffer to a path.
 *   @buf: The buffer.
 *   @path: The path.
 *   &returns: Error.
 */
char *dsp_buf_save(struct dsp_buf_t *buf, const char *path)
{
	return NULL;
}


/**
 * Create a ring buffer.
 *   @len: The length.
 *   &returns: The ring buffer.
 */
struct dsp_ring_t *dsp_ring_new(unsigned int len)
{
	struct dsp_ring_t *ring;

	ring = malloc(sizeof(struct dsp_ring_t) + len * sizeof(double));
	ring->i = 0;
	ring->len = len;
	dsp_zero_d(ring->arr, len);

	return ring;
}

/**
 * Delete a ring buffer.
 *   @ring: The ring buffer.
 */
void dsp_ring_delete(struct dsp_ring_t *ring)
{
	free(ring);
}


/**
 * Find the length for a sample rate conversion.
 *   @len: The input length.
 *   @outrate: The output sample rate.
 *   @inrate: The input sample rate.
 *   &returns: The output length.
 */
unsigned int dsp_rerate(unsigned int len, unsigned int outrate, unsigned int inrate)
{
	return ceil((double)len * (double)outrate / (double)inrate);
}

/**
 * Perform sample rate conversion between buffers.
 *   @out: The output.
 *   @outlen: The output length.
 *   @outrate: The output rate.
 *   @in: The input.
 *   @inlen: The input length.
 *   @inrate: The input rate.
 */
void dsp_rerate_f(float *out, unsigned int outlen, unsigned int outrate, float *in, unsigned int inlen, unsigned int inrate)
{
	float idx, ratio;
	unsigned int i;

	ratio = (float)inrate / (float)outrate;

	for(i = 0; i < outlen; i++) {
		idx = ratio * i;

		idx = round(idx);
		out[i] = ((int)idx < inlen) ? in[(int)idx] : 0.0;
	}
}
