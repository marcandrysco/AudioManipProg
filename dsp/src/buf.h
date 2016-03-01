#ifndef BUF_H
#define BUF_H

/**
 * Buffer structure.
 *   @len: The length.
 *   @arr: The array.
 */

struct dsp_buf_t {
	unsigned int len;
	float arr[];
};

/*
 * buffer declarations
 */

struct dsp_buf_t *dsp_buf_new(unsigned int len);
struct dsp_buf_t *dsp_buf_load(const char *path, unsigned int chan, unsigned int rate);
void dsp_buf_delete(struct dsp_buf_t *buf);


/**
 * Ring buffer structure.
 *   @i, len: The index and length.
 *   @arr: The array.
 */

struct dsp_ring_t {
	unsigned int i, len;
	double arr[];
};

/*
 * ring buffer declarations
 */

struct dsp_ring_t *dsp_ring_new(unsigned int len);
void dsp_ring_delete(struct dsp_ring_t *ring);

/**
 * Retrieve the last value added to a ring buffer.
 *   @ring: The ring buffer.
 *   &returns: The oldest value in the buffer.
 */

static inline double dsp_ring_last(struct dsp_ring_t *ring)
{
	return ring->arr[ring->i];
}

/**
 * Put a value on a ring buffer.
 *   @ring: The ring buffer.
 *   @val: The new value.
 */

static inline void dsp_ring_put(struct dsp_ring_t *ring, double val)
{
	ring->arr[ring->i] = val;
	ring->i = (ring->i + 1) % ring->len;
}


/*
 * sample rate conversion declarations
 */

unsigned int dsp_rerate(unsigned int len, unsigned int outrate, unsigned int inrate);
void dsp_rerate_f(float *out, unsigned int outlen, unsigned int outrate, float *in, unsigned int inlen, unsigned int inrate);

#endif
