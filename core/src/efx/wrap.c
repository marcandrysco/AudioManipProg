#include "../common.h"

/*
 * global variables
 */
const struct amp_effect_i amp_wrap_iface = {
	(amp_info_f)amp_wrap_info,
	(amp_effect_f)amp_wrap_proc,
	(amp_copy_f)amp_wrap_copy,
	(amp_delete_f)amp_wrap_delete
};


/**
 * Create a wrap effect.
 *   @limit: Consumed. The limit.
 *   &returns: The wrap.
 */
struct amp_wrap_t *amp_wrap_new(struct amp_param_t *limit)
{
	struct amp_wrap_t *wrap;

	wrap = malloc(sizeof(struct amp_wrap_t));
	wrap->limit = limit;

	return wrap;
}

/**
 * Copy a wrap effect.
 *   @wrap: The original wrap.
 *   &returns: The copied wrap.
 */
struct amp_wrap_t *amp_wrap_copy(struct amp_wrap_t *wrap)
{
	return amp_wrap_new(amp_param_copy(wrap->limit));
}

/**
 * Delete a wrap effect.
 *   @wrap: The wrap.
 */
void amp_wrap_delete(struct amp_wrap_t *wrap)
{
	amp_param_delete(wrap->limit);
	free(wrap);
}


/**
 * Create a wrap from a value.
 *   @ret: Ref. The returned value.
 *   @value: The value.
 *   @env: The environment.
 *   &returns: Error.
 */
char *amp_wrap_make(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env)
{
#define onexit
	struct amp_param_t *limit;

	chkfail(amp_match_unpack(value, "P", &limit));

	*ret = amp_pack_effect((struct amp_effect_t){ amp_wrap_new(limit), &amp_wrap_iface });
	return NULL;
#undef onexit
}


/**
 * Handle information on a wrap.
 *   @wrap: The wrap.
 *   @info: The information.
 */
void amp_wrap_info(struct amp_wrap_t *wrap, struct amp_info_t info)
{
	amp_param_info(wrap->limit, info);
}

static inline double dsp_wrap_d(double v, double limit)
{
	double s;

	v += limit;
	s = copysign(1.0, v);
	v = copysign(fabs(v), s);
	v = limit - fabs(dsp_mod_d(v, 4.0 * limit) - 2.0 * limit);

	return v;
}

/**
 * Process a wrap.
 *   @wrap: The wrap.
 *   @buf: The buffer.
 *   @time: The time.
 *   @len: The length.
 *   @queue: The action queue.
 *   &returns: The continuation flag.
 */
bool amp_wrap_proc(struct amp_wrap_t *wrap, double *buf, struct amp_time_t *time, unsigned int len, struct amp_queue_t *queue)
{
	unsigned int i;
	bool cont = false;

	if(amp_param_isfast(wrap->limit)) {
		double limit = wrap->limit->flt;

		for(i = 0; i < len; i++)
			buf[i] = dsp_wrap_d(buf[i], limit);
	}
	else {
		double limit[len];

		cont |= amp_param_proc(wrap->limit, limit, time, len, queue);

		for(i = 0; i < len; i++)
			buf[i] = dsp_wrap_d(buf[i], limit[i]);
	}

	return cont;
}
