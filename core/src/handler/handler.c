#include "../common.h"


/*
 * global variables
 */

struct amp_handler_t amp_handler_null = { NULL, NULL };


/**
 * Create a value from a value.
 *   @value: The value.
 *   @env: The environment.
 *   @err: The error.
 *   &returns: The value.
 */

struct amp_value_t amp_value_make(struct ml_value_t *value, struct ml_env_t *env, char **err)
{
	struct amp_box_t *box;

	if(value->type == ml_value_num_e)
		return amp_value_flt(value->data.num);
	else if((box = amp_unbox_value(value, amp_box_handler_e)) != NULL)
		return amp_value_new(amp_handler_proc(box->data.handler, (struct amp_event_t){ UINT16_MAX, UINT16_MAX, UINT16_MAX / 2 }), amp_handler_copy(box->data.handler));

	amp_eprintf(err, "Invalid value.");

	return amp_value_flt(value->data.num);
}

/**
 * Scan a list of AMP values from an ML value.
 *   @value: The ML value.
 *   @env: The environment.
 *   @...: The null-terminated list of AMP value pointers.
 *   &returns: Null on success, allocated error on failure.
 */

char *amp_value_scan(struct ml_value_t *value, struct ml_env_t *env, ...)
{
	char *err;
	va_list args;
	unsigned int i, n;
	struct amp_value_t *dest;

	va_start(args, env);

	n = 0;
	while(va_arg(args, struct amp_value_t *) != NULL)
		n++;

	va_end(args);

	if(value->type != ml_value_tuple_e)
		return amp_printf("Type mismatch. Expected tuple.");

	if(value->data.tuple.len != n)
		return amp_printf("Type mismatch. Expected %u-tuple, found %u-tuple.", n, value->data.tuple.len);

	va_start(args, env);

	err = NULL;
	for(n = 0; (dest = va_arg(args, struct amp_value_t *)) != NULL; n++) {
		*dest = amp_value_make(value->data.tuple.value[n], env, &err);
		if(err != NULL)
			break;
	}

	va_end(args);

	if(err != NULL) {
		va_start(args, env);

		for(i = 0; i < n; i++)
			amp_value_delete(*va_arg(args, struct amp_value_t *));

		va_end(args);
	}

	return err;
}
