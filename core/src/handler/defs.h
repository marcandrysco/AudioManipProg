#ifndef HANDLER_DEFS_H
#define HANDLER_DEFS_H

/**
 * Handler processing function.
 *   @ref: The reference.
 *   @event: The event.
 *   &returns: The value.
 */

typedef double (*amp_handler_f)(void *ref, struct amp_event_t event);

/**
 * Handler interface.
 *   @proc: Process.
 *   @copy: Copy.
 *   @deelte: Delete.
 */

struct amp_handler_i {
	amp_handler_f proc;
	amp_copy_f copy;
	amp_delete_f delete;
};

/**
 * Handler structure.
 *   @ref: The reference.
 *   @iface: The interface.
 */

struct amp_handler_t {
	void *ref;
	const struct amp_handler_i *iface;
};


/**
 * Value structure.
 *   @flt: The current value.
 *   @handler: The handler.
 */

struct amp_value_t {
	double flt;

	struct amp_handler_t handler;
};

#endif
