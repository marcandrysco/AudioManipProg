#ifndef CLK_DEFS_H
#define CLK_DEFS_H

/**
 * Clock processing function.
 *   @ref: The reference.
 *   @time: The time.
 *   @len: The length.
 */

typedef void (*amp_clock_f)(void *ref, struct amp_time_t *time, unsigned int len);

/**
 * Clock interface.
 *   @info: Information.
 *   @proc: Processing.
 *   @copy: Copy.
 *   @delete: Deletion.
 */

struct amp_clock_i {
	amp_info_f info;
	amp_clock_f proc;
	amp_copy_f copy;
	amp_delete_f delete;
};

/**
 * Clock structure.
 *   @ref: The reference.
 *   @iface: The interface.
 */

struct amp_clock_t {
	void *ref;
	const struct amp_clock_i *iface;
};


/**
 * Process information on a clock.
 *   @clock: The clock.
 *   @info: The infoormation.
 */

static inline void amp_clock_info(struct amp_clock_t clock, struct amp_info_t info)
{
	clock.iface->info(clock.ref, info);
}

/**
 * Process a clock.
 *   @clock: The clock.
 *   @time: The time.
 *   @len: The length.
 */

static inline void amp_clock_proc(struct amp_clock_t clock, struct amp_time_t *time, unsigned int len)
{
	clock.iface->proc(clock.ref, time, len);
}

/**
 * Copy a clock.
 *   @clock: The original clock.
 *   &returns: The copied clock.
 */

static inline struct amp_clock_t amp_clock_copy(struct amp_clock_t clock)
{
	return (struct amp_clock_t){ clock.iface->copy(clock.ref), clock.iface };
}

/**
 * Delete a clock.
 *   @clock: The clock.
 */

static inline void amp_clock_delete(struct amp_clock_t clock)
{
	clock.iface->delete(clock.ref);
}

/**
 * Delete a clock if not null.
 *   @clock: The clock.
 */

static inline void amp_clock_erase(struct amp_clock_t clock)
{
	if(clock.iface != NULL)
		amp_clock_delete(clock);
}

/**
 * Set a clock.
 *   @dest: The destination, deleted if not null.
 *   @src: Consumed. The source clock.
 */

static inline void amp_clock_set(struct amp_clock_t *dest, struct amp_clock_t src)
{
	amp_clock_erase(*dest);
	*dest = src;
}

#endif
