#include "../common.h"


/**
 * Clip structure.
 *   @maxlo, satlo, sathi, maxhi: Clip parameters.
 */

struct amp_clip_t {
	struct amp_value_t maxlo, satlo, sathi, maxhi;
};


/*
 * global variables
 */

const struct amp_effect_i amp_clip_iface = {
	(amp_info_f)amp_clip_info,
	(amp_effect_f)amp_clip_proc,
	(amp_copy_f)amp_clip_copy,
	(amp_delete_f)amp_clip_delete
};


/**
 * Create a clip effect.
 *   @maxlo: Maximum low.
 *   @satlo: Saturation low.
 *   @sathi: Saturation high.
 *   @maxhi: Maximum high.
 *   &returns: The clip.
 */

struct amp_clip_t *amp_clip_new(struct amp_value_t maxlo, struct amp_value_t satlo, struct amp_value_t sathi, struct amp_value_t maxhi)
{
	struct amp_clip_t *clip;

	clip = malloc(sizeof(struct amp_clip_t));
	clip->maxlo = maxlo;
	clip->satlo = satlo;
	clip->sathi = sathi;
	clip->maxhi = maxhi;

	return clip;
}

/**
 * Copy a clip effect.
 *   @clip: The original clip.
 *   &returns: The copied clip.
 */

struct amp_clip_t *amp_clip_copy(struct amp_clip_t *clip)
{
	return amp_clip_new(amp_value_copy(clip->maxlo), amp_value_copy(clip->satlo), amp_value_copy(clip->sathi), amp_value_copy(clip->maxhi));
}

/**
 * Delete a clip effect.
 *   @clip: The clip.
 */

void amp_clip_delete(struct amp_clip_t *clip)
{
	amp_value_delete(clip->maxlo);
	amp_value_delete(clip->satlo);
	amp_value_delete(clip->sathi);
	amp_value_delete(clip->maxhi);
	free(clip);
}


/**
 * Create a clip from a value.
 *   @value: The value.
 *   @env: The environment.
 *   @err: The rror.
 *   &returns: The value or null.
 */

struct ml_value_t *amp_clip_make(struct ml_value_t *value, struct ml_env_t *env, char **err)
{
	struct amp_value_t maxlo, satlo, sathi, maxhi;

	*err = amp_match_unpack(value, "(V,V,V,V)", &maxlo, &satlo, &sathi, &maxhi);

	return (*err == NULL) ? amp_pack_effect((struct amp_effect_t){ amp_clip_new(maxlo, satlo, sathi, maxhi), &amp_clip_iface }) : NULL;
}


/**
 * Handle information on a clip.
 *   @clip: The clip.
 *   @info: The info.
 */

void amp_clip_info(struct amp_clip_t *clip, struct amp_info_t info)
{
}

/**
 * Perform clipping with quadratic saturation. The inequality
 * 'maxlo <= satlo <= 0.0 <= sathi <= maxhi' must hold.
 *   @val: The input.
 *   @maxlo: The low maximum level.
 *   @satlo: The low saturation level.
 *   @sathi: The high saturation level.
 *   @maxhi: The high maximum level.
 *   &returns: The clipped output.
 */

static inline double dsp_clip_d(double val, double maxlo, double satlo, double sathi, double maxhi)
{
	if(val >= satlo) {
		double h = 2.0 * maxhi - sathi;
		double a = 1.0 / (4.0 * (sathi - maxhi));

		if(val > h)
			return maxhi;
		else if(val > sathi)
			return a * (val - h) * (val - h) + maxhi;
		else 
			return val;
	}
	else
		return -dsp_clip_d(-val, 0.0, 0.0, -satlo, -maxlo);
}

/**
 * Process a clip.
 *   @clip: The clip.
 *   @buf: The buffer.
 *   @time: The time.
 *   @len: The length.
 */

void amp_clip_proc(struct amp_clip_t *clip, double *buf, struct amp_time_t *time, unsigned int len)
{
	unsigned int i;
	double maxlo, satlo, sathi, maxhi;

	maxlo = clip->maxlo.flt;
	satlo = clip->satlo.flt;
	sathi = clip->sathi.flt;
	maxhi = clip->maxhi.flt;

	for(i = 0; i < len; i++)
		buf[i] = dsp_clip_d(buf[i], maxlo, satlo, sathi, maxhi);
}
