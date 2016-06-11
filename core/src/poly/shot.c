#include "../common.h"


/**
 * Single shot structure.
 *   @id: The key identifier.
 *   @box: The boxed value.
 */
struct amp_shot_t {
	struct amp_id_t id;
	struct amp_box_t *box;
};


/*
 * global variables
 */
const struct amp_instr_i amp_shot_iface_instr = {
	(amp_info_f)amp_shot_info,
	(amp_instr_f)amp_shot_proc_instr,
	(amp_copy_f)amp_shot_copy,
	(amp_delete_f)amp_shot_delete
};
const struct amp_effect_i amp_shot_iface_effect = {
	(amp_info_f)amp_shot_info,
	(amp_effect_f)amp_shot_proc_effect,
	(amp_copy_f)amp_shot_copy,
	(amp_delete_f)amp_shot_delete
};
const struct amp_module_i amp_shot_iface_module = {
	(amp_info_f)amp_shot_info,
	(amp_module_f)amp_shot_proc_module,
	(amp_copy_f)amp_shot_copy,
	(amp_delete_f)amp_shot_delete
};


/**
 * Create a new single shot.
 *   @id: The key identifier.
 *   @box: The boxed value.
 *   &returns: The single shot.
 */
struct amp_shot_t *amp_shot_new(struct amp_id_t id, struct amp_box_t *box)
{
	struct amp_shot_t *shot;

	shot = malloc(sizeof(struct amp_shot_t));
	shot->id = id;
	shot->box = box;

	return shot;
}

/**
 * Copy a single shot.
 *   @shot: The original single shot.
 *   &returns: The copied single shot.
 */
struct amp_shot_t *amp_shot_copy(struct amp_shot_t *shot)
{
	return amp_shot_new(shot->id, amp_box_copy(shot->box));
}

/**
 * Delete a single shot.
 *   @shot: The single shot.
 */
void amp_shot_delete(struct amp_shot_t *shot)
{
	amp_box_delete(shot->box);
	free(shot);
}


/**
 * Handle information on a single shot.
 *   @shot: The single shot.
 *   @info: The information.
 */
void amp_shot_info(struct amp_shot_t *shot, struct amp_info_t info)
{
	switch(shot->box->type) {
	case amp_box_clock_e: amp_clock_info(shot->box->data.clock, info); break;
	case amp_box_instr_e: amp_instr_info(shot->box->data.instr, info); break;
	case amp_box_effect_e: amp_effect_info(shot->box->data.effect, info); break;
	case amp_box_module_e: amp_module_info(shot->box->data.module, info); break;
	case amp_box_seq_e: amp_seq_info(shot->box->data.seq, info); break;
	case amp_box_ctrl_e: break;
	}
}


/**
 * Create a single shot from a value.
 *   @ret: Ref. The returned value.
 *   @value: The value.
 *   @env: The environment.
 *   &returns: Error.
 */
char *amp_shot_make(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env)
{
#define onexit
	struct amp_box_t *box;
	int dev, key;

	chkfail(amp_match_unpack(value, "(*,(d,d))", &box, &dev, &key));

	switch(box->type) {
	case amp_box_instr_e: 
		*ret = amp_pack_instr((struct amp_instr_t){ amp_shot_new((struct amp_id_t){ dev, key }, box), &amp_shot_iface_instr });
		break;

	case amp_box_effect_e: 
		*ret = amp_pack_effect((struct amp_effect_t){ amp_shot_new((struct amp_id_t){ dev, key }, box), &amp_shot_iface_effect });
		break;

	case amp_box_module_e: 
		*ret = amp_pack_module((struct amp_module_t){ amp_shot_new((struct amp_id_t){ dev, key }, box), &amp_shot_iface_module });
		break;

	case amp_box_clock_e:
	case amp_box_seq_e:
	case amp_box_ctrl_e:
	default:
		fatal("stub");
	}

	return NULL;
#undef onexit
}


/**
 * Process the filter for the single shot.
 *   @shot: The single shot.
 *   @queue: The original queue.
 *   &returns: The filtered queue.
 */
static inline struct amp_queue_t amp_shot_filt(struct amp_shot_t *shot, struct amp_queue_t *queue)
{
	unsigned int n = 0;
	struct amp_queue_t filt;
	struct amp_action_t *action;

	amp_queue_init(&filt);

	while((action = amp_queue_action(queue, &n, UINT_MAX)) != NULL) {
		if((action->event.dev == shot->id.dev) && (action->event.key == shot->id.key))
			amp_queue_add(&filt, *action);
	}

	return filt;
}

/*
 * process handlers.
 */
bool amp_shot_proc_instr(struct amp_shot_t *shot, double **buf, struct amp_time_t *time, unsigned int len, struct amp_queue_t *queue)
{
	struct amp_queue_t filt;

	filt = amp_shot_filt(shot, queue);
	return amp_instr_proc(shot->box->data.instr, buf, time, len, &filt);
}
bool amp_shot_proc_effect(struct amp_shot_t *shot, double *buf, struct amp_time_t *time, unsigned int len, struct amp_queue_t *queue)
{
	struct amp_queue_t filt;

	filt = amp_shot_filt(shot, queue);
	return amp_effect_proc(shot->box->data.effect, buf, time, len, &filt);
}
bool amp_shot_proc_module(struct amp_shot_t *shot, double *buf, struct amp_time_t *time, unsigned int len, struct amp_queue_t *queue)
{
	struct amp_queue_t filt;

	filt = amp_shot_filt(shot, queue);
	return amp_module_proc(shot->box->data.module, buf, time, len, &filt);
}
