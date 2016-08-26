#include "common.h"


/**
 * Player configuration structure.
 *   @dev: The device.
 *   @nbeats: The number of beats.
 *   @ndivs, nbars: The number of divisoins and bars.
 */
struct web_player_conf_t {
	uint16_t dev;
	double nbeats;
	unsigned int ndivs, nbars;
};

/**
 * Player instance structure.
 *   @key, vel: The key and velocity.
 *   @begin, end: The begin and end times.
 *   @left, right; The left and right chain.
 */
struct web_player_inst_t {
	uint16_t key, vel;
	struct amp_loc_t begin, end;

	struct web_player_inst_t *left, *right;
};

/**
 * Player structure.
 *   @id: The identifier.
 *   @left, right, inst: The current instances and instance list.
 *   @conf: The configuration.
 */
struct web_player_t {
	const char *id;
	struct web_player_inst_t *left, *right, *inst;

	struct web_player_conf_t conf;
};


/*
 * local declarations
 */
static void player_proc(struct io_file_t file, void *arg);


/**
 * Create a new player.
 *   @id: The indetifier.
 *   &returns: The player.
 */
struct web_player_t *web_player_new(const char *id)
{
	struct web_player_t *player;

	player = malloc(sizeof(struct web_player_t));
	player->id = id;
	player->left = player->right = player->inst = NULL;
	web_player_conf_init(&player->conf);

	return player;
}

/**
 * Create a player from a value.
 *   @ret: Ref. The returned value.
 *   @value: The value.
 *   @env: The environment.
 *   &returns: Error.
 */
char *web_player_make(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env)
{
	if(value->type != ml_value_str_v)
		return mprintf("WebPlayer takes a string as input.");

	*ret = amp_pack_seq(amp_seq(web_serv_player(web_serv, value->data.str), &web_iface_seq));
	return NULL;
}

/**
 * Delete a player.
 *   @player: The player.
 */
void web_player_delete(struct web_player_t *player)
{
	struct web_player_inst_t *cur, *next;

	for(cur = player->left; cur != NULL; cur = next) {
		next = cur->left;
		free(cur);
	}

	web_player_conf_destroy(&player->conf);
	free(player);
}


/**
 * Add a note to a player.
 *   @player: The player.
 *   @begin: The begin time.
 *   @end: The end time.
 *   @key: The key.
 *   @vel: The velocity..
 */
void web_player_add(struct web_player_t *player, struct amp_loc_t begin, struct amp_loc_t end, uint16_t key, uint16_t vel)
{
	struct web_player_inst_t *inst, **ref;

	inst = malloc(sizeof(struct web_player_inst_t));
	inst->begin = begin;
	inst->end = end;
	inst->key = key;
	inst->vel = vel;

	for(ref = &player->left; *ref != NULL; ref = &(*ref)->left) {
		if(amp_loc_cmp((*ref)->begin, begin) >= 0)
			break;
	}
	*ref = inst;

	for(ref = &player->right; *ref != NULL; ref = &(*ref)->right) {
		if(amp_loc_cmp((*ref)->end, end) >= 0)
			break;
	}
	*ref = inst;
}

/**
 * Remove a note from a player.
 *   @player: The player.
 *   @begin: The begin time.
 *   @key: The key.
 */
void web_player_remove(struct web_player_t *player, struct amp_time_t begin, uint16_t key)
{
}


/**
 * Process information on a player.
 *   @player: The player.
 *   @info: The information.
 */
void web_player_info(struct web_player_t *player, struct amp_info_t info)
{
}

/**
 * Process data on a player.
 *   @player: The player.
 *   @time: The time.
 *   @len: The length.
 *   @queue: The action queue.
 *   &returns: The continuation flag.
 */
bool web_player_proc(struct web_player_t *player, struct amp_time_t *time, unsigned int len, struct amp_queue_t *queue)
{
	return false;
}


/**
 * Retrieve the information from a player.
 *   @player: The player.
 *   @file: The file.
 */
void web_player_print(struct web_player_t *player, struct io_file_t file)
{
	hprintf(file, "{\"conf\":{\"dev\":%u,\"ndivs\":%u,\"nbeats\":%.3f,\"nbars\":%u},\"data\":[", player->conf.dev, player->conf.ndivs, player->conf.nbeats, player->conf.nbars);
	hprintf(file, "]}");
}

/**
 * Create a chunk for the player.
 *   @player: The player.
 *   &returns: The chunk.
 */
struct io_chunk_t web_player_chunk(struct web_player_t *player)
{
	return (struct io_chunk_t){ player_proc, player };
}
static void player_proc(struct io_file_t file, void *arg)
{
	web_player_print(arg, file);
}


/**
 * Initialize a player configuration.
 *   @conf: The configuration.
 */
void web_player_conf_init(struct web_player_conf_t *conf)
{
	conf->dev = 0;
	conf->ndivs = 4;
	conf->nbeats = 4.0;
	conf->nbars = 200;
}

/**
 * Destroy a player configuration.
 *   @conf: The configuration.
 */
void web_player_conf_destroy(struct web_player_conf_t *conf)
{
}
