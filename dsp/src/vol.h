#ifndef VOL_H
#define VOL_H

/**
 * Volume tracker structure.
 *   @s, v: The state and current volume.
 *   @c: The filter constant.
 */
struct dsp_vol_t {
	double s, v;
	struct dsp_lpf_t c;
};

/**
 * Initialize a volume tracker.
 *   @freq: The cutoff frequency.
 *   @rate: The sample rate.
 *   &returns: The tracker.
 */
static inline struct dsp_vol_t dsp_vol_init(double freq, unsigned int rate)
{
	return (struct dsp_vol_t){ 0.0, 0.0, dsp_lpf_init(freq, rate) };
}

/**
 * Process a volume tracker.
 *   @vol: The volume tracker.
 *   @v: The incoming value.
 *   &returns: The current volume.
 */
static inline double dsp_vol_proc(struct dsp_vol_t *vol, double v)
{
	return vol->v = dsp_lpf_proc(fabs(v * (M_PI / 2)), vol->c, &vol->s);
}

#endif
