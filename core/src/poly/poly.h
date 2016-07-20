#ifndef POLY_POLY_H
#define POLY_POLY_H

/*
 * structure declarations
 */
struct amp_poly_t;


/**
 * Instrument data.
 *   @buf: The buffer.
 */
struct amp_polyinstr_t {
	double **buf;
};

/**
 * Effect data.
 *   @buf: The buffer.
 */
struct amp_polyeffect_t {
	double *buf;
};

/**
 * Module data.
 *   @buf: The buffer.
 */
struct amp_polymodule_t {
	double *buf;
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

/**
 * Polymorphic information structure.
 *   @len: The length.
 *   @queue: The queue.
 *   @data: The data.
 */
struct amp_polyinfo_t {
	unsigned int len;
	struct amp_time_t *time;
	struct amp_queue_t *queue;

	union amp_polyinfo_u data;
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
	amp_info_f info;
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
