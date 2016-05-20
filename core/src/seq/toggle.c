#include "../common.h"


/**
 * Toggle structure.
 *   @len: The instance array length.
 *   @inst: The instance array.
 */
struct amp_toggle_t {
	unsigned int len;
	struct amp_toggle_inst_t *inst;
};

/**
 * Toggle instance structure.
 *   @id: The identifier.
 *   @state: The key state.
 */
struct amp_toggle_inst_t {
	struct amp_id_t id;
	bool state;
};


/*
 * local declarations
 */
static struct amp_toggle_inst_t *toggle_match(struct amp_toggle_t *toggle, struct amp_event_t *event);

/*
 * global variables
 */
const struct amp_seq_i amp_toggle_iface = {
	(amp_info_f)amp_toggle_info,
	(amp_seq_f)amp_toggle_proc,
	(amp_copy_f)amp_toggle_copy,
	(amp_delete_f)amp_toggle_delete
};


/**
 * Create a new toggle.
 *   &returns: The toggle.
 */
struct amp_toggle_t *amp_toggle_new(void)
{
	struct amp_toggle_t *toggle;

	toggle = malloc(sizeof(struct amp_toggle_t));
	toggle->len = 0;
	toggle->inst = malloc(0);

	return toggle;
}

/**
 * Copy a toggle.
 *   @toggle: The original toggle.
 *   &returns: The copied toggle.
 */
struct amp_toggle_t *amp_toggle_copy(struct amp_toggle_t *toggle)
{
	struct amp_toggle_t *copy;

	copy = malloc(sizeof(struct amp_toggle_t));
	copy->len = toggle->len;
	copy->inst = malloc(copy->len * sizeof(struct amp_toggle_inst_t));
	memcpy(copy->inst, toggle->inst, copy->len * sizeof(struct amp_toggle_inst_t));

	return copy;
}

/**
 * Delete a toggle.
 *   @toggle: The toggle.
 */
void amp_toggle_delete(struct amp_toggle_t *toggle)
{
	free(toggle->inst);
	free(toggle);
}


/**
 * Create a toggle from a value.
 *   @ret: Ref. The returned value.
 *   @value: The value.
 *   @env: The environment.
 *   &returns: Error.
 */
char *amp_toggle_make(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env)
{
#define onexit amp_toggle_delete(toggle);
	struct amp_toggle_t *toggle;
	struct ml_link_t *link;

	toggle = amp_toggle_new();

	if(value->type != ml_value_list_v)
		fail("Type error. Toggle expects type '[(int,int)]'.");

	for(link = value->data.list->head; link != NULL; link = link->next) {
		struct amp_id_t id;

		chkfail(amp_match_unpack(ml_value_copy(link->value), "(d,d)", &id.dev, &id.key));
		amp_toggle_add(toggle, id);
	}

	ml_value_delete(value);
	*ret = amp_pack_seq((struct amp_seq_t){ toggle, &amp_toggle_iface });
	return NULL;
#undef onexit
}


/**
 * Add an identifier to the toggle.
 *   @toggle: The toggle.
 *   @id: The identifier.
 */
void amp_toggle_add(struct amp_toggle_t *toggle, struct amp_id_t id)
{
	toggle->inst = realloc(toggle->inst, (toggle->len + 1) * sizeof(struct amp_toggle_inst_t));
	toggle->inst[toggle->len++] = (struct amp_toggle_inst_t){ id, false };
}

/**
 * Process information on a toggle.
 *   @toggle: The toggle.
 *   @queue: The queue.
 *   @time: The time.
 *   @len: The length.
 */
void amp_toggle_info(struct amp_toggle_t *toggle, struct amp_info_t info)
{
}

/**
 * Process a toggle.
 *   @toggle: The toggle.
 *   @time: The time.
 *   @len: The length.
 *   @queue: The action queue.
 */
void amp_toggle_proc(struct amp_toggle_t *toggle, struct amp_time_t *time, unsigned int len, struct amp_queue_t *queue)
{
	unsigned int i = 0;

	while(i < queue->idx) {
		struct amp_toggle_inst_t *inst;
		struct amp_event_t *event = &queue->arr[i].event;

		inst = toggle_match(toggle, event);
		if(inst != NULL) {
			if(event->val > 0) {
				inst->state = !inst->state;
				if(!inst->state)
					event->val = 0;

				i++;
			}
			else
				amp_queue_remove(queue, i);
		}
		else
			i++;
	}
}
static struct amp_toggle_inst_t *toggle_match(struct amp_toggle_t *toggle, struct amp_event_t *event)
{
	unsigned int i;

	for(i = 0; i < toggle->len; i++) {
		if((event->dev == toggle->inst[i].id.dev) && (event->key == toggle->inst[i].id.key))
			return &toggle->inst[i];
	}

	return NULL;
}
