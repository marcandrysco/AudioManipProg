#include "common.h"


/**
 * Player configuration structure.
 *   @dev: The device.
 *   @nbeats: The number of beats.
 *   @ndivs, nbars: The number of divisoins and bars.
 *   @keys: Key array.
 *   @nkeys: The number of keys.
 */
struct web_player_conf_t {
	uint16_t dev;
	double nbeats;
	unsigned int ndivs, nbars;

	uint16_t *keys;
	unsigned int nkeys;
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
 *   @serv: The server.
 *   @id: The identifier.
 *   @last: The last location.
 *   @begin, end: The begin and end instance chains.
 *   @left, right: The current instances.
 *   @conf: The configuration.
 */
struct web_player_t {
	struct web_serv_t *serv;
	const char *id;

	struct amp_loc_t last;
	struct web_player_inst_t *begin, *end;
	struct web_player_inst_t *left, *right;

	struct web_player_conf_t conf;
};


/*
 * local declarations
 */
static char *req_keys(struct web_player_t *player, const char *body);

static void player_proc(struct io_file_t file, void *arg);


/**
 * Create a new player.
 *   @serv: The server.
 *   @id: The indetifier.
 *   &returns: The player.
 */
struct web_player_t *web_player_new(struct web_serv_t *serv, const char *id)
{
	struct web_player_t *player;

	player = malloc(sizeof(struct web_player_t));
	player->serv = serv;
	player->id = id;
	player->begin = player->end = NULL;
	player->left = player->right = NULL;
	player->last = amp_loc(0, 0.0);
	web_player_conf_init(&player->conf);
	web_player_load(player);

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

	for(cur = player->begin; cur != NULL; cur = next) {
		next = cur->left;
		free(cur);
	}

	web_player_conf_destroy(&player->conf);
	free(player);
}


/**
 * Lookup an instance thats at the given locations.
 *   @player: The player.
 *   @begin: The begin location.
 *   @end: The end location.
 *   @key: The key.
 */
struct web_player_inst_t *web_player_lookup(struct web_player_t *player, struct amp_loc_t begin, struct amp_loc_t end, uint16_t key)
{
	struct web_player_inst_t *inst;

	for(inst = player->begin; inst != NULL; inst = inst->left) {
		if(inst->key != key)
			continue;

		if(amp_loc_near(inst->begin, begin) && amp_loc_near(inst->end, end))
			return inst;
	}

	return NULL;
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

	for(ref = &player->begin; *ref != NULL; ref = &(*ref)->left) {
		if(amp_loc_cmp((*ref)->begin, begin) > 0)
			break;
	}

	if(amp_loc_cmp(begin, player->last) >= 0) {
		if((player->left == NULL) || (amp_loc_cmp(begin, player->left->begin) < 0))
			player->left = inst;
	}

	inst->left = *ref;
	*ref = inst;

	for(ref = &player->end; *ref != NULL; ref = &(*ref)->right) {
		if(amp_loc_cmp((*ref)->end, end) > 0)
			break;
	}

	if(amp_loc_cmp(begin, player->last) >= 0) {
		if((player->right == NULL) || (amp_loc_cmp(end, player->right->end) < 0))
			player->right = inst;
	}

