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

static inline struct dsp_lpf_t dsp_lpf_init(double freq, double rate)
{
	double w = tanf(freq * M_PI / rate);

	return (struct dsp_lpf_t){ w };
}

/**
 * Compute the frequency of a low-pass constant.
 *   @c: The low-pass constant.
 *   &returns: The frequency.
 */

static inline double dsp_lpf_freq(struct dsp_lpf_t c, double rate)
{
	return atanf(c.g) * rate / M_PI;
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

static inline struct dsp_hpf_t dsp_hpf_init(double freq, double rate)
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


/**
 * Moog filter structure.
 *   @g: The gain element.
 *   @r: The resonance.
 */

struct dsp_moog_t {
	double g;
	double r;
};

/**
 * Initialize a moog filter.
 *   @freq: The cutoff frequency.
 *   @res: The resonance.
 *   @rate: The sample rate.
 */

static inline struct dsp_moog_t dsp_moog_init(double freq, double res, double rate)
{
	double w = tanf(freq * M_PI / rate);

	return (struct dsp_moog_t){ w, res };
}

/**
 * Process a moog filter.
 *   @x: The input.
 *   @moog: The moog filter constant.
 *   @s: The state. Must have 4 elements.
 *   &returns: The output.
 */

static inline double dsp_moog_proc(double x, struct dsp_moog_t moog, double *s)
{
	double t1, t2, g = moog.g, y;

	t1 = g*g*g*s[0] + g*g*s[1] + g*s[2] + s[3];
	t2 = g*g*g*g;

	x = (x - moog.r * t1) / (1 + moog.r * t2);

	y = (moog.g * x + s[0]) / (1 + moog.g);
	s[0] = s[0] + 2 * moog.g * (x - y);
	x = y;

	y = (moog.g * x + s[1]) / (1 + moog.g);
	s[1] = s[1] + 2 * moog.g * (x - y);
	x = y;

	y = (moog.g * x + s[2]) / (1 + moog.g);
	s[2] = s[2] + 2 * moog.g * (x - y);
	x = y;

	y = (moog.g * x + s[3]) / (1 + moog.g);
	s[3] = s[3] + 2 * moog.g * (x - y);
	x = y;
	
	return x;
}


/**
 * State variable filter structure.
 *   @g: The gain element.
 *   @r: The resonance.
 */

struct dsp_svf_t {
	double g;
	double r;
};

/**
 * Initialize a state variable filter.
 *   @freq: The frequencer.
 *   @res: The resonance.
 *   @rate: The sample rate.
 *   &returns: The state variable filter.
 */

static inline struct dsp_svf_t dsp_svf_init(double freq, double res, unsigned int rate)
{
	double w = tanf(freq * M_PI / rate);

	return (struct dsp_svf_t){ w, 1.0 - res };
}

/**
 * Process a state variable filter.
 *   @y: The outputs.
 *   @x: The input.
 *   @c: The constant.
 *   @s: The state. Must have at least two variables.
 */

static inline void dsp_svf_proc(double *y, double x, struct dsp_svf_t c, double *s)
{
	y[0] = (x - 2*c.r*s[0] - c.g*s[0] - s[1]) / (1 + 2*c.r*c.g + c.g*c.g);
	y[1] = s[0] + y[0]*c.g;
	y[2] = s[1] + y[1]*c.g;

	s[0] = y[1] + y[0]*c.g;
	s[1] = y[2] + y[1]*c.g;
}

/**
 * Process a low-pass state variable filter.
 *   @x: The input.
 *   @c: The constant.
 *   @s: The state. Must have at least two variables.
 *   &returns: The output.
 */

static inline double dsp_svf_low(double x, struct dsp_svf_t c, double *s)
{
	double y[3];

	dsp_svf_proc(y, x, c, s);

	return y[2];
}

/**
 * Initialize a SVF for a resonant filter.
 *   @freq: The frequencer.
 *   @qual: The quality.
 *   @rate: The sample rate.
 *   &returns: The state variable filter.
 */

static inline struct dsp_svf_t dsp_res_init(double freq, double qual, unsigned int rate)
{
	return dsp_svf_init(freq, 1.0 - 1.0 / qual, rate);
}

/**
 * Process a resonant filter.
 *   @x: The input.
 *   @c: The constant.
 *   @gain: The gain.
 *   @s: The state. Must have at least two variables.
 */

static inline double dsp_res_proc(double x, struct dsp_svf_t svf, double *s)
{
	double y[3];

	dsp_svf_proc(y, x, svf, s);

	return 2 * svf.r * y[1];
}


/**
 * Peaking filter structure.
 *   @g: The gain.
 *   @svf: The state variable data.
 */

struct dsp_peak_t {
	double g;
	struct dsp_svf_t svf;
};

/**
 * Initialize a peaking filter.
 *   @freq: The frequency.
 *   @gain: The gain.
 *   @qual: The quality.
 *   @rate: The sample rate.
 *   &returns: The state variable filter.
 */

static inline struct dsp_peak_t dsp_peak_init(double freq, double gain, double qual, unsigned int rate)
{
	return (struct dsp_peak_t){ gain, dsp_svf_init(freq, 1.0 - 1.0 / qual, rate) };
}

/**
 * Process a peaking filter.
 *   @x: The input.
 *   @c: The constant.
 *   @gain: The gain.
 *   @s: The state. Must have at least two variables.
 */

static inline double dsp_peak_proc(double x, struct dsp_peak_t peak, double *s)
{
	double y[3];

	dsp_svf_proc(y, x, peak.svf, s);

	return (peak.g - 1.0) * 2 * peak.svf.r * y[1] + x;
}

#endif
