#include "common.h"


/**
 * Create a new parameter.
 *   @flt: The initial floating-point value.
 *   @type: The type.
 *   @data: Consumed. The data.
 */

struct amp_param_t *amp_param_new(double flt, enum amp_param_e type, union amp_param_u data)
{
	struct amp_param_t *param;
	
	param = malloc(sizeof(struct amp_param_t));
	param->flt = flt;
	param->type = type;
	param->data = data;

	return param;
}

/**
 * Copy a parameter.
 *   @param: The original parameter.
 *   &returns: The copied parameter.
 */

struct amp_param_t *amp_param_copy(struct amp_param_t *param)
{
	switch(param->type) {
	case amp_param_flt_e: return amp_param_flt(param->flt);
	case amp_param_handler_e: return amp_param_handler(amp_handler_copy(param->data.handler));
	case amp_param_module_e: return amp_param_module(amp_module_copy(param->data.module));
	}

	fprintf(stderr, "Invalid parameter type.\n"), abort();
}

/**
 * Delete a parameter.
 *   @param: The parameter.
 */

void amp_param_delete(struct amp_param_t *param)
{
	switch(param->type) {
	case amp_param_flt_e: break;
	case amp_param_handler_e: amp_handler_delete(param->data.handler); break;
	case amp_param_module_e: amp_module_delete(param->data.module); break;
	}

	free(param);
}


/**
 * Create a constant floating-point parameter.
 *   @flt: The floating-point value.
 *   &returns: The parameter.
 */

struct amp_param_t *amp_param_flt(double flt)
{
	return amp_param_new(flt, amp_param_flt_e, (union amp_param_u){ });
}

/**
 * Create a handler parameter.
 *   @handler: Consumed. The handler.
 *   &returns: The parameter.
 */

struct amp_param_t *amp_param_handler(struct amp_handler_t handler)
{
	double flt = amp_handler_proc(handler, (struct amp_event_t){ UINT16_MAX, UINT16_MAX, UINT16_MAX / 2 });

	return amp_param_new(flt, amp_param_handler_e, (union amp_param_u){ .handler = handler });
}

/**
 * Create a module parameter.
 *   @module: Consumed. The module.
 *   &returns: The parameter.
 */

struct amp_param_t *amp_param_module(struct amp_module_t module)
{
	return amp_param_new(0.0, amp_param_module_e, (union amp_param_u){ .module = module });
}


/**
 * Handle information on a parameter.
 *   @param: The parameter.
 *   @info: The information.
 */

void amp_param_info(struct amp_param_t *param, struct amp_info_t info)
{
	switch(param->type) {
	case amp_param_flt_e:
		break;

	case amp_param_handler_e:
		if(info.type == amp_info_action_e)
			param->flt = amp_handler_proc(param->data.handler, info.data.action->event);

		break;

	case amp_param_module_e:
		amp_module_info(param->data.module, info);
		break;
	}
}

/**
 * Process a parameter.
 *   @param: The parameter.
 *   @buf: The buffer.
 *   @time: The time.
 *   @len: The length.
 *   &returns: The continuation flag.
 */

bool amp_param_proc(struct amp_param_t *param, double *buf, struct amp_time_t *time, unsigned int len)
{
	switch(param->type) {
	case amp_param_flt_e:
	case amp_param_handler_e:
		{
			unsigned int i;

			for(i = 0; i < len; i++)
				buf[i] = param->flt;
		}

		return false;

	case amp_param_module_e:
		return amp_module_proc(param->data.module, buf, time, len);
	}

	fprintf(stderr, "Invalid parameter type.\n"), abort();
}
