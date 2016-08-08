#ifndef PARAM_H
#define PARAM_H

/**
 * Control structure.
 *   @exp: The exponential flag.
 *   @val, low, high: The current, low, and high values.
 *   @dev, key: The listening device and key.
 */
struct amp_ctrl_t {
	bool exp;
	double val, low, high;

	uint16_t dev, key;
};

/**
 * Parameter type enumerator.
 *   @amp_param_flt_e: Floating-point value.
 *   @amp_param_ctrl_e: Control.
 *   @amp_param_module_e: Module.
 */
enum amp_param_e {
	amp_param_flt_e,
	amp_param_ctrl_e,
	amp_param_module_e
};

/**
 * Parameter data union.
 *   @ctrl: MIDI control.
 *   @module: The module.
 */
union amp_param_u {
	struct amp_ctrl_t *ctrl;
	struct amp_module_t module;
};

/**
 * Parameter structure.
 *   @flt: The floating-point value.
 *   @type: The type.
 *   @data: The data.
 */
struct amp_param_t {
	double flt;
	enum amp_param_e type;
	union amp_param_u data;
};


/*
 * parameter declarations
 */
struct amp_param_t *amp_param_new(double flt, enum amp_param_e type, union amp_param_u data);
struct amp_param_t *amp_param_copy(struct amp_param_t *param);
void amp_param_delete(struct amp_param_t *param);

struct amp_param_t *amp_param_flt(double flt);
struct amp_param_t *amp_param_ctrl(struct amp_ctrl_t *ctrl);
struct amp_param_t *amp_param_module(struct amp_module_t module);

void amp_param_info(struct amp_param_t *param, struct amp_info_t info);
bool amp_param_proc(struct amp_param_t *param, double *buf, struct amp_time_t *time, unsigned int len, struct amp_queue_t *queue);


/**
 * Check if a parameter is fast/constant.
 *   @param: The parameter.
 *   &returns: True if parameter is fast/constant.
 */
static inline bool amp_param_isfast(struct amp_param_t *param)
{
	return param->type == amp_param_flt_e;
}

/**
 * Delete a parameter if not null.
 *   @param: The parameter.
 */
static inline void amp_param_erase(struct amp_param_t *param)
{
	if(param != NULL)
		amp_param_delete(param);
}

/**
 * Set a parameter.
 *   @dest: The destination, deleted if not null.
 *   @src: Consumed. The source parameter.
 */
static inline void amp_param_set(struct amp_param_t **dest, struct amp_param_t *src)
{
	amp_param_erase(*dest);
	*dest = src;
}

#endif
