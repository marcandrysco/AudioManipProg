#ifndef REVERB_H
#define REVERB_H

/**
 * Process an all-pass reverberator.
 *   @x: The input.
 *   @ring: The ring buffer.
 *   @gain: The gain.
 *   &returns: The next output.
 */

static inline double dsp_reverb_allpass(double x, struct dsp_ring_t *ring, double gain)
{
	double v, y;

	v = dsp_ring_last(ring);
	x -= gain * v;
	y = v + gain * x;
	dsp_ring_put(ring, x);

	return y;
}

#endif
