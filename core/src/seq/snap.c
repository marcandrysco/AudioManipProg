#include "../common.h"


/**
 * Snap structure.
 *   @time: The snap time.
 *   @len: The instance array length.
 *   @inst: The instance array.
 */
struct amp_snap_t {
	struct amp_time_t time;

	unsigned int len;
	struct amp_snap_inst_t *inst;
};

/**
 * Snap instance structure.
 *   @id: The identifier.
 *   @prev, cur: The previous and current value.
 */
struct amp_snap_inst_t {
	struct amp_id_t id;
	uint16_t prev, cur;
};


/*
 * local declarations
 */
//static bool make_time(struct amp_time_t *time, struct ml_value_t *value);

/*
 * global variables
 */
const struct amp_seq_i amp_snap_iface = {
	(amp_info_f)amp_snap_info,
	(amp_seq_f)amp_snap_proc,
	(amp_copy_f)amp_snap_copy,
	(amp_delete_f)amp_snap_delete
};


/**
 * Create a new snap.
 *   @time: The snap time.
 *   &returns: The snap.
 */
struct amp_snap_t *amp_snap_new(struct amp_time_t time)
{
	struct amp_snap_t *snap;

	snap = malloc(sizeof(struct amp_snap_t));
	snap->time = time;
	snap->len = 0;
	snap->inst = malloc(0);

	return snap;
}

/**
 * Copy a snap.
 *   @snap: The original snap.
 *   &returns: The copied snap.
 */
struct amp_snap_t *amp_snap_copy(struct amp_snap_t *snap)
{
	struct amp_snap_t *copy;

	copy = malloc(sizeof(struct amp_snap_t));
	copy->len = snap->len;
	copy->inst = malloc(copy->len * sizeof(struct amp_snap_inst_t));
	memcpy(copy->inst, snap->inst, copy->len * sizeof(struct amp_snap_inst_t));

	return copy;
}

/**
 * Delete a snap.
 *   @snap: The snap.
 */
void amp_snap_delete(struct amp_snap_t *snap)
{
	free(snap->inst);
	free(snap);
}


/**
 * Create a snap from a value.
 *   @ret: Ref. The returned value.
 *   @value: The value.
 *   @env: The environment.
 *   &returns: Error.
 */
char *amp_snap_make(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env)
{
	/*
#define onexit amp_snap_delete(snap);
#define errstr "Type error. Snap expects type '((float,int,float),[(int,int)])'."
	struct amp_snap_t *snap;
	struct ml_link_t *link;
	struct ml_tuple_t tuple;
	struct amp_time_t time;

	if(value->type != ml_value_tuple_e)
		return mprintf(errstr);

	tuple = value->data.tuple;
	if(tuple.len != 2)
		return mprintf(errstr);

	if(!make_time(&time, tuple.value[0]))
		return mprintf(errstr);

	snap = amp_snap_new(time);

	if(tuple.value[1]->type != ml_value_list_e)
		fail(errstr);

	for(link = tuple.value[1]->data.list.head; link != NULL; link = link->next) {
		struct amp_id_t id;

		chkfail(amp_match_unpack(ml_value_copy(link->value), "(d,d)", &id.dev, &id.key));
		amp_snap_add(snap, id);
	}

	ml_value_delete(value);
	*ret = amp_pack_seq((struct amp_seq_t){ snap, &amp_snap_iface });
	return NULL;
#undef onexit
*/
	fatal("stub");
}
/*
static bool make_time(struct amp_time_t *time, struct ml_value_t *value)
{
	struct ml_tuple_t tuple;

	if(value->type != ml_value_tuple_v)
		return false;

	tuple = value->data.tuple;
	if(tuple.len != 3)
		return false;

	if((tuple.value[0]->type != ml_value_num_e) || (tuple.value[1]->type != ml_value_num_e) || (tuple.value[2]->type != ml_value_num_e))
		return false;

	*time = (struct amp_time_t){ tuple.value[0]->data.num, tuple.value[1]->data.num, tuple.value[2]->data.num };

	return true;
}
*/


/**
 * Add an identifier to the snap.
 *   @snap: The snap.
 *   @id: The identifier.
 */
void amp_snap_add(struct amp_snap_t *snap, struct amp_id_t id)
{
	snap->inst = realloc(snap->inst, (snap->len + 1) * sizeof(struct amp_snap_inst_t));
	snap->inst[snap->len++] = (struct amp_snap_inst_t){ id, 0, 0 };
}

/**
 * Process information on a snap.
 *   @snap: The snap.
 *   @queue: The queue.
 *   @time: The time.
 *   @len: The length.
 */
void amp_snap_info(struct amp_snap_t *snap, struct amp_info_t info)
{
	if(info.type == amp_info_action_e) {
		/*
		unsigned int i;
		struct amp_event_t *event = &info.data.action->event;

		if(event->val > 0) {
			for(i = 0; i < snap->len; i++) {
				if((event->dev == snap->inst[i].id.dev) &&(event->key == snasnapt[i].id.key))
					break;
			}

			if(i < snap->len) {
				snap->inst[i].state = !snap->inst[i].state;
				if(!snap->inst[i].state)
					event->val = 0;
			}
		}
		else
			*event = (struct amp_event_t){ 0, 0, 0 };
			*/
	}
}

/**
 * Process a snap.
 *   @snap: The snap.
 *   @time: The time.
 *   @len: The length.
 *   @queue: The action queue.
 */

struct amp_snap_inst_t *contains(struct amp_snap_t *snap, struct amp_event_t *event)
{
	unsigned int i;

	for(i = 0; i < snap->len; i++) {
		if((snap->inst[i].id.dev == event->dev) && (snap->inst[i].id.key == event->key))
			return &snap->inst[i];
	}

	return false;
}
void amp_snap_proc(struct amp_snap_t *snap, struct amp_time_t *time, unsigned int len, struct amp_queue_t *queue)
{
	unsigned int i, n = 0;
	struct amp_time_t left, right;
	struct amp_action_t *action;
	struct amp_snap_inst_t *inst;

	left = time[0];
	for(i = 0; i < len; i++) {
		right = time[i+1];

		while((action = amp_queue_get(queue, n, i)) != NULL) {
			if((inst = contains(snap, &action->event)) != NULL) {
				inst->cur = action->event.val;
				amp_queue_remove(queue, n);
			}
			else
				n++;
		}

		if(amp_time_between(snap->time, left, right)) {
			unsigned int j;

			for(j = 0; j < snap->len; j++) {
				struct amp_snap_inst_t *inst = &snap->inst[i];

				if(inst->prev != inst->cur)
					;
			}

			printf("beep\n");
		}

		left = right;
	}
}