	inst->right = *ref;
	*ref = inst;
}

/**
 * Remove a note from a player.
 *   @player: The player.
 *   @begin: The begin time.
 *   @key: The key.
 */
void web_player_remove(struct web_player_t *player, struct amp_loc_t begin, struct amp_loc_t end, uint16_t key)
{
	struct web_player_inst_t *inst, **ref;

	inst = web_player_lookup(player, begin, end, key);
	if(inst == NULL)
		return;

	for(ref = &player->begin; *ref != NULL; ref = &(*ref)->left) {
		if(*ref == inst)
			break;
	}
	if(player->left == *ref)
		player->left = (*ref)->left;

	*ref = (*ref)->left;

	for(ref = &player->end; *ref != NULL; ref = &(*ref)->right) {
		if(*ref == inst)
			break;
	}
	if(player->right == *ref)
		player->right = (*ref)->right;

	*ref = (*ref)->right;

	free(inst);
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
	unsigned int i;
	struct web_player_inst_t *left, *right;

	left = player->left;
	right = player->right;

	if(amp_time_isequal(time[0], time[len]))
		return false;

	for(i = 0; i < len; i++) {
		while(left != NULL) {
			if((int)time[i].bar < left->begin.bar)
				break;
			else if(((int)time[i].bar == left->begin.bar) && (time[i].beat < left->begin.beat))
				break;

			//printf("begin! %d .. %d:%.3f %d:%.3f\n", left->key, left->begin.bar, left->begin.beat, (int)time[i].bar, time[i].beat);

			amp_queue_add(queue, (struct amp_action_t){ i, { player->conf.dev, left->key, left->vel }, queue });
			left = left->left;
		}

		while(right != NULL) {
			if((int)time[i].bar < right->end.bar)
				break;
			else if(((int)time[i].bar == right->end.bar) && (time[i].beat < right->end.beat))
				break;

			//printf("end!   %d .. %d:%.3f %d:%.3f\n", right->key, right->end.bar, right->end.beat, (int)time[i].bar, time[i].beat);
			amp_queue_add(queue, (struct amp_action_t){ i, { player->conf.dev, right->key, 0 }, queue });
			right = right->right;
		}
	}

	player->left = left;
	player->right = right;
	player->last = amp_loc(time[len].bar, time[len].beat);

	return false;
}


/**
 * Retrieve the information from a player.
 *   @player: The player.
 *   @file: The file.
 */
void web_player_print(struct web_player_t *player, struct io_file_t file)
{
	unsigned int i;
	struct web_player_inst_t *inst;

	hprintf(file, "{\"conf\":{\"dev\":%u,\"ndivs\":%u,\"nbeats\":%.3f,\"nbars\":%u,\"keys\":[", player->conf.dev, player->conf.ndivs, player->conf.nbeats, player->conf.nbars);

	for(i = 0; i < player->conf.nkeys; i++)
		hprintf(file, "%s%d", i ? "," : "", player->conf.keys[i]);

	hprintf(file, "]},\"data\":[");

	for(inst = player->begin; inst != NULL; inst = inst->left) {
		if(inst != player->begin)
			hprintf(file, ",");

		hprintf(file, "{\"key\":%u,\"begin\":{\"bar\":%d,\"beat\":%.8f},\"end\":{\"bar\":%d,\"beat\":%.8f},\"vel\":%u}", inst->key, inst->begin.bar, inst->begin.beat, inst->end.bar, inst->end.beat, inst->vel);
	}

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
 * Handle a player requeust.
 *   @player: The player.
 *   @path: The path.
 *   @args: The arguments.
 *   &returns: True if handled.
 */
bool web_player_req(struct web_player_t *player, const char *path, struct http_args_t *args)
{
	unsigned int key, vel, n;
	struct amp_loc_t begin, end;

	if(sscanf(path, "/set/%u/%u:%lf/%u:%lf/%u%n", &key, &begin.bar, &begin.beat, &end.bar, &end.beat, &vel, &n) >= 6) {
		if(vel > UINT16_MAX)
			return false;

		sys_mutex_lock(&player->serv->lock);
		web_player_remove(player, begin, end, key);

		if(vel > 0)
			web_player_add(player, begin, end, key, vel);

		sys_mutex_unlock(&player->serv->lock);
		web_player_save(player);

		return true;
	}
	else if(strcmp(path, "/keys") == 0)
		return chkwarn(req_keys(player, args->body));
	else
		return false;
}

/**
 * Handle a request to modify the keys.
 *   @player: The player.
 *   @body: The document body.
 *   &returns: Error.
 */
static char *req_keys(struct web_player_t *player, const char *body)
{
#define onexit json_erase(json);
	int val;
	unsigned int i;
	struct json_t *json = NULL;
	struct json_arr_t *arr;

	chkfail(json_parse_str(&json, body));

	if(json->type != json_arr_v)
			fail("Invalid keys request. Expected array of keys.");

	arr = json->data.arr;
	for(i = 0; i < arr->len; i++) {
		if(!json_int_range(arr->vec[i], 0, UINT16_MAX, NULL))
			fail("Invalid keys request. Expected array of keys.");
	}

	player->conf.keys = realloc(player->conf.keys, arr->len * sizeof(uint16_t));
	for(i = 0; i < arr->len; i++) {
		json_int_get(arr->vec[i], &val);
		player->conf.keys[i] = val;
	}

	web_player_save(player);

	json_delete(json);
	return NULL;
#undef onexit
}

/**
 * Load a player.
 *   @player: The player.
 *   &returns: Error.
 */
char *web_player_load(struct web_player_t *player)
{
#define onexit fclose(file); cfg_line_erase(line);
	FILE *file;
	struct cfg_line_t *line;
	unsigned int lineno = 1;
	char path[strlen(player->id) + 9];

	sprintf(path, "web.dat/%s", player->id);

	file = fopen(path, "r");
	if(file == NULL)
		return mprintf("Cannot read from '%s'.", path);

	while(true) {
		chkfail(cfg_line_parse(&line, file, &lineno));
		if(line == NULL)
			break;

		if(strcmp(line->key, "Keys") == 0) {
			uint16_t *keys;
			unsigned int nkeys;

			chkfail(cfg_readf(line, "u16* $", &keys, &nkeys));

			free(player->conf.keys);
			player->conf.keys = keys;
			player->conf.nkeys = nkeys;
		}
		else if(strcmp(line->key, "Event") == 0) {
			struct amp_loc_t begin, end;
			uint16_t key, vel;

			chkfail(cfg_readf(line, "df df u16 u16 $", &begin.bar, &begin.beat, &end.bar, &end.beat, &key, &vel));

			web_player_add(player, begin, end, key, vel);
		}
		else
			fail("Unknown directive '%s'.", line->key);

		cfg_line_delete(line);
	}

	fclose(file);
	return NULL;
}

/**
 * Save a player.
 *   @player: The player.
 */
void web_player_save(struct web_player_t *player)
{
	FILE *file;
	struct web_player_inst_t *inst;
	char path[strlen(player->id) + 9];

	sprintf(path, "web.dat/%s", player->id);

	fs_mkdir("web.dat", 0775);
	file = fopen(path, "w");
	if(file == NULL)
		fatal("Failed to save to path '%s'", path);

	cfg_writef(file, "Keys", "u16*", player->conf.keys, player->conf.nkeys);

	for(inst = player->begin; inst != NULL; inst = inst->left)
		cfg_writef(file, "Event", "df df u16 u16", inst->begin.bar, inst->begin.beat, inst->end.bar, inst->end.beat, inst->key, inst->vel);

	fclose(file);
}


/**
 * Initialize a player configuration.
 *   @conf: The configuration.
 */
void web_player_conf_init(struct web_player_conf_t *conf)
{
	unsigned int i;

	conf->dev = 0;
	conf->ndivs = 4;
	conf->nbeats = 4.0;
	conf->nbars = 200;
	conf->keys = malloc((conf->nkeys = 37) * sizeof(uint16_t));

	for(i = 0; i < conf->nkeys; i++)
		conf->keys[i] = i + 12;
}

/**
 * Destroy a player configuration.
 *   @conf: The configuration.
 */
void web_player_conf_destroy(struct web_player_conf_t *conf)
{
	free(conf->keys);
}
