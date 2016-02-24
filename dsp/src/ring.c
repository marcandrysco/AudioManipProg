#include "common.h"


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
