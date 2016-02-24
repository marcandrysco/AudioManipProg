#include "../common.h"


/**
 * Pan structure.
 */

struct amp_pan_t {
};


/*
 * local declarations
 */

struct amp_instr_i amp_pan_iface = {
	(amp_info_f)amp_pan_info,
	(amp_instr_f)amp_pan_proc,
	(amp_copy_f)amp_pan_copy,
	(amp_delete_f)amp_pan_delete
};


/**
 * Create a new pan.
 *   &returns: The pan.
 */

struct amp_pan_t *amp_pan_new(void)
{
	struct amp_pan_t *pan;

	pan = malloc(sizeof(struct amp_pan_t));

	return pan;
}

/**
 * Copy a pan.
 *   @pan: The original pan.
 *   &returns: The copied pan.
 */

struct amp_pan_t *amp_pan_copy(struct amp_pan_t *pan)
{
	return amp_pan_new();
}

/**
 * Delete a pan.
 *   @pan: The pan.
 */

void amp_pan_delete(struct amp_pan_t *pan)
{
	free(pan);
}


/**
 * Create a pan from a value.
 *   @value: The value.
 *   @env: The environment.
 *   @err: The rror.
 *   &returns: The value or null.
 */

struct ml_value_t *amp_pan_make(struct ml_value_t *value, struct ml_env_t *env, char **err)
{
	return NULL;
	/*
#undef fail
#define fail(...) do { ml_value_delete(value); *err = amp_printf(__VA_ARGS__); return NULL; } while(0)

	struct amp_pan_t *pan;
	struct ml_link_t *link;

	if(value->type != ml_value_list_e)
		fail("Type error. Instrument pan requires a list of instrument as input.");

	for(link = value->data.list.head; link != NULL; link = link->next) {
		if(amp_unbox_value(link->value, amp_box_instr_e) == NULL)
			fail("Type error. Instrument pan requires a list of instrument as input.");
	}

	pan = amp_pan_new();

	for(link = value->data.list.head; link != NULL; link = link->next)
		amp_pan_append(pan, amp_instr_copy(amp_unbox_value(link->value, amp_box_instr_e)->data.instr));

	ml_value_delete(value);

	return amp_pack_instr((struct amp_instr_t){ pan, &amp_pan_iface });
	*/
}


/**
 * Handle information on a pan.
 *   @pan: The pan.
 *   @info: The information.
 */

void amp_pan_info(struct amp_pan_t *pan, struct amp_info_t info)
{
}

/**
 * Process a pan.
 *   @pan: The pan.
 *   @buf: The buffer.
 *   @time: The time.
 *   @len: The length.
 */

void amp_pan_proc(struct amp_pan_t *pan, double **buf, struct amp_time_t *time, unsigned int len)
{
}
