#ifndef INSTR_DEFS_H
#define INSTR_DEFS_H

/**
 * Instrument processing function.
 *   @ref: The reference.
 *   @buf: The stereo buffers.
 *   @time: The time.
 *   @len: The length.
 */

typedef void (*amp_instr_f)(void *ref, double **buf, struct amp_time_t *time, unsigned int len);

/**
 * Instrument interface.
 *   @info: Information.
 *   @proc: Processing.
 *   @copy: Copy.
 *   @delete: Delete.
 */

struct amp_instr_i {
	amp_info_f info;
	amp_instr_f proc;
	amp_copy_f copy;
	amp_delete_f delete;
};

/**
 * Instrument structure.
 *   @ref: The reference.
 *   @iface: The interface.
 */

struct amp_instr_t {
	void *ref;
	const struct amp_instr_i *iface;
};

/**
 * Process information on an instrument.
 *   @instr: The instrument.
 *   @info: The information.
 */

static inline void amp_instr_info(struct amp_instr_t instr, struct amp_info_t info)
{
	instr.iface->info(instr.ref, info);
}

/**
 * Process an instrument.
 *   @instr: The instrument.
 *   @buf: The buffer.
 *   @time: The time.
 *   @len: The length.
 */

static inline void amp_instr_proc(struct amp_instr_t instr, double **buf, struct amp_time_t *time, unsigned int len)
{
	instr.iface->proc(instr.ref, buf, time, len);
}

/**
 * Copy an instrument.
 *   @instr: The instrument.
 *   &returns: The copy.
 */

static inline struct amp_instr_t amp_instr_copy(struct amp_instr_t instr)
{
	return (struct amp_instr_t){ instr.iface->copy(instr.ref), instr.iface };
}

/**
 * Delete an instrument.
 *   @instr: The instrument.
 */

static inline void amp_instr_delete(struct amp_instr_t instr)
{
	instr.iface->delete(instr.ref);
}

/**
 * Delete an instrument if non-null.
 *   @instr: The instrument.
 */

static inline void amp_instr_erase(struct amp_instr_t instr)
{
	if(instr.iface != NULL)
		amp_instr_delete(instr);
}

/**
 * Set an instrument.
 *   @dest: The destination, deleted if not null.
 *   @src: Consumed. The source instrument.
 */

static inline void amp_instr_set(struct amp_instr_t *dest, struct amp_instr_t src)
{
	amp_instr_erase(*dest);
	*dest = src;
}

#endif
