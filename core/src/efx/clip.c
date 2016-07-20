#include "../common.h"

/**
 * Clipping structure.
 *   @dir: The direction, with 1 as positive, -1 as negative, 0 is symmetric.
 *   @type: The type.
 *   @pos, neg: The positive and negative flags.
 *   @sat, dist: The saturation and distortion parameters.
 */
struct amp_clipt {
	int dir;
	enum amp_clip_e type;
	struct amp_param_t *sat, *dist;
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
 * Create a clipper.
 *   @dir: The direction.
 *   @type: The type.
 *   @sat: The saturation parameter.
 *   @dist: The distortion parameter.
 *   &returns: The clipper.
 */
struct amp_clipt *amp_clip_new(int dir, enum amp_clip_e type, struct amp_param_t *sat, struct amp_param_t *dist)
{
	struct amp_clipt *clip;

	clip = malloc(sizeof(struct amp_clipt));
	clip->dir = dir;
	clip->type = type;
	clip->sat = sat;
	clip->dist = dist;

	return clip;
}

/**
 * Copy a cliper.
 *   @clip: The original clippe.
 *   &returns: The copied clipper.
 */
struct amp_clipt *amp_clip_copy(struct amp_clipt *clip)
{
	return amp_clip_new(clip->dir, clip->type, amp_param_copy(clip->sat), amp_param_copy(clip->dist));
}

/**
 * Delete a clipper.
 *   @clip: The clipper.
 */
void amp_clip_delete(struct amp_clipt *clip)
{
	amp_param_delete(clip->sat);
	amp_param_delete(clip->dist);
	free(clip);
}


/**
 * Create a one parameter clip from a value.
 *   @ret: Ref. The returned value.
 *   @value: The value.
 *   @env: The environment.
 *   &returns: Error.
 */
char *amp_clip_make1(struct ml_value_t **ret, int dir, enum amp_clip_e type, struct ml_value_t *value)
{
#define onexit
	struct amp_param_t *sat;

	chkfail(amp_match_unpack(value, "P", &sat));

	*ret = amp_pack_effect((struct amp_effect_t){ amp_clip_new(dir, type, sat, amp_param_flt(1.0)), &amp_clip_iface });
	return NULL;
#undef onexit
}
char *amp_hardclip_pos(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env)
{
	return amp_clip_make1(ret, 1, amp_clip_hard_v, value);
}
char *amp_hardclip_sym(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env)
{
	return amp_clip_make1(ret, 0, amp_clip_hard_v, value);
}
char *amp_hardclip_neg(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env)
{
	return amp_clip_make1(ret, -1, amp_clip_hard_v, value);
}

/**
 * Create a two parameter clip from a value.
 *   @ret: Ref. The returned value.
 *   @value: The value.
 *   @env: The environment.
 *   &returns: Error.
 */
char *amp_clip_make2(struct ml_value_t **ret, int dir, enum amp_clip_e type, struct ml_value_t *value)
{
#define onexit
	struct amp_param_t *sat, *dist;

	chkfail(amp_match_unpack(value, "(P,P)", &sat, &dist));

	*ret = amp_pack_effect((struct amp_effect_t){ amp_clip_new(dir, type, sat, dist), &amp_clip_iface });
	return NULL;
#undef onexit
}
char *amp_linclip_pos(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env)
{
	return amp_clip_make2(ret, 1, amp_clip_lin_v, value);
}
char *amp_linclip_sym(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env)
{
	return amp_clip_make2(ret, 0, amp_clip_lin_v, value);
}
char *amp_linclip_neg(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env)
{
	return amp_clip_make2(ret, -1, amp_clip_lin_v, value);
}
char *amp_polyclip_pos(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env)
{
	return amp_clip_make2(ret, 1, amp_clip_poly_v, value);
}
char *amp_polyclip_sym(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env)
{
	return amp_clip_make2(ret, 0, amp_clip_poly_v, value);
}
char *amp_polyclip_neg(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env)
{
	return amp_clip_make2(ret, -1, amp_clip_poly_v, value);
}
char *amp_rootclip_pos(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env)
{
	return amp_clip_make2(ret, 1, amp_clip_root_v, value);
}
char *amp_rootclip_sym(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env)
{
	return amp_clip_make2(ret, 0, amp_clip_root_v, value);
}
char *amp_rootclip_neg(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env)
{
	return amp_clip_make2(ret, -1, amp_clip_root_v, value);
}
char *amp_logclip_pos(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env)
{
	return amp_clip_make2(ret, 1, amp_clip_log_v, value);
}
char *amp_logclip_sym(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env)
{
	return amp_clip_make2(ret, 0, amp_clip_log_v, value);
}
char *amp_logclip_neg(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env)
{
	return amp_clip_make2(ret, -1, amp_clip_log_v, value);
}


/**
 * Handle information on a cliper.
 *   @clip: The cliper.
 *   @info: The info.
 */
void amp_clip_info(struct amp_clipt *clip, struct amp_info_t info)
{
	amp_param_info(clip->sat, info);
	amp_param_info(clip->dist, info);
}

/**
 * Process a cliper.
 *   @clip: The clip.
 *   @buf: The buffer.
 *   @time: The time.
 *   @len: The length.
 *   @queue: The action queue.
 *   &returns: The continuation flag.
 */
bool amp_clip_proc(struct amp_clipt *clip, double *buf, struct amp_time_t *time, unsigned int len, struct amp_queue_t *queue)
{
	unsigned int i;
	bool cont = false;

	switch(clip->type) {
	case amp_clip_hard_v:
		if(amp_param_isfast(clip->sat) && amp_param_isfast(clip->dist)) {
			double sat = clip->sat->flt;

			for(i = 0; i < len; i++) {
				if((buf[i] >= 0))
					buf[i] = amp_clip_hard(buf[i], sat);
				else if((buf[i] <= 0))
					buf[i] = -amp_clip_hard(-buf[i], sat);
			}
		}
		else {
			double sat[len];

			cont |= amp_param_proc(clip->sat, sat, time, len, queue);

			for(i = 0; i < len; i++) {
				if((clip->dir >= 0) && (buf[i] >= 0))
					buf[i] = amp_clip_hard(buf[i], sat[i]);
				else if((clip->dir <= 0) && (buf[i] <= 0))
					buf[i] = -amp_clip_hard(-buf[i], sat[i]);
			}
		}

		break;

	case amp_clip_lin_v:
		if(amp_param_isfast(clip->sat) && amp_param_isfast(clip->dist)) {
			double sat = clip->sat->flt, dist = clip->dist->flt;

			for(i = 0; i < len; i++) {
				if((clip->dir >= 0) && (buf[i] >= 0))
					buf[i] = amp_clip_lin(buf[i], sat, dist);
				else if((clip->dir <= 0) && (buf[i] <= 0))
					buf[i] = -amp_clip_lin(-buf[i], sat, dist);
			}
		}
		else {
			double sat[len], dist[len];

			cont |= amp_param_proc(clip->sat, sat, time, len, queue);
			cont |= amp_param_proc(clip->dist, dist, time, len, queue);

			for(i = 0; i < len; i++) {
				if((clip->dir >= 0) && (buf[i] >= 0))
					buf[i] = amp_clip_lin(buf[i], sat[i], dist[i]);
				else if((clip->dir <= 0) && (buf[i] <= 0))
					buf[i] = -amp_clip_lin(-buf[i], sat[i], dist[i]);
			}
		}

		break;

	case amp_clip_poly_v:
		if(amp_param_isfast(clip->sat) && amp_param_isfast(clip->dist)) {
			double sat = clip->sat->flt, dist = clip->dist->flt;

			for(i = 0; i < len; i++) {
				if((clip->dir >= 0) && (buf[i] >= 0))
					buf[i] = amp_clip_poly(buf[i], sat, dist);
				else if((clip->dir <= 0) && (buf[i] <= 0))
					buf[i] = -amp_clip_poly(-buf[i], sat, dist);
			}
		}
		else {
			double sat[len], dist[len];

			cont |= amp_param_proc(clip->sat, sat, time, len, queue);
			cont |= amp_param_proc(clip->dist, dist, time, len, queue);

			for(i = 0; i < len; i++) {
				if((clip->dir >= 0) && (buf[i] >= 0))
					buf[i] = amp_clip_poly(buf[i], sat[i], dist[i]);
				else if((clip->dir <= 0) && (buf[i] <= 0))
					buf[i] = -amp_clip_poly(-buf[i], sat[i], dist[i]);
			}
		}

		break;

	case amp_clip_root_v:
		if(amp_param_isfast(clip->sat) && amp_param_isfast(clip->dist)) {
			double sat = clip->sat->flt, dist = clip->dist->flt;

			for(i = 0; i < len; i++) {
				if((clip->dir >= 0) && (buf[i] >= 0))
					buf[i] = amp_clip_root(buf[i], sat, dist);
				else if((clip->dir <= 0) && (buf[i] <= 0))
					buf[i] = -amp_clip_root(-buf[i], sat, dist);
			}
		}
		else {
			double sat[len], dist[len];

			cont |= amp_param_proc(clip->sat, sat, time, len, queue);
			cont |= amp_param_proc(clip->dist, dist, time, len, queue);

			for(i = 0; i < len; i++) {
				if((clip->dir >= 0) && (buf[i] >= 0))
					buf[i] = amp_clip_root(buf[i], sat[i], dist[i]);
				else if((clip->dir <= 0) && (buf[i] <= 0))
					buf[i] = -amp_clip_root(-buf[i], sat[i], dist[i]);
			}
		}

		break;

	case amp_clip_log_v:
		if(amp_param_isfast(clip->sat) && amp_param_isfast(clip->dist)) {
			double sat = clip->sat->flt, dist = clip->dist->flt;

			for(i = 0; i < len; i++) {
				if((clip->dir >= 0) && (buf[i] >= 0))
					buf[i] = amp_clip_log(buf[i], sat, dist);
				else if((clip->dir <= 0) && (buf[i] <= 0))
					buf[i] = -amp_clip_log(-buf[i], sat, dist);
			}
		}
		else {
			double sat[len], dist[len];

			cont |= amp_param_proc(clip->sat, sat, time, len, queue);
			cont |= amp_param_proc(clip->dist, dist, time, len, queue);

			for(i = 0; i < len; i++) {
				if((clip->dir >= 0) && (buf[i] >= 0))
					buf[i] = amp_clip_log(buf[i], sat[i], dist[i]);
				else if((clip->dir <= 0) && (buf[i] <= 0))
					buf[i] = -amp_clip_log(-buf[i], sat[i], dist[i]);
			}
		}

		break;
	}

	return cont;
}
