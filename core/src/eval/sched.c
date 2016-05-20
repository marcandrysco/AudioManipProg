#include "../common.h"


/**
 * Key structure.
 *   @dev, key: The device and key.
 *   @inst: The instance tree root.
 *   @node: The node.
 */
struct key_t {
	uint16_t id[2];
	struct avltree_root_t inst;

	struct avltree_node_t node;
};

/**
 * Node structure.
 *   @time: The time.
 *   @event: The event.
 *   @node: The AVL tree node.
 */
struct inst_t {
	struct amp_time_t time;
	struct amp_event_t event;

	struct avltree_node_t node;
};


/*
 * local declarations
 */
/*
static struct key_t *key_create(struct avltree_root_t *root, uint16_t dev, uint16_t keyid);
static void key_delete(struct key_t *key);
static int key_compare(const void *left, const void *right);

static struct inst_t *inst_atmost(struct avltree_root_t *root, struct amp_time_t time);
static struct inst_t *inst_first(struct avltree_root_t *root);
static struct inst_t *inst_next(struct inst_t *inst);
bool inst_near(struct inst_t *inst, struct amp_time_t time);
struct inst_t *inst_add(struct avltree_root_t *root, struct amp_time_t time, struct amp_event_t event);
*/


struct ml_value_t *amp_eval_skyline0(struct ml_value_t *value, struct ml_env_t *env, char **err)
{
	struct ml_value_t *res;

	*err = amp_eval_skyline(&res, value, env);

	return res;
}

char *amp_eval_skyline(struct ml_value_t **result, struct ml_value_t *value, struct ml_env_t *env)
{
#if 0
#define onexit ml_value_delete(value); avltree_root_destroy(&keys, offsetof(struct key_t, node), (delete_f)key_delete);
	struct ml_link_t *link;
	struct avltree_root_t keys;

	*result = NULL;
	keys = avltree_root_init(key_compare);

	if(value->type != ml_value_list_e)
		fail("Expected list.");

	for(link = value->data.list.head; link != NULL; link = link->next) {
		uint16_t prev;
		//struct node_t *node;
		double len;
		struct key_t *key;
		struct inst_t *inst, *head, *tail;
		struct amp_time_t time;
		struct amp_event_t event;

		if(link->value->type != ml_value_tuple_e)
			fail("Expected list of events '[((Num,Num),Num,(Num,Num,Num))]'.");

		chkfail(amp_match_unpack(ml_value_copy(link->value), "((d,f),f,(d,d,d)))", &time.bar, &time.beat, &len, &event.dev, &event.key, &event.val));

		if(event.val == 0)
			continue;

		key = key_create(&keys, event.dev, event.key);

		/* trailing edge */
		tail = inst_atmost(&key->inst, time);
		if(tail != NULL) {
			if(tail->event.val < event.val) {
				if(!inst_near(tail, time))
					tail = inst_add(&key->inst, time, (struct amp_event_t){ event.dev, event.key, tail->event.val });
			}
		}
		else
			tail = inst_add(&key->inst, time, (struct amp_event_t){ event.dev, event.key, 0 });

		/* leading edge */
		head = inst_atmost(&key->inst, time);
		if(head != NULL) {
			printf("nonnull\n");
			if(head->event.val < event.val) {
				if(inst_near(head, time))
					head->event.val = event.val;
				else
					head = inst_add(&key->inst, time, event);
			}
		}
		else
			head = inst_add(&key->inst, time, event);

		/* midpoints */
		for(prev = head->event.val, inst = inst_next(head); inst != tail; prev = inst->event.val, inst = inst_next(inst)) {
			if(inst->event.val < event.val)
				inst->event.val = event.val;

			if(prev == inst->event.val)
				; // remove
		}
	}

	struct avltree_node_t *node;

	for(node = avltree_root_first(&keys); node != NULL; node = avltree_node_next(node)) {
		struct inst_t *inst;
		struct key_t *key = getparent(node, struct key_t, node);

		for(inst = inst_first(&key->inst); inst != NULL; inst = inst_next(inst)) {
			printf("%d.%.02f %d\n", inst->time.bar, inst->time.beat, inst->event.val);
		}
	}

	onexit;
	*result = ml_value_nil(value->tag);
	return NULL;
#undef onexit
#endif
	fatal("stbu");
}


#if 0
/**
 * Create a key.
 *   @root: The root.
 *   @dev: The device.
 *   @keyid: The key identifier.
 *   &returns: The key.
 */
static struct key_t *key_create(struct avltree_root_t *root, uint16_t dev, uint16_t keyid)
{
	struct key_t *key;
	struct avltree_node_t *node;
	uint16_t id[2] = { dev, keyid };

	node = avltree_root_lookup(root, id);
	if(node == NULL) {
		key = malloc(sizeof(struct key_t));
		key->id[0] = id[0];
		key->id[1] = id[1];
		key->inst = avltree_root_init(amp_time_compare);
		key->node.ref = key->id;

		avltree_root_insert(root, &key->node);
	}
	else
		key = getparent(node, struct key_t, node);

	return key;
}

/**
 * Delete a key.
 *   @key: The key.
 */
static void key_delete(struct key_t *key)
{
	avltree_root_destroy(&key->inst, offsetof(struct inst_t, node), free);
	free(key);
}

/**
 * Compare nodes.
 *   @left: The left node pointer.
 *   @right: The right node pointer.
 *   &returns: Their order.
 */
static int key_compare(const void *left, const void *right)
{
	const int *lval = left, *rval = right;

	if(lval[0] < rval[0])
		return -2;
	else if(lval[0] > rval[0])
		return 2;
	else if(lval[0] < rval[0])
		return -1;
	else if(lval[0] > rval[0])
		return 1;
	else
		return 0;
}


/**
 * Retrieve the earliest instance before a time.
 *   @root: The root.
 *   @time: The time.
 *   &returns: The instance.
 */
static struct inst_t *inst_atmost(struct avltree_root_t *root, struct amp_time_t time)
{
	struct avltree_node_t *node;

	node = avltree_root_atmost(root, &time);
	return node ? getparent(node, struct inst_t, node) : 0;
}

/**
 * Retrieve the first instance.
 *   @root: The root.
 *   &returns: The instance or null.
 */
static struct inst_t *inst_first(struct avltree_root_t *root)
{
	struct avltree_node_t *node;

	node = avltree_root_first(root);
	return node ? getparent(node, struct inst_t, node) : 0;
}

/**
 * Retrieve the next instance.
 *   @inst: The current instance.
 *   &returns: The next instance or null.
 */
static struct inst_t *inst_next(struct inst_t *inst)
{
	struct avltree_node_t *node;

	node = avltree_node_next(&inst->node);
	return node ? getparent(node, struct inst_t, node) : 0;
}

bool inst_near(struct inst_t *inst, struct amp_time_t time)
{
	if(inst == NULL)
		return false;

	return amp_time_cmp(inst->time, time) == 0;
}


struct inst_t *inst_add(struct avltree_root_t *root, struct amp_time_t time, struct amp_event_t event)
{
	struct inst_t *inst;
	
	inst = malloc(sizeof(struct inst_t));
	inst->time = time;
	inst->event = event;
	inst->node.ref = &inst->time;
	avltree_root_insert(root, &inst->node);

	return NULL;
}
#endif
