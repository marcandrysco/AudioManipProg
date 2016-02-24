#ifndef FILT_H
#define FILT_H

/**
 * Low-pass constant structure.
 *   @g: The gain element.
 */

struct dsp_lpf_t {
	double g;
};

/**
 * Initialize the low-pass constant.
 *   @freq: The frequency.
 *   &returns: The constant.
 */

static inline struct dsp_lpf_t dsp_lpf_init(double freq, unsigned int rate)
{
	double w = tanf(freq * M_PI / rate);

	return (struct dsp_lpf_t){ w };
}

/**
 * Compute the frequency of a low-pass constant.
 *   @c: The low-pass constant.
 *   &returns: The frequency.
 */

static inline double dsp_lpf_freq(struct dsp_lpf_t c, unsigned int rate)
{
	return atan(c.g) * rate / M_PI;
}

/**
 * Process a low-pass filter.
 *   @x: The input.
 *   @c: The constant.
 *   @s: The state. Must have at least one variables.
 *   &returns: The output.
 */

static inline double dsp_lpf_proc(double x, struct dsp_lpf_t c, double *s)
{
	double y;

	y = (c.g * x + s[0]) / (1 + c.g);
	s[0] = s[0] + 2 * c.g * (x - y);

	return y;
}


/**
 * High-pass constant structure.
 *   @g: The gain element.
 */

struct dsp_hpf_t {
	double g;
};

/**
 * Initialize the high-pass constant.
 *   @freq: The frequency.
 *   &returns: The constant.
 */

static inline struct dsp_hpf_t dsp_hpf_init(double freq, unsigned int rate)
{
	double w = tanf(freq * M_PI / rate);

	return (struct dsp_hpf_t){ w / (1 + w) };
}

/**
 * Process a high-pass filter.
 *   @x: The input.
 *   @c: The constant.
 *   @s: The state. Must have at least one variables.
 *   &returns: The output.
 */

static inline double dsp_hpf_proc(double x, struct dsp_hpf_t c, double *s)
{
	double y;

	y = (x - s[0]) / (1 + c.g);
	s[0] += 2 * c.g * y;

	return y;
}

#endif
