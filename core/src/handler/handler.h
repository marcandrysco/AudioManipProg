#ifndef HANDLER_HANDLER_H
#define HANDLER_HANDLER_H

/*
 * handler declarations
 */

extern struct amp_handler_t amp_handler_null;


/*
 * value declarations
 */

struct amp_value_t amp_value_make(struct ml_value_t *value, struct ml_env_t *env, char **err);
char *amp_value_scan(struct ml_value_t *value, struct ml_env_t *env, ...);


/**
 * Process a handler.
 *   @handler: The handler.
 *   @event: The event.
 *   &returns: The value.
 */

static inline double amp_handler_proc(struct amp_handler_t handler, struct amp_event_t event)
{
	return handler.iface->proc(handler.ref, event);
}

/**
 * Copy a handler.
 *   @handler: The handler.
 *   &returns: The copy.
 */

static inline struct amp_handler_t amp_handler_copy(struct amp_handler_t handler)
{
	return (struct amp_handler_t){ handler.iface->copy(handler.ref), handler.iface };
}

/**
 * Delete a handler.
 *   @handler: The handler.
 */

static inline void amp_handler_delete(struct amp_handler_t handler)
{
	handler.iface->delete(handler.ref);
}

/**
 * Delete a handler if non-null.
 *   @handler: The handler.
 */

static inline void amp_handler_erase(struct amp_handler_t handler)
{
	if(handler.iface != NULL)
		amp_handler_delete(handler);
}


/**
 * Create a value.
 *   @flt: The initial floating-point value.
 *   @handler: Consumed. The handler.
 *   &returns: The value.
 */

static inline struct amp_value_t amp_value_new(double flt, struct amp_handler_t handler)
{
	return (struct amp_value_t){ flt, handler };
}

/**
 * Create a handler value.
 *   @handler: Consumed. The handler.
 *   &returns: The value.
 */

static inline struct amp_value_t amp_value_handler(struct amp_handler_t handler)
{
	return amp_value_new(amp_handler_proc(handler, (struct amp_event_t){ UINT16_MAX, UINT16_MAX, UINT16_MAX / 2 }), handler);
}

/**
 * Create a float-only value.
 *   @flt: The initial floating-point value.
 *   &returns: The value.
 */

static inline struct amp_value_t amp_value_flt(double flt)
{
	return amp_value_new(flt, amp_handler_null);
}

/**
 * Copy a value.
 *   @value: The value.
 *   &returns: The copy.
 */

static inline struct amp_value_t amp_value_copy(struct amp_value_t value)
{
	return (struct amp_value_t){ value.flt, value.handler.iface ? amp_handler_copy(value.handler) : amp_handler_null };
}

/**
 * Delete a value.
 *   @value; The value.
 */

static inline void amp_value_delete(struct amp_value_t value)
{
	amp_handler_erase(value.handler);
}

/**
 * Process an event on a value.
 *   @value: The value.
 *   @event: The event.
 */

static inline void amp_value_event(struct amp_value_t *value, struct amp_event_t event)
{
	if(value->handler.iface == NULL)
		return;

	value->flt = amp_handler_proc(value->handler, event);
}

#endif
