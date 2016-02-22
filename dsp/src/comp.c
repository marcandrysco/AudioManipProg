#include "common.h"


/**
 * Create a compressor.
 *   @rate: The sample rate.
 *   &returns: The compressor.
 */

struct dsp_comp_t dsp_comp_init(unsigned int rate)
{
	struct dsp_comp_t comp;

	comp.rate = rate;
	comp.vol = 0.0;
	comp.ctrl = 1.0;

	dsp_comp_atk(&comp, 0.0001);
	dsp_comp_rel(&comp, 0.04);
	comp.thresh = dsp_db2amp_d(-45.0);
	comp.ratio = 1.0 / dsp_db2amp_d(200.0);

	return comp;
}

/**
 * Process the compressor.
 *   @comp: The compressor.
 *   @buf: The buffer.
 *   @len: The length.
 */

void dsp_comp_proc(struct dsp_comp_t *comp, double *buf, unsigned int len)
{
	double x, y;
	unsigned int i;
	double vol, ctrl, atk, rel, diff, thresh, ratio;

	vol = comp->vol;
	ctrl = comp->ctrl;
	atk = comp->atk;
	rel = comp->rel;
	thresh = comp->thresh;
	ratio = comp->ratio;

	for(i = 0; i < len; i++) {
		y = buf[i];
		x = fabs(y);

		diff = x - vol;
		diff = (diff > 0) ? (atk * diff) : (rel * diff);
		vol = vol + diff;

		diff = ((x <= thresh) ? 1.0 : (x / (thresh + (x - thresh) * ratio))) - ctrl;
		diff = (diff > 0) ? (atk * diff) : (rel * diff);
		ctrl = ctrl + diff;

		buf[i] = y / ctrl;
	}

	comp->vol = vol;
	comp->ctrl = ctrl;
}
