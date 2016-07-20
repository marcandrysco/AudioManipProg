#include "../common.h"


/**
 * Injector structure.
 *   @seq: The sequencer.
 */
struct amp_inject_t {
	struct amp_seq_t seq;
};


/*
 * global declarations
 */
struct amp_poly_i amp_inject_iface = { (amp_poly_f)amp_inject_proc, (amp_info_f)amp_inject_info, (copy_f)amp_inject_copy, (delete_f)amp_inject_delete };


/**
 * Create a new injector.
 *   @id: The identifier.
 *   @core: The core.
 *   &returns: The injector.
 */
struct amp_inject_t *amp_inject_new(struct amp_seq_t seq)
{
	struct amp_inject_t *inject;

	inject = malloc(sizeof(struct amp_inject_t));
	inject->seq = seq;

	return inject;
}

/**
 * Copy an injector.
 *   @inject: The injector.
 *   &returns: The copy.
 */
struct amp_inject_t *amp_inject_copy(struct amp_inject_t *inject)
{
	return amp_inject_new(amp_seq_copy(inject->seq));
}

/**
 * Delete an injector.
 *   @inject: The injector.
 */
void amp_inject_delete(struct amp_inject_t *inject)
{
	amp_seq_delete(inject->seq);
	free(inject);
}


/**
 * Create a gain from a value.
 *   @ret: Ref. The returned value.
 *   @value: The value.
 *   @env: The environment.
 *   &returns: Error.
 */
char *amp_inject_make(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env)
{
#define onexit
	struct amp_seq_t seq;
	struct amp_box_t *box;

	chkfail(amp_match_unpack(value, "(S,*)", &seq, &box));
	*ret = amp_poly_make(box, amp_inject_new(seq), &amp_inject_iface);

	return NULL;
#undef onexit
}


/**
 * Process information on an injector.
 *   @inject: The injector.
 *   @info: The information.
 */
void amp_inject_info(struct amp_inject_t *inject, struct amp_info_t info)
{
	amp_seq_info(inject->seq, info);
}


/**
 * Process an injector.
 *   @inject: The injector.
 *   @poly: The polymorphic object.
 *   @info: The information.
 *   &returns: The continuation flag.
 */
bool amp_inject_proc(struct amp_inject_t *inject, struct amp_poly_t *poly, struct amp_polyinfo_t *info)
{
	bool cont;
	struct amp_queue_t copy, *orig = info->queue;

	amp_queue_copy(&copy, info->queue);
	amp_seq_proc(inject->seq, info->time, info->len, &copy);
	info->queue = &copy;
	cont = amp_poly_proc(poly, info);
	info->queue = orig;

	return cont;
}
