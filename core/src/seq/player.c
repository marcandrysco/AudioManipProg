#include "../common.h"

/**
 * Active note structure.
 *   @dev, key: The device and key.
 *   @rem: The remaining length of time.
 */
struct amp_player_active_t {
	uint16_t dev, key;
	double rem;
};

/**
 * Player structure.
 *   @cur: The current instance.
 *   @inst: The instance root.
 */
struct amp_player_t {
	struct amp_player_inst_t *cur;
	struct avltree_root_t inst;
};

/**
 * Instance structure.
 *   @len: The length.
 *   @time: The time.
 *   @event: The event.
 *   @node: The tree node.
 *   @prev, next: The previous and next instances.
 */
struct amp_player_inst_t {
	double len;
	struct amp_time_t time;
	struct amp_event_t event;

	struct avltree_node_t node;
	struct amp_player_inst_t *prev, *next;
};


/*
 * global variables
 */
const struct amp_seq_i amp_player_iface = {
	(amp_info_f)amp_player_info,
	(amp_seq_f)amp_player_proc,
	(amp_copy_f)amp_player_copy,
	(amp_delete_f)amp_player_delete
};


/**
 * Create a new player.
 *   &returns: The player.
 */
struct amp_player_t *amp_player_new(void)
{
	struct amp_player_t *player;

	player = malloc(sizeof(struct amp_player_t));
	player->cur = NULL;
	player->inst = avltree_root_init(amp_time_compare);

	return player;
}

/**
 * Copy a player.
 *   @player: The original player.
 *   &returns: The copied player.
 */
struct amp_player_t *amp_player_copy(struct amp_player_t *player)
{
	struct amp_player_t *copy;
	struct amp_player_inst_t *inst;

	copy = amp_player_new();

	for(inst = amp_player_first(player); inst != NULL; inst = amp_player_next(inst))
		amp_player_add(copy, inst->time, inst->len, inst->event);

	return copy;
}

/**
 * Delete a player.
 *   @player: The player.
 */
void amp_player_delete(struct amp_player_t *player)
{
	avltree_root_destroy(&player->inst, offsetof(struct amp_player_inst_t, node), free);
	free(player);
}


/**
 * Create a schedule from a value.
 *   @ret: Ref. The return value.
 *   @value: The value.
 *   @env: The environment.
 *   &returns: Error.
 */
char *amp_player_make(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env)
{
#define onexit amp_player_delete(player); ml_value_delete(value);
	struct amp_player_t *player;
	struct ml_link_t *link;

	player = amp_player_new();

	if(value->type != ml_value_list_e)
		fail("Type mismatch. Expected '[((Int,Float),Float,(Int,Int,Int))]'.");

	for(link = value->data.list.head; link != NULL; link = link->next) {
		double len;
		struct amp_time_t time;
		struct amp_event_t event;

		chkfail(amp_match_unpack(ml_value_copy(link->value), "((d,f),f,(d,d,d))", &time.bar, &time.beat, &len, &event.dev, &event.key, &event.val));

		amp_player_add(player, time, len, event);
	}

	*ret = amp_pack_seq((struct amp_seq_t){ player, &amp_player_iface });
	ml_value_delete(value);

	return NULL;
}
struct ml_value_t *amp_player_make0(struct ml_value_t *value, struct ml_env_t *env, char **err)
{
	struct ml_value_t *ret = NULL;
	*err = amp_player_make(&ret, value, env);
	return ret;
}


/**
 * Add an event to the player.
 *   @player: The player.
 *   @time: The time.
 *   @len: The length.
 *   @event: The event.
 */
void amp_player_add(struct amp_player_t *player, struct amp_time_t time, double len, struct amp_event_t event)
{
	struct avltree_node_t *node;
	struct amp_player_inst_t *inst, *adj;

	inst = malloc(sizeof(struct amp_player_inst_t));
	inst->time = time;
	inst->len = len;
	inst->event = event;
	inst->node.ref = &inst->time;

	avltree_root_insert(&player->inst, &inst->node);

	node = avltree_node_prev(&inst->node) ?: avltree_root_last(&player->inst);
	adj = getparent(node, struct amp_player_inst_t, node);
	adj->next = inst;
	inst->prev = adj;

	node = avltree_node_next(&inst->node) ?: avltree_root_first(&player->inst);
	adj = getparent(node, struct amp_player_inst_t, node);
	adj->prev = inst;
	inst->next = adj;
}

/**
 * Process information on a player.
 *   @player: The player.
 *   @queue: The queue.
 *   @time: The time.
 *   @len: The length.
 */
void amp_player_info(struct amp_player_t *player, struct amp_info_t info)
{
	switch(info.type) {
	case amp_info_start_e:
		player->cur = amp_player_atleast(player, info.data.seek->time);
		break;
	case amp_info_stop_e:
		break;

	default:
		break;
	}
}

/**
 * Process a player.
 *   @player: The player.
 *   @queue: The queue.
 *   @time: The time.
 *   @len: The length.
 */
void amp_player_proc(struct amp_player_t *player, struct amp_queue_t *queue, struct amp_time_t *time, unsigned int len)
{
}


/**
 * Retrieve the first instance from the player.
 *   @player: The player.
 *   &returns: The instance or null.
 */
struct amp_player_inst_t *amp_player_first(struct amp_player_t *player)
{
	struct avltree_node_t *node;

	node = avltree_root_first(&player->inst);
	return node ? getparent(node, struct amp_player_inst_t, node) : NULL;
}

/**
 * Retrieve the next instance from the player.
 *   @inst: The current instance.
 *   &returns: The next instance or null.
 */
struct amp_player_inst_t *amp_player_next(struct amp_player_inst_t *inst)
{
	struct avltree_node_t *node;

	node = avltree_node_next(&inst->node);
	return node ? getparent(node, struct amp_player_inst_t, node) : NULL;
}

/**
 * Retrieve the instance from the player after a given time.
 *   @player: The player.
 *   @time: The time.
 *   &returns: The instance or null.
 */
struct amp_player_inst_t *amp_player_atleast(struct amp_player_t *player, struct amp_time_t time)
{
	struct avltree_node_t *node;

	node = avltree_root_atleast(&player->inst, &time);
	return node ? getparent(node, struct amp_player_inst_t, node) : NULL;
}
