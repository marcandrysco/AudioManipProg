#ifndef COMP_H
#define COMP_H

/**
 * Compressor structure.
 *   @atk, rel, thresh, ratio: The parameters.
 */

struct dsp_comp_t {
	double atk, rel, thresh, ratio;
};


/**
 * Initialize a compressor.
 *   @atk: The attack length in samples.
 *   @rel: The release length in samples.
 *   @thresh: The threshold.
 *   @ratio: The ratio.
 *   &returns: The compressor.
 */

static inline struct dsp_comp_t dsp_comp_init(double atk, double rel, double thresh, double ratio)
{
	struct dsp_comp_t comp;

	comp.atk = 1.0 - dsp_decay_d(0.5, atk);
	comp.rel = 1.0 - dsp_decay_d(0.5, rel);
	comp.thresh = thresh;
	comp.ratio = 1.0 / ratio;

	return comp;
}

/**
 * Process the compressor.
 *   @comp: The compressor.
 *   @buf: The buffer.
 *   @len: The length.
 */

static inline double dsp_comp_proc(struct dsp_comp_t *comp, double x, double *vol, double *ctrl)
{
	double y, diff;

	y = x;
	x = fabs(y);

	diff = x - *vol;
	diff = (diff > 0) ? (comp->atk * diff) : (comp->rel * diff);
	*vol = *vol + diff;

	diff = ((x <= comp->thresh) ? 1.0 : (x / (comp->thresh + (x - comp->thresh) * comp->ratio))) - *ctrl;
	diff = (diff > 0) ? (comp->atk * diff) : (comp->rel * diff);
	*ctrl = *ctrl + diff;

	return y / *ctrl;
}

#endif
