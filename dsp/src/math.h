#ifndef MATH_H
#define MATH_H

/**
 * Compute a faster but less precise power as a double.
 *   @b: The base.
 *   @e: The exponent.
 *   &returns: The result.
 */

static inline double dsp_pow_d(double b, double e)
{
	return exp(log(b) * e);
}

/**
 * Compute a faster but less precise power as a float.
 *   @b: The base.
 *   @e: The exponent.
 *   &returns: The result.
 */

static inline float dsp_pow_f(float b, float e)
{
	return expf(logf(b) * e);
}


/**
 * Convert amplitude to decibels.
 *   @amp: The amplitude value.
 *   &returns: The decibel value.
 */

static inline double dsp_amp2db_d(double amp)
{
	return 20.0 * log(amp) / log(10.0);
}

/**
 * Convert amplitude to decibels.
 *   @amp: The amplitude value.
 *   &returns: The decibel value.
 */

static inline double dsp_amp2db_f(double amp)
{
	return 20.0 * logf(amp) / logf(10.0);
}

/**
 * Convert decibels to amplitude.
 *   @db: The decibel value.
 *   &returns: The amplitude value.
 */

static inline double dsp_db2amp_d(double db)
{
	return pow(10.0, db / 20.0);
}

/**
 * Convert decibels to amplitude.
 *   @db: The decibel value.
 *   &returns: The amplitude value.
 */

static inline double dsp_db2amp_f(double db)
{
	return powf(10.0, db / 20.0);
}


/**
 * Calculate the exponential decay constant as a float.
 *   @target: The target value.
 *   @len: The time in samples to reach the target.
 *   &returns: The exponential structure.
 */

static inline double dsp_decay_d(double target, double len)
{
	return (len > 0) ? dsp_pow_d(1.0 - target, 1.0 / len) : 0.0;
}

#endif
