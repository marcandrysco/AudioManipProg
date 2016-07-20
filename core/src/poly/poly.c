#include "../common.h"


/**
 * Polymorphic structure.
 *   @box: The boxed value.
 *   @ref: The reference.
 *   @iface: The interface.
 */
struct amp_poly_t {
	struct amp_box_t *box;

	void *ref;
	const struct amp_poly_i *iface;
};


/*
 * global variables
 */
const struct amp_instr_i amp_poly_iface_instr = {
	(amp_info_f)amp_poly_info,
	(amp_instr_f)amp_poly_proc_instr,
	(amp_copy_f)amp_poly_copy,
	(amp_delete_f)amp_poly_delete
};
const struct amp_effect_i amp_poly_iface_effect = {
	(amp_info_f)amp_poly_info,
	(amp_effect_f)amp_poly_proc_effect,
	(amp_copy_f)amp_poly_copy,
	(amp_delete_f)amp_poly_delete
};
const struct amp_module_i amp_poly_iface_module = {
	(amp_info_f)amp_poly_info,
	(amp_module_f)amp_poly_proc_module,
	(amp_copy_f)amp_poly_copy,
	(amp_delete_f)amp_poly_delete
};


/**
 * Create a new polymorphic object.
 *   @box: Consumed. The internal object.
 *   @ref: Consumed. The reference.
 *   @iface: The interface.
 *   &returns: The polymorphic object.
 */
struct amp_poly_t *amp_poly_new(struct amp_box_t *box, void *ref, const struct amp_poly_i *iface)
{
	struct amp_poly_t *poly;

	poly = malloc(sizeof(struct amp_poly_t));
	poly->box = box;
	poly->ref = ref;
	poly->iface = iface;

	return poly;
}

/**
 * Copy a polymorphic object.
 *   @poly: The original polymorphic object.
 *   &returns: The copied polymorphic object.
 */
struct amp_poly_t *amp_poly_copy(struct amp_poly_t *poly)
{
	return amp_poly_new(amp_box_copy(poly->box), poly->iface->copy(poly->ref), poly->iface);
}

/**
 * Delete a polymorphic object.
 *   @poly: The polymorphic object.
 */
void amp_poly_delete(struct amp_poly_t *poly)
{
	amp_box_delete(poly->box);
	poly->iface->delete(poly->ref);
	free(poly);
}


/**
 * Create a value for a polymorphic object.
 *   @box: The boxed value.
 *   @ref: Consumed. The reference.
 *   @iface: The interface.
 *   &returns: The polymorphic object.
 */
struct ml_value_t *amp_poly_make(struct amp_box_t *box, void *ref, const struct amp_poly_i *iface)
{
	switch(box->type) {
	case amp_box_instr_e: 
		return amp_pack_instr((struct amp_instr_t){ amp_poly_new(box, ref, iface), &amp_poly_iface_instr });

	case amp_box_effect_e: 
		return amp_pack_effect((struct amp_effect_t){ amp_poly_new(box, ref, iface), &amp_poly_iface_effect });

	case amp_box_module_e: 
		return amp_pack_module((struct amp_module_t){ amp_poly_new(box, ref, iface), &amp_poly_iface_module });

	case amp_box_clock_e:
	case amp_box_seq_e:
	case amp_box_ctrl_e:
		fatal("stub");
	}

	fatal("Invalid boxed type.");
}


/**
 * Handle information on a polymorphic object.
 *   @poly: The polymorphic object.
 *   @info: The information.
 */
void amp_poly_info(struct amp_poly_t *poly, struct amp_info_t info)
{
	poly->iface->info(poly->ref, info);

	switch(poly->box->type) {
	case amp_box_clock_e: amp_clock_info(poly->box->data.clock, info); break;
	case amp_box_instr_e: amp_instr_info(poly->box->data.instr, info); break;
	case amp_box_effect_e: amp_effect_info(poly->box->data.effect, info); break;
	case amp_box_module_e: amp_module_info(poly->box->data.module, info); break;
	case amp_box_seq_e: amp_seq_info(poly->box->data.seq, info); break;
	case amp_box_ctrl_e: break;
	}
}


bool amp_poly_proc(struct amp_poly_t *poly, struct amp_polyinfo_t *info)
{
	switch(poly->box->type) {
	case amp_box_clock_e: break;

	case amp_box_instr_e:
		return amp_instr_proc(poly->box->data.instr, info->data.instr.buf, info->time, info->len, info->queue);

	case amp_box_effect_e:
		return amp_effect_proc(poly->box->data.effect, info->data.effect.buf, info->time, info->len, info->queue);

	case amp_box_module_e:
		return amp_module_proc(poly->box->data.module, info->data.module.buf, info->time, info->len, info->queue);

	case amp_box_seq_e: break;
	case amp_box_ctrl_e: break;
	}

	fatal("Invalid type.");
}

/*
 * process handlers.
 */
bool amp_poly_proc_instr(struct amp_poly_t *poly, double **buf, struct amp_time_t *time, unsigned int len, struct amp_queue_t *queue)
{
	struct amp_polyinfo_t info = { len, time, queue, { .instr = { buf } } };

	return poly->iface->proc(poly->ref, poly, &info);
}
bool amp_poly_proc_effect(struct amp_poly_t *poly, double *buf, struct amp_time_t *time, unsigned int len, struct amp_queue_t *queue)
{
	struct amp_polyinfo_t info = { len, time, queue, { .effect = { buf } } };

	return poly->iface->proc(poly->ref, poly, &info);
}
bool amp_poly_proc_module(struct amp_poly_t *poly, double *buf, struct amp_time_t *time, unsigned int len, struct amp_queue_t *queue)
{
	struct amp_polyinfo_t info = { len, time, queue, { .module = { buf } } };

	return poly->iface->proc(poly->ref, poly, &info);
}
