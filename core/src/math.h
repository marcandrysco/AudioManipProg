#ifndef MATH_H
#define MATH_H


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
