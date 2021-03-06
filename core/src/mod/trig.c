#include "../common.h"


/**
 * Trigger structure.
 *   @freq, mul: The frequency and multiplier.
 */
struct amp_trig_t {
	double freq, mul;
};


/*
 * global variables
 */
const struct amp_module_i amp_trig_iface = {
	(amp_info_f)amp_trig_info,
	(amp_module_f)amp_trig_proc,
	(amp_copy_f)amp_trig_copy,
	(amp_delete_f)amp_trig_delete
};


/**
 * Create a trigger.
 *   @mul: The multiplier.
 *   &returns: The trigger.
 */
struct amp_trig_t *amp_trig_new(double mul)
{
	struct amp_trig_t *trig;

	trig = malloc(sizeof(struct amp_trig_t));
	trig->mul = mul;
	trig->freq = 0.0;

	return trig;
}

/**
 * Copy a trigger.
 *   @trig: The original trigger.
 *   &returns: The copied trigger.
 */
struct amp_trig_t *amp_trig_copy(struct amp_trig_t *trig)
{
	return amp_trig_new(trig->mul);
}

/**
 * Delete a trigger.
 *   @trig: The trigger.
 */
void amp_trig_delete(struct amp_trig_t *trig)
{
	free(trig);
}


/**
 * Create a trigger from a value.
 *   @ret: Ref. The returned value.
 *   @value: The value.
 *   @env: The environment.
 *   &returns: Error.
 */
char *amp_trig_make(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env)
{
	if(!ml_value_isnum(value))
		return amp_printf("Type mismatch. Expected number.");

	*ret = amp_pack_module((struct amp_module_t){ amp_trig_new(ml_value_getflt(value)), &amp_trig_iface });
	return NULL;
}


/**
 * Handle information on a trigger.
 *   @trig: The trigger.
 *   @info: The information.
 */
void amp_trig_info(struct amp_trig_t *trig, struct amp_info_t info)
{
	if(info.type == amp_info_note_e)
		trig->freq = info.data.note->freq * trig->mul;
}

/**
 * Process a trigger.
 *   @trig: The trigger.
 *   @buf: The buffer.
 *   @time: The time.
 *   @len: The length.
 *   @queue: The action queue.
 *   &returns: The continuation flag.
 */
bool amp_trig_proc(struct amp_trig_t *trig, double *buf, struct amp_time_t *time, unsigned int len, struct amp_queue_t *queue)
{
	unsigned int i;

	for(i = 0; i < len; i++)
		buf[i] = trig->freq;

	return false;
}
