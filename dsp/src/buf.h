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
 * Delete a buffer if non-null.
 *   @buf: The buffer.
 */
static inline void dsp_buf_erase(struct dsp_buf_t *buf)
{
	if(buf != NULL)
		dsp_buf_delete(buf);
}


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

/**
 * Retrieve a value from the ring buffer using linear interpolation.
 *   @ring: The ring buffer.
 *   @idx: The index into the buffer.
 *   &returns: The interpolated value.
 */
static inline double dsp_ring_getf(struct dsp_ring_t *ring, double idx)
{
	int p = floor(idx), n = ceil(idx);
	double r = idx - floor(idx);

	p = ((ring->i - p) + ring->len) % ring->len;
	n = ((ring->i - n) + ring->len) % ring->len;

	return ring->arr[p] * (1.0 - r) + r * ring->arr[n];
}

/**
 * Process a ring buffer, adding a value and retreiving the last value.
 *   @ring: The ring buffer.
 *   @val: The value.
 *   &returns: The last value.
 */
static inline double dsp_ring_proc(struct dsp_ring_t *ring, double val)
{
	double ret;

	ret = dsp_ring_last(ring);
	dsp_ring_put(ring, val);

	return ret;
}

/**
 * Delete a ring buffer if non-null.
 *   @ring: The ring buffer.
 */
static inline void dsp_ring_erase(struct dsp_ring_t *ring)
{
	if(ring != NULL)
		dsp_ring_delete(ring);
}


/*
 * sample rate conversion declarations
 */
unsigned int dsp_rerate(unsigned int len, unsigned int outrate, unsigned int inrate);
void dsp_rerate_f(float *out, unsigned int outlen, unsigned int outrate, float *in, unsigned int inlen, unsigned int inrate);

#endif
