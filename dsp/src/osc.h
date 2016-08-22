#ifndef OSC_H
#define OSC_H

/**
 * Compute the step size of a wave.
 *   @freq: The frequency.
 *   @rate: The sample rate.
 *   &returns: The step size.
 */
static inline double dsp_osc_step(double freq, unsigned int rate)
{
	return rate ? (freq / rate) : 0.0;
}

/**
 * Increment the time using the step.
 *   @t: The time.
 *   @step: The step.
 */
static inline double dsp_osc_inc(double t, double step)
{
	return fmod(t + step, 1.0);
}

/**
 * Compute a sine wave as a float.
 *   @t: The time.
 *   &returns: The wave value.
 */
static inline float dsp_osc_sine_f(double t)
{
	return sinf(2.0f * M_PI * t);
}

/**
 * Compute a sine wave as a double.
 *   @t: The time.
 *   &returns: The wave value.
 */
static inline double dsp_osc_sine_d(double t)
{
	return sin(2.0 * M_PI * t);
}

/**
 * Generate a square wave.
 *   @t: The time.
 *   &returns: The wave value.
 */
static inline double dsp_osc_square(double t)
{
	return (t < 0.5) ? 1.0 : -1.0;
}

/**
 * Generate a sawtooth wave data point.
 *   @t: The time.
 *   &returns: The wave value.
 */
static inline double dsp_osc_saw(double t)
{
	return fmod(1.0 + 2.0 * t, 2.0) - 1.0;
}

/**
 * Generate a reverse sawtooth wave data point.
 *   @t: The time.
 *   &returns: The wave value.
 */
static inline double dsp_osc_rsaw(double t)
{
	return fmod(3.0 - 2.0 * t, 2.0) - 1.0;
}

/**
 * Generate a triangle wave data point.
 *   @t: The time.
 *   &returns: The wave value.
 */
static inline double dsp_osc_tri(double t)
{
	//return 2.0 * fabs(2.0 * t - 1.0) - 1.0;
	return 4.0 * (t + fabs(t - 0.75) - fabs(t - 0.25)) - 2.0;
}

/**
 * Warp time.
 *   @t: The time.
 *   @v: The warp value.
 *   &returns: The warped time.
 */
static inline double dsp_osc_warp(double t, double v)
{   
	double mid = 0.5 * (1.0 - v);

	return (t < mid) ? (t / (2.0 * mid)) : ((t - mid) / (1.0 - mid) / 2.0 + 0.5);
}

/**
 * Unwarp time.
 *   @t: The warped time.
 *   @v: The warp value.
 *   &returns: The unwarped time.
 */
static inline double dsp_osc_unwarp(double t, double v)
{   
	double mid = 0.5 * (1.0 - v);

	return (t < 0.5) ? (2.0 * mid * t) : (2.0 * (t - 0.5) * (1.0 - mid) + mid);
}

#endif
