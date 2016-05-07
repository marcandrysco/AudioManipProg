#ifndef EFX_DEFS_H
#define EFX_DEFS_H

/**
 * Effect processing function.
 *   @ref: The reference.
 *   @buf: The buffer.
 *   @time: The time.
 *   @len: The length.
 *   @queue: The action queue.
 *   &returns: The continuation flag.
 */
typedef bool (*amp_effect_f)(void *ref, double *buf, struct amp_time_t *time, unsigned int len, struct amp_queue_t *queue);

/**
 * Effect interface.
 *   @info: Info.
 *   @proc: Process.
 *   @copy: Copy.
 *   @deelte: Delete.
 */

struct amp_effect_i {
	amp_info_f info;
	amp_effect_f proc;
	amp_copy_f copy;
	amp_delete_f delete;
};

/**
 * Handler structure.
 *   @ref: The reference.
 *   @iface: The interface.
 */

struct amp_effect_t {
	void *ref;
	const struct amp_effect_i *iface;
};


/**
 * Process information on an effect.
 *   @effect: The effect.
 *   @info: The infoormation.
 */

static inline void amp_effect_info(struct amp_effect_t effect, struct amp_info_t info)
{
	effect.iface->info(effect.ref, info);
}

/**
 * Process an effect.
 *   @effect: The effect.
 *   @buf: The buffer.
 *   @time: The time.
 *   @len: The length.
 *   @queue: The action queue.
 *   &returns: The continuation flag.
 */
static inline bool amp_effect_proc(struct amp_effect_t effect, double *buf, struct amp_time_t *time, unsigned int len, struct amp_queue_t *queue)
{
	return effect.iface->proc(effect.ref, buf, time, len, queue);
}

/**
 * Copy an effect.
 *   @effect: The original effect.
 *   &returns: The copied effect.
 */
struct amp_effect_t amp_effect_copy(struct amp_effect_t effect);

/**
 * Delete an effect.
 *   @effect: The effect.
 */
static inline void amp_effect_delete(struct amp_effect_t effect)
{
	effect.iface->delete(effect.ref);
}

/**
 * Delete an effect if not null.
 *   @effect: The effect.
 */
static inline void amp_effect_erase(struct amp_effect_t effect)
{
	if(effect.iface != NULL)
		amp_effect_delete(effect);
}

/**
 * Set an effect.
 *   @dest: The destination, deleted if not null.
 *   @src: Consumed. The source effect.
 */
static inline void amp_effect_set(struct amp_effect_t *dest, struct amp_effect_t src)
{
	amp_effect_erase(*dest);
	*dest = src;
}

#endif
