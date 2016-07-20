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
 * Varying comb filter with interpolation.
 *   @x: The input.
 *   @ring: The ring buffer.
 *   @gain: The gain.
 *   @delay: The delay.
 *   @s: The interpolation state.
 *   &returns: The next output.
 */
static inline double dsp_vary_comb(double x, struct dsp_ring_t *ring, double delay, double *s, double gain)
{
	double v;

	v = gain * dsp_ring_get1(ring, delay, s);
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

/**
 * Process a low-pass comb-feedback reverberator.
 *   @x: The input.
 *   @ring: The ring buffer.
 *   @gain: The gain.
 *   @lpf: The low-pass filter constant.
 *   @s: The state.
 *   &returns: The next output.
 */
static inline double dsp_vary_lpcf(double x, struct dsp_ring_t *ring, double delay, double *s1, double gain, struct dsp_lpf_t lpf, double *s2)
{
	double v;

	v = dsp_lpf_proc(gain * dsp_ring_get1(ring, delay, s1), lpf, s2);
	dsp_ring_put(ring, v + x);

	return v;
}


/**
 * Process a band-pass comb-feedback reverberator.
 *   @x: The input.
 *   @ring: The ring buffer.
 *   @gain: The gain.
 *   @lpf: The band-pass filter constant.
 *   @s: The state. Must have two variables.
 *   &returns: The next output.
 */
static inline double dsp_reverb_bpcf(double x, struct dsp_ring_t *ring, double gain, struct dsp_bpf_t bpf, double *s)
{
	double v;

	v = dsp_bpf_proc(gain * dsp_ring_last(ring), bpf, s);
	dsp_ring_put(ring, v + x);

	return v;
}

/**
 * Process a band-pass comb-feedback reverberator.
 *   @x: The input.
 *   @ring: The ring buffer.
 *   @gain: The gain.
 *   @lpf: The band-pass filter constant.
 *   @s2: The state. Must have two variables.
 *   &returns: The next output.
 */
static inline double dsp_vary_bpcf(double x, struct dsp_ring_t *ring, double delay, double *s1, double gain, struct dsp_bpf_t bpf, double *s2)
{
	double v;

	v = dsp_bpf_proc(gain * dsp_ring_get1(ring, delay, s1), bpf, s2);
	dsp_ring_put(ring, v + x);

	return v;
}


/**
 * Process a band-pass comb-feedback reverberator.
 *   @x: The input.
 *   @ring: The ring buffer.
 *   @gain: The gain.
 *   @lpf: The band-pass filter constant.
 *   @s: The state. Must have two variables.
 *   &returns: The next output.
 */
static inline double dsp_reverb_bpcf2(double x, struct dsp_ring_t *ring, double gain, struct dsp_bpf2_t bpf, double *s)
{
	double v;

	v = dsp_bpf2_proc(gain * dsp_ring_last(ring), bpf, s);
	dsp_ring_put(ring, v + x);

	return v;
}

/**
 * Process a band-pass comb-feedback reverberator.
 *   @x: The input.
 *   @ring: The ring buffer.
 *   @gain: The gain.
 *   @lpf: The band-pass filter constant.
 *   @s2: The state. Must have two variables.
 *   &returns: The next output.
 */
static inline double dsp_vary_bpcf2(double x, struct dsp_ring_t *ring, double delay, double *s1, double gain, struct dsp_bpf2_t bpf, double *s2)
{
	double v;

	v = dsp_bpf2_proc(gain * dsp_ring_get1(ring, delay, s1), bpf, s2);
	dsp_ring_put(ring, v + x);

	return v;
}
#endif
