#ifndef POLY_POLY_H
#define POLY_POLY_H

/*
 * structure declarations
 */
struct amp_poly_t;


/**
 * Instrument data.
 *   @buf: The buffer.
 *   @time: The time.
 *   @len: The length.
 */
struct amp_polyinstr_t {
	double **buf;
	struct amp_time_t *time;
	unsigned int len;
};

/**
 * Effect data.
 *   @buf: The buffer.
 *   @time: The time.
 *   @len: The length.
 */
struct amp_polyeffect_t {
	double *buf;
	struct amp_time_t *time;
	unsigned int len;
};

/**
 * Module data.
 *   @buf: The buffer.
 *   @time: The time.
 *   @len: The length.
 */
struct amp_polymodule_t {
	double *buf;
	struct amp_time_t *time;
	unsigned int len;
};

/*
 * Polymorphic information data union.
 *   @instr: Instrument.
 *   @effect: Effect.
 *   @module: Module.
 */
union amp_polyinfo_u {
	struct amp_polyinstr_t instr;
	struct amp_polyeffect_t effect;
	struct amp_polymodule_t module;
};

struct amp_polyinfo_t {
	union amp_polyinfo_u data;
	struct amp_queue_t *queue;
};

/**
 * Polymorphic callback.
 *   @ref: The reference.
 *   @poly: The polymorphic object.
 *   @info: The call information.
 *   &returns: The continuation flag.
 */
typedef bool (*amp_poly_f)(void *ref, struct amp_poly_t *poly, struct amp_polyinfo_t *info);

/**
 * Polymorphic interface.
 *   @proc: Process.
 *   @copy: Copy.
 *   @delete: Delete.
 */
struct amp_poly_i {
	amp_poly_f proc;
	copy_f copy;
	delete_f delete;
};

/*
 * polymorphic declarations
 */
struct amp_poly_t *amp_poly_new(struct amp_box_t *box, void *ref, const struct amp_poly_i *iface);
struct amp_poly_t *amp_poly_copy(struct amp_poly_t *poly);
void amp_poly_delete(struct amp_poly_t *poly);

struct ml_value_t *amp_poly_make(struct amp_box_t *box, void *ref, const struct amp_poly_i *iface);

void amp_poly_info(struct amp_poly_t *poly, struct amp_info_t info);
bool amp_poly_proc(struct amp_poly_t *poly, struct amp_polyinfo_t *info);
bool amp_poly_proc_instr(struct amp_poly_t *poly, double **buf, struct amp_time_t *time, unsigned int len, struct amp_queue_t *queue);
bool amp_poly_proc_effect(struct amp_poly_t *poly, double *buf, struct amp_time_t *time, unsigned int len, struct amp_queue_t *queue);
bool amp_poly_proc_module(struct amp_poly_t *poly, double *buf, struct amp_time_t *time, unsigned int len, struct amp_queue_t *queue);

#endif
