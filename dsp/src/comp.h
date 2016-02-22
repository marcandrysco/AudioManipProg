#ifndef COMP_H
#define COMP_H

/**
 * Compressor structure.
 *   @rate: The sample rate.
 *   @vol, ctrl: The volume and control parameters.
 */

struct dsp_comp_t {
	unsigned int rate;

	double vol, ctrl;
	double atk, rel, thresh, ratio;
};


/*
 * compressor declarations
 */

struct dsp_comp_t dsp_comp_init(unsigned int rate);
void dsp_comp_proc(struct dsp_comp_t *comp, double *buf, unsigned int len);


/**
 * Set the attack on the compressor.
 *   @comp: The compressor.
 *   @len: The attack length in seconds.
 */

static inline void dsp_comp_atk(struct dsp_comp_t *comp, double len)
{
	comp->atk = 1.0 - dsp_decay_d(0.95, len, comp->rate);
}

/**
 * Set the release on the compressor.
 *   @comp: The compressor.
 *   @len: The release length in seconds.
 */

static inline void dsp_comp_rel(struct dsp_comp_t *comp, double len)
{
	comp->rel = 1.0 - dsp_decay_d(0.95, len, comp->rate);
}

/**
 * Set the threshold on the compressor.
 *   @comp: The compressor.
 *   @thresh: The threshold in decibels.
 */

static inline void dsp_comp_thresh(struct dsp_comp_t *comp, double db)
{
	comp->thresh = dsp_db2amp_d(db);
}

/**
 * Set the ratio on the compressor.
 *   @comp: The compressor.
 *   @thresh: The ratio.
 */

static inline void dsp_comp_ratio(struct dsp_comp_t *comp, double db)
{
	comp->ratio = 1.0 / dsp_db2amp_d(db);
}

#endif
