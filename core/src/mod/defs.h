#ifndef MOD_DEFS_H
#define MOD_DEFS_H

/**
 * Module processing function.
 *   @ref: The reference.
 *   @buf: The buffer.
 *   @time: The time.
 *   @len: The length.
 *   @queue: The action queue.
 *   @cont: The continuation flag.
 */
typedef bool (*amp_module_f)(void *ref, double *buf, struct amp_time_t *time, unsigned int len, struct amp_queue_t *queue);

/**
 * Module interface.
 *   @info: Information.
 *   @proc; Procesing.
 *   @copy: Copy.
 *   @delete: Delete.
 */
struct amp_module_i {
	amp_info_f info;
	amp_module_f proc;
	amp_copy_f copy;
	amp_delete_f delete;
};

/**
 * Module structure.
 *   @ref: The reference.
 *   @iface: The interface.
 */
struct amp_module_t {
	void *ref;
	const struct amp_module_i *iface;
};


/**
 * Process information on a module.
 *   @module: The module.
 *   @info: The information.
 */
static inline void amp_module_info(struct amp_module_t module, struct amp_info_t info)
{
	module.iface->info(module.ref, info);
}

/**
 * Process a module.
 *   @module: The module.
 *   @buf: The buffer.
 *   @time: The time.
 *   @len: The length.
 *   @queue: The action queue.
 *   &returns: The continuation flag.
 */
static inline bool amp_module_proc(struct amp_module_t module, double *buf, struct amp_time_t *time, unsigned int len, struct amp_queue_t *queue)
{
	return module.iface->proc(module.ref, buf, time, len, queue);
}

/**
 * Copy a module.
 *   @module: The original module.
 *   &returns: The copied module.
 */
static inline struct amp_module_t amp_module_copy(struct amp_module_t module)
{
	return (struct amp_module_t){ module.iface->copy(module.ref), module.iface };
}

/**
 * Delete a module.
 *   @module: The module.
 */
static inline void amp_module_delete(struct amp_module_t module)
{
	module.iface->delete(module.ref);
}

#endif
