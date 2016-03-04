#include "../common.h"


/**
 * Pan structure.
 *   @lvol, rvol: The left and right volume.
 *   @ldelay< rdelay: The left and right ring buffers.
 */

struct amp_pan_t {
	struct amp_param_t *lvol, *rvol;
	struct dsp_ring_t *ldelay, *rdelay;
};


/*
 * local declarations
 */

struct amp_instr_i amp_pan_iface = {
	(amp_info_f)amp_pan_info,
	(amp_instr_f)amp_pan_proc,
	(amp_copy_f)amp_pan_copy,
	(amp_delete_f)amp_pan_delete
};


/**
 * Create a new pan.
 *   @lvol: The left volume.
 *   @rvol: The right volume.
 *   @ldelay: The left delay.
 *   @rdelay: The right delay.
 *   &returns: The pan.
 */

struct amp_pan_t *amp_pan_new(struct amp_param_t *lvol, struct amp_param_t *rvol, double ldelay, double rdelay, double rate)
{
	unsigned int llen, rlen;
	struct amp_pan_t *pan;

	llen = ldelay * rate;
	rlen = rdelay * rate;

	pan = malloc(sizeof(struct amp_pan_t));
	pan->lvol = lvol;
	pan->rvol = rvol;
	pan->ldelay = llen ? dsp_ring_new(llen) : NULL;
	pan->rdelay = rlen ? dsp_ring_new(rlen) : NULL;

	return pan;
}

/**
 * Copy a pan.
 *   @pan: The original pan.
 *   &returns: The copied pan.
 */

struct amp_pan_t *amp_pan_copy(struct amp_pan_t *pan)
{
	struct amp_pan_t *copy;

	copy = malloc(sizeof(struct amp_pan_t));
	copy->lvol = amp_param_copy(pan->lvol);
	copy->rvol = amp_param_copy(pan->rvol);
	copy->ldelay = pan->ldelay ? dsp_ring_new(pan->ldelay->len) : NULL;
	copy->rdelay = pan->rdelay ? dsp_ring_new(pan->rdelay->len) : NULL;

	return copy;
}

/**
 * Delete a pan.
 *   @pan: The pan.
 */

void amp_pan_delete(struct amp_pan_t *pan)
{
	amp_param_delete(pan->lvol);
	amp_param_delete(pan->rvol);
	dsp_ring_erase(pan->ldelay);
	dsp_ring_erase(pan->rdelay);
	free(pan);
}


/**
 * Create a pan from a value.
 *   @value: The value.
 *   @env: The environment.
 *   @err: The rror.
 *   &returns: The value or null.
 */

struct ml_value_t *amp_pan_make(struct ml_value_t *value, struct ml_env_t *env, char **err)
{
	struct amp_param_t *lvol, *rvol;
	double ldelay, rdelay;

	*err = amp_match_unpack(value, "((P,P),(f,f))", &lvol, &rvol, &ldelay, &rdelay);
	if(*err != NULL)
		return NULL;

	return amp_pack_instr((struct amp_instr_t){ amp_pan_new(lvol, rvol, ldelay, rdelay, amp_core_rate(env)), &amp_pan_iface });
}


/**
 * Handle information on a pan.
 *   @pan: The pan.
 *   @info: The information.
 */

void amp_pan_info(struct amp_pan_t *pan, struct amp_info_t info)
{
	amp_param_info(pan->lvol, info);
	amp_param_info(pan->rvol, info);
}

/**
 * Process a pan.
 *   @pan: The pan.
 *   @buf: The buffer.
 *   @time: The time.
 *   @len: The length.
 */

void amp_pan_proc(struct amp_pan_t *pan, double **buf, struct amp_time_t *time, unsigned int len)
{
	unsigned int i;
	struct dsp_ring_t *delay;

	delay = pan->ldelay;

	if(amp_param_isfast(pan->lvol)) {
		double vol = pan->lvol->flt;

		if(delay != NULL) {
			for(i = 0; i < len; i++)
				buf[0][i] = vol * dsp_ring_proc(delay, buf[0][i]);
		}
		else
			for(i = 0; i < len; i++)
				buf[0][i] = vol * buf[0][i];
	}
	else {
		double vol[len];

		amp_param_proc(pan->lvol, vol, time, len);

		if(delay != NULL) {
			for(i = 0; i < len; i++)
				buf[0][i] = vol[i] * dsp_ring_proc(delay, buf[0][i]);
		}
		else
			for(i = 0; i < len; i++)
				buf[0][i] = vol[i] * buf[0][i];
	}

	delay = pan->rdelay;

	if(amp_param_isfast(pan->rvol)) {
		double vol = pan->rvol->flt;

		if(delay != NULL) {
			for(i = 0; i < len; i++)
				buf[1][i] = vol * dsp_ring_proc(delay, buf[1][i]);
		}
		else
			for(i = 0; i < len; i++)
				buf[1][i] = vol * buf[1][i];
	}
	else {
		double vol[len];

		amp_param_proc(pan->rvol, vol, time, len);

		if(delay != NULL) {
			for(i = 0; i < len; i++)
				buf[1][i] = vol[i] * dsp_ring_proc(delay, buf[1][i]);
		}
		else
			for(i = 0; i < len; i++)
				buf[1][i] = vol[i] * buf[1][i];
	}
}
