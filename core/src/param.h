#ifndef PARAM_H
#define PARAM_H

/**
 * Parameter type enumerator.
 *   @amp_param_flt_e: Floating-point value.
 *   @amp_param_handler_e: MIDI event handler.
 *   @amp_param_module_e: Module.
 */

enum amp_param_e {
	amp_param_flt_e,
	amp_param_handler_e,
	amp_param_module_e
};

/**
 * Parameter data union.
 *   @handler: The handler.
 *   @module: The module.
 */

union amp_param_u {
	struct amp_handler_t handler;
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
struct amp_param_t *amp_param_handler(struct amp_handler_t handler);
struct amp_param_t *amp_param_module(struct amp_module_t module);

void amp_param_info(struct amp_param_t *param, struct amp_info_t info);
bool amp_param_proc(struct amp_param_t *param, double *buf, struct amp_time_t *time, unsigned int len);


/**
 * Check if a parameter is fast/constant.
 *   @param: The parameter.
 *   &returns: True if parameter is fast/constant.
 */

static inline bool amp_param_isfast(struct amp_param_t *param)
{
	return param->type != amp_param_module_e;
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
