#include "../common.h"


/**
 * Schedule structure.
 *   @root: Instance root.
 *   @head, tail: The head and tail instances.
 */
struct amp_sched_t {
	struct avltree_root_t root;
	struct amp_sched_inst_t *head, *tail;
};

/**
 * Instance structure.
 *   @time: The time.
 *   @len: The event list length.
 *   @event: The event array.
 *   @node: Tree node.
 *   @prev, next: The previous and next instances.
 */
struct amp_sched_inst_t {
	struct amp_time_t time;

	unsigned int len;
	struct amp_event_t *event;

	struct avltree_node_t node;
	struct amp_sched_inst_t *prev, *next;
};

/*
 * global variables
 */
const struct amp_seq_i amp_sched_iface = {
	(amp_info_f)amp_sched_info,
	(amp_seq_f)amp_sched_proc,
	(amp_copy_f)amp_sched_copy,
	(amp_delete_f)amp_sched_delete
};


/*
 * local declarations
 */
static struct amp_sched_inst_t *inst_get(struct avltree_node_t *node);
static struct amp_sched_inst_t *inst_copy(struct amp_sched_inst_t *inst);
static void inst_delete(struct amp_sched_inst_t *inst);


/**
 * Create a schedule from a value.
 *   @ret: Ref. The returned value.
 *   @value: The value.
 *   @env: The environment.
 *   &returns: Error.
 */
char *amp_sched_make(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env)
{
#define onexit amp_sched_delete(sched);
#define error() fail("%C: Type error. Expected list of events.", ml_tag_chunk(&value->tag))
	struct ml_link_t *link;
	struct amp_sched_t *sched;

	sched = amp_sched_new();

	if(value->type != ml_value_list_v)
		error();

	for(link = value->data.list->head; link != NULL; link = link->next) {
		int bar;
		struct amp_time_t time;
		struct amp_event_t event;

		chkfail(amp_match_unpack(link->value, "((d,f),(d,d),d)", &bar, &time.beat, &event.dev, &event.key, &event.val));

		time.bar = bar;
		amp_sched_add(sched, time, event);
	}

	*ret = amp_pack_seq((struct amp_seq_t){ sched, &amp_sched_iface });

	return NULL;
#undef onexit
}


/**
 * Create a new schedule.
 *   &returns: The schedule.
 */
struct amp_sched_t *amp_sched_new(void)
{
	struct amp_sched_t *sched;

	sched = malloc(sizeof(struct amp_sched_t));
	sched->head = sched->tail = NULL;
	sched->root = avltree_root_init(amp_time_compare);

	return sched;
}

/**
 * Copy a schedule.
 *   @schedule: The original schedule.
 *   &returns: The copied schedule.
 */
struct amp_sched_t *amp_sched_copy(struct amp_sched_t *sched)
{
	struct amp_sched_t *copy;
	struct avltree_node_t *node;

	copy = amp_sched_new();

	for(node = avltree_root_first(&sched->root); node != NULL; node = avltree_node_next(node))
		avltree_root_insert(&copy->root, &inst_copy(inst_get(node))->node);

	return copy;
}

/**
 * Delete a schedule.
 *   @sched: The schedule.
 */
void amp_sched_delete(struct amp_sched_t *sched)
{
	avltree_root_destroy(&sched->root, offsetof(struct amp_sched_inst_t, node), (delete_f)inst_delete);
	free(sched);
}


/**
 * Add an event to the schedule.
 *   @sched: The schedule.
 *   @time: The time.
 *   @event: The event.
 */
void amp_sched_add(struct amp_sched_t *sched, struct amp_time_t time, struct amp_event_t event)
{
	struct avltree_node_t *node;
	struct amp_sched_inst_t *inst;

	node = avltree_root_lookup(&sched->root, &time);
	if(node == NULL) {
		inst = malloc(sizeof(struct amp_sched_inst_t));
		inst->time = time;
		inst->event = malloc(0);
		inst->len = 0;
		inst->node.ref = &inst->time;

		avltree_root_insert(&sched->root, &inst->node);
	}
	else
		inst = getparent(node, struct amp_sched_inst_t, node);

	inst->event = realloc(inst->event, (inst->len + 1) * sizeof(struct amp_event_t));
	inst->event[inst->len++] = event;
}

/**
 * Process information on a schedule.
 *   @sched: The schedule.
 *   @queue: The queue.
 *   @time: The time.
 *   @len: The length.
 */
void amp_sched_info(struct amp_sched_t *sched, struct amp_info_t info)
{
	if(info.type == amp_info_init_e) {
		struct avltree_node_t *node;

		for(node = avltree_root_first(&sched->root); node != NULL; node = avltree_node_next(node)) {
			inst_get(node)->prev = inst_get(avltree_node_prev(node) ?: avltree_root_last(&sched->root));
			inst_get(node)->next = inst_get(avltree_node_next(node) ?: avltree_root_first(&sched->root));
		}
	}
}

/**
 * Process a schedule.
 *   @sched: The schedule.
 *   @time: The time.
 *   @len: The length.
 *   @queue: The action queue.
 */
void amp_sched_proc(struct amp_sched_t *sched, struct amp_time_t *time, unsigned int len, struct amp_queue_t *queue)
{
	unsigned int i, j;
	struct amp_sched_inst_t *cur, *iter;

	if(amp_time_cmp(time[0], time[len]) == 0)
		return;

	cur = inst_get(avltree_root_atleast(&sched->root, &time[0]) ?: avltree_root_first(&sched->root));
	if(cur == NULL)
		return;

	for(i = 0; i < len; i++) {
		iter = cur;

		do {
			if(!amp_time_between(iter->time, time[i], time[i+1]))
				break;

			for(j = 0; j < iter->len; j++)
				amp_queue_add(queue, (struct amp_action_t){ i, iter->event[j], queue });
		} while((iter = iter->next) != cur);

		cur = iter;
	}

}

	
/**
 * Retrieve an instance from a node.
 *   @node: The node, may be null.
 *   &returns: The instance or null.
 */
static struct amp_sched_inst_t *inst_get(struct avltree_node_t *node)
{
	return node ? getparent(node, struct amp_sched_inst_t, node) : NULL;
}

/**
 * Copy an instance.
 *   @inst: The original instance.
 *   &returns: The copied instance.
 */
static struct amp_sched_inst_t *inst_copy(struct amp_sched_inst_t *inst)
{
	struct amp_sched_inst_t *copy;

	copy = malloc(sizeof(struct amp_sched_inst_t));
	copy->time = inst->time;
	copy->len = inst->len;
	copy->node.ref = &copy->time;
	copy->event = malloc(inst->len * sizeof(struct amp_event_t));
	memcpy(copy->event, inst->event, inst->len * sizeof(struct amp_event_t));

	return copy;
}

/**
 * Delete an instance.
 *   @inst: The instance.
 */
static void inst_delete(struct amp_sched_inst_t *inst)
{
	free(inst->event);
	free(inst);
}
