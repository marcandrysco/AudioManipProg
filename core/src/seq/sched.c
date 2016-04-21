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


bool amp_scan_time(struct amp_time_t *time, struct ml_value_t *value)
{
	struct ml_tuple_t tuple;

	if(value->type != ml_value_tuple_e)
		return false;

	tuple = value->data.tuple;
	if(tuple.len != 2)
		return false;

	if((tuple.value[0]->type != ml_value_num_e) || (tuple.value[1]->type != ml_value_num_e))
		return false;

	*time = (struct amp_time_t){ 0, tuple.value[0]->data.num, tuple.value[1]->data.num };

	return true;
}

bool amp_scan_event(struct amp_event_t *event, struct ml_value_t *value)
{
	struct ml_tuple_t tuple;

	if(value->type != ml_value_tuple_e)
		return false;

	tuple = value->data.tuple;
	if(tuple.len != 3)
		return false;

	if((tuple.value[0]->type != ml_value_num_e) || (tuple.value[1]->type != ml_value_num_e) || (tuple.value[2]->type != ml_value_num_e))
		return false;

	*event = (struct amp_event_t){ tuple.value[0]->data.num, tuple.value[1]->data.num, tuple.value[2]->data.num };

	return true;
}

/**
 * Create a schedule from a value.
 *   @value: The value.
 *   @env: The environment.
 *   @err: The rror.
 *   &returns: The value or null.
 */
struct ml_value_t *amp_sched_make(struct ml_value_t *value, struct ml_env_t *env, char **err)
{
#undef fail
#define fail() do { amp_sched_delete(sched); ml_value_delete(value); *err = amp_printf("Type error. Expected '[((Num,Num),(Num,Num,Num))]'."); return NULL; } while(0)

	struct ml_link_t *link;
	struct ml_tuple_t tuple;
	struct amp_sched_t *sched;
	struct amp_time_t time;
	struct amp_event_t event;

	sched = amp_sched_new();

	if(value->type != ml_value_list_e)
		fail();

	for(link = value->data.list.head; link != NULL; link = link->next) {
		if(link->value->type != ml_value_tuple_e)
			fail();

		tuple = link->value->data.tuple;
		if(tuple.len != 2)
			fail();

		if(!amp_scan_time(&time, tuple.value[0]) || !amp_scan_event(&event, tuple.value[1]))
			fail();

		amp_sched_add(sched, time, event);
	}

	ml_value_delete(value);

	return amp_pack_seq((struct amp_seq_t){ sched, &amp_sched_iface });
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
			//if(i == 0)
				//printf("hi? %d:%.2f %d:%.2f\n", time[0].bar, time[0].beat, iter->time.bar, iter->time.beat);
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

static void inst_delete(struct amp_sched_inst_t *inst)
{
	free(inst->event);
	free(inst);
}
