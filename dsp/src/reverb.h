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

/**
 * Process a comb-feedback reverberator.
 *   @x: The input.
 *   @ring: The ring buffer.
 *   @gain: The gain.
 *   &returns: The next output.
 */

static inline double dsp_reverb_comb(double x, struct dsp_ring_t *ring, double gain)
{
	double v;

	v = gain * dsp_ring_last(ring);
	dsp_ring_put(ring, v + x);

	return v;
}

/**
 * Process a low-pass comb-feedback reverberator.
 *   @x: The input.
 *   @ring: The ring buffer.
 *   @gain: The gain.
 *   @lpf: The low-pass filter constant.
 *   @s: The state.
 *   &returns: The next output.
 */

static inline double dsp_reverb_lpcf(double x, struct dsp_ring_t *ring, double gain, struct dsp_lpf_t lpf, double *s)
{
	double v;

	v = dsp_lpf_proc(gain * dsp_ring_last(ring), lpf, s);
	dsp_ring_put(ring, v + x);

	return v;
}

#endif
