#ifndef MEM_H
#define MEM_H

/**
 * Clamp a single double to a range.
 *   @v: The value to clamp.
 *   @low: The low end of the rnage.
 *   @high: The high end of the range.
 *   &returns: The value clamped to the range '[low,high]'.
 */
static inline double dsp_clamp(double v, double low, double high)
{
	if(v < low)
		return low;
	else if(v > high)
		return high;
	else
		return v;
}

/**
 * Clamp a buffer of doubles.
 *   @buf: The buffer.
 *   @len: The length.
 */
static inline void dsp_clamp_d(double *buf, unsigned int len)
{
	unsigned int i;

	for(i = 0; i < len; i++) {
		if(buf[i] > 1.0)
			buf[i] = 1.0;
		else if(buf[i] < -1.0)
			buf[i] = -1.0;
	}
}


/**
 * Zero a buffer of floats.
 *   @buf: The buffer.
 *   @len: The length.
 */
static inline void dsp_zero_f(float *buf, unsigned int len)
{
	unsigned int i;

	for(i = 0; i < len; i++)
		buf[i] = 0.0f;
}

/**
 * Zero a buffer of doubles.
 *   @buf: The buffer.
 *   @len: The length.
 */
static inline void dsp_zero_d(double *buf, unsigned int len)
{
	unsigned int i;

	for(i = 0; i < len; i++)
		buf[i] = 0.0;
}


/**
 * Set a buffer of floats to one.
 *   @buf: The buffer.
 *   @len: The length.
 */
static inline void dsp_one_f(float *buf, unsigned int len)
{
	unsigned int i;

	for(i = 0; i < len; i++)
		buf[i] = 1.0f;
}

/**
 * Set a buffer of doubles to one.
 *   @buf: The buffer.
 *   @len: The length.
 */
static inline void dsp_one_d(double *buf, unsigned int len)
{
	unsigned int i;

	for(i = 0; i < len; i++)
		buf[i] = 1.0;
}


/**
 * Copy a buffer of doubles.
 *   @dest: The destination.
 *   @src: The source.
 *   @len: The length.
 */
static inline void dsp_copy_d(double *dest, double *src, unsigned int len)
{
	unsigned int i;

	for(i = 0; i < len; i++)
		dest[i] = src[i];
}


/**
 * Add two buffer of doubles.
 *   @dest: The destination.
 *   @src: The source.
 *   @len: The length.
 */
static inline void dsp_add_d(double *dest, double *src, unsigned int len)
{
	unsigned int i;

	for(i = 0; i < len; i++)
		dest[i] += src[i];
}


/**
 * Multiply two buffer of doubles.
 *   @dest: The destination.
 *   @src: The source.
 *   @len: The length.
 */
static inline void dsp_mul_d(double *dest, double *src, unsigned int len)
{
	unsigned int i;

	for(i = 0; i < len; i++)
		dest[i] *= src[i];
}


/**
 * Scale a buffer of doubles.
 *   @buf: The buffer.
 *   @fact: The scaling factoring.
 *   @len: The length.
 */
static inline void dsp_scale_d(double *buf, double fact, unsigned int len)
{
	unsigned int i;

	for(i = 0; i < len; i++)
		buf[i] *= fact;
}

#endif
