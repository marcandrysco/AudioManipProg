#ifndef EFX_CLIP_H
#define EFX_CLIP_H

/**
 * Clipping type enumerator.
 *   @amp_clip_hard_v: Hard clipping.
 *   @amp_clip_poly_v: Polynomial clipping.
 *   @amp_clip_root_v: Root clipping.
 *   @amp_clip_log_v: Logarithmic clipping.
 */
enum amp_clip_e {
	amp_clip_hard_v,
	amp_clip_poly_v,
	amp_clip_root_v,
	amp_clip_log_v
};


/*
 * clip declarations
 */
extern const struct amp_effect_i amp_clip_iface;

struct amp_clipt *amp_clip_new(int dir, enum amp_clip_e type, struct amp_param_t *param1, struct amp_param_t *param2);
struct amp_clipt *amp_clip_copy(struct amp_clipt *clip);
void amp_clip_delete(struct amp_clipt *clip);

char *amp_clip_make1(struct ml_value_t **ret, int dir, enum amp_clip_e type, struct ml_value_t *value);
char *amp_hardclip_pos(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env);
char *amp_hardclip_sym(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env);
char *amp_hardclip_neg(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env);

char *amp_clip_make2(struct ml_value_t **ret, int dir, enum amp_clip_e type, struct ml_value_t *value);
char *amp_polyclip_pos(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env);
char *amp_polyclip_sym(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env);
char *amp_polyclip_neg(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env);
char *amp_rootclip_pos(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env);
char *amp_rootclip_sym(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env);
char *amp_rootclip_neg(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env);
char *amp_logclip_pos(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env);
char *amp_logclip_sym(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env);
char *amp_logclip_neg(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env);

void amp_clipinfo(struct amp_clipt *clip, struct amp_info_t info);
bool amp_clipproc(struct amp_clipt *clip, double *buf, struct amp_time_t *time, unsigned int len, struct amp_queue_t *queue);


/**
 * Perform hard clipping on a value.
 *   @val: The input.
 *   @max: The maximum level.
 *   &returns: The clipped output.
 */
static inline double amp_clip_hard(double val, double max)
{
	return (val > max) ? max : val;
}

/**
 * Perform linear clipping on a value.
 *   @val: The input.
 *   @max: The maximum level.
 *   &returns: The clipped output.
 */
static inline double amp_clip_line(double val, double sat, double dist)
{
	dist = 1.0 / (1.0 + dist);

	return (val > sat) ? (sat + dist * (val - sat)) : val;
}

/**
 * Perform clipping with quadratic saturation.
 *   @val: The input.
 *   @sat: The saturation level.
 *   @dist: The amount of distortion.
 *   &returns: The clipped output.
 */
static inline double amp_clip_poly(double val, double sat, double dist)
{
	double max = sat + 1.0 / dist;

	if(val <= sat)
		return val;
	else if(val >= (2.0 * max - sat))
		return max;
	else {
		double t = val - 2.0*max + sat;

		return -0.25 * dist * t * t + max;
	}
}

/**
 * Perform root clipping on a value.
 *   @val: The input.
 *   @sat: The saturation level.
 *   @dist: The amount of distortion.
 *   &returns: The clipped output.
 */
static inline double amp_clip_root(double val, double sat, double dist)
{
	dist = 1.0 / dist;

	if(val <= sat)
		return val;
	else
		return 2 * (sqrtf(dist * (val - sat + dist)) - dist) + sat;
}

/**
 * Perform logarithmic clipping on a value.
 *   @val: The input.
 *   @sat: The saturation level.
 *   @dist: The amount of distortion.
 *   &returns: The clipped output.
 */
static inline double amp_clip_log(double val, double sat, double dist)
{
	dist = 1.0 / dist;

	if(val <= sat)
		return val;
	else
		return dist * logf((val - sat + dist) / dist) + sat;
}

#endif
