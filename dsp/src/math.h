#ifndef MATH_H
#define MATH_H

/**
 * Perform integer modulus with strictliy positive results.
 *   @a: The value.
 *   @m: The modulus.
 *   &returns: The remainder.
 */
static inline unsigned int dsp_mod_i(int v, unsigned int m)
{
	return ((v % m) + m) % m;
}

/**
 * Perform double modulus with strictliy positive results.
 *   @a: The value.
 *   @m: The modulus.
 *   &returns: The remainder.
 */
static inline double dsp_mod_d(double v, double m)
{
	return fmod(fmod(v, m) + m, m);
}


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
 * Calculate the exponential decay constant as a double.
 *   @target: The target value.
 *   @len: The time in samples to reach the target.
 *   &returns: The exponential structure.
 */
static inline double dsp_decay_d(double target, double len)
{
	return (len > 0.0) ? dsp_pow_d(1.0 - target, 1.0 / len) : 0.0;
}

/**
 * Calculate the exponential decay constant as a float.
 *   @target: The target value.
 *   @len: The time in samples to reach the target.
 *   &returns: The exponential structure.
 */
static inline float dsp_decay_f(float target, float len)
{
	return (len > 0.0f) ? dsp_pow_d(1.0f - target, 1.0f / len) : 0.0f;
}

#define LOGHALF (-0.693147180559945309417232)


/**
 * Calculate the half-life decay constant as a double.
 *   @len: The length in samples.
 *   &returns: The decay constant.
 */
static inline double dsp_half_d(double len)
{
	return (len > 0.0) ? exp(LOGHALF / len) : 0.0;
}

/**
 * Calculate the half-life decay constant as a float.
 *   @len: The length in samples.
 *   &returns: The decay constant.
 */
static inline float dsp_half_f(float len)
{
	return (len > 0.0f) ? expf(LOGHALF / len) : 0.0f;
}

#endif
