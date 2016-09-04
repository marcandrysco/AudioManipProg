#include "common.h"


/**
 * Server structure.
 *   @lock: The lock.
 *   @task: The http task.
 *   @inst: The instance tree.
 */
struct web_serv_t {
	struct sys_mutex_t lock;

	struct sys_task_t *task;
	struct avltree_root_t inst;
};

/**
 * Instance data union.
 *   @mach: Machine.
 *   @player: Player.
 */
union web_inst_u {
	struct web_mach_t *mach;
	struct web_player_t *player;
};

/**
 * Instance structure.
 *   @id: The identifier.
 *   @refcnt: The reference count.
 *   @serv: The server.
 *   @type: The type.
 *   @data: The data.
 *   @node: The tree node.
 */
struct web_inst_t {
	char *id;
	unsigned int refcnt;
	struct web_serv_t *serv;

	enum web_inst_e type;
	union web_inst_u data;

	struct avltree_node_t node;
};


/*
 * global variables
 */
const struct amp_seq_i web_iface_seq = {
	(amp_info_f)web_inst_info,
	(amp_seq_f)web_inst_seq,
	(amp_copy_f)web_inst_ref,
	(amp_delete_f)web_inst_unref
};

/*
 * local declarations
 */
static bool serv_handler(const char *path, struct http_args_t *args, void *arg);


/**
 * Create the server.
 *   &returns: The server.
 */
struct web_serv_t *web_serv_new(void)
{
	struct web_serv_t *serv;

	serv = malloc(sizeof(struct web_serv_t));
	serv->lock = sys_mutex_init(0);
	serv->inst = avltree_root_init(compare_str);
	serv->task = http_server_async(8080, serv_handler, serv);

	return serv;
}

/**
 * Delete the server.
 *   @serv: The server.
 */
void web_serv_delete(struct web_serv_t *serv)
{
	sys_task_delete(serv->task);
	sys_mutex_destroy(&serv->lock);
	free(serv);
}


/**
 * Handle a request on the server.
 *   @serv: The server.
 *   @path: The path.
 *   @args: The arguments.
 *   &returns: True if handled.
 */
bool web_serv_req(struct web_serv_t *serv, const char *path, struct http_args_t *args)
{
	char type[16];
	unsigned int n, idx;

	if(strcmp(path, "/all") == 0) {
		struct web_inst_t *inst;

		hprintf(args->file, "[");
		for(inst = web_inst_first(web_serv); inst != NULL; inst = web_inst_next(inst))
			hprintf(args->file, "{\"id\":\"%s\",\"type\":\"%s\",\"data\":%C}", inst->id, web_inst_type(inst->type), web_inst_chunk(inst));
		hprintf(args->file, "]");

		http_head_add(&args->resp, "Content-Type", "application/json");

		return true;
	}
	else if(sscanf(path, "/%u/%15[a-z]%n", &idx, type, &n) >= 2) {
		struct web_inst_t *inst;

		for(inst = web_inst_first(serv); inst != NULL; inst = web_inst_next(inst)) {
			if(idx-- == 0)
				break;
		}

		if(inst == NULL)
			return false;

		if(strcmp(type, web_inst_type(inst->type)) != 0)
			return false;

		switch(inst->type) {
		case web_mach_v: return web_mach_req(inst->data.mach, path + n, args);
		case web_player_v: return web_player_req(inst->data.player, path + n, args);
		}

		return false;
	}
	else
		return false;
}


struct web_inst_t *inst_new(struct web_serv_t *serv, char *id, enum web_inst_e type, union web_inst_u data)
{
	struct web_inst_t *inst;

	inst = malloc(sizeof(struct web_inst_t));
	inst->serv = serv;
	inst->id = id;
	inst->refcnt = 1;
	inst->type = type;
	inst->data = data;
	inst->node.ref = id;

	return inst;
}


/**
 * Retrieve the first instance.
 *   @serv: The server.
 *   &returns: The instance.
 */
struct web_inst_t *web_inst_first(struct web_serv_t *serv)
{
	struct avltree_node_t *node;

	node = avltree_root_first(&serv->inst);
	return node ? getparent(node, struct web_inst_t, node) : NULL;
}

/**
 * Retrieve the next instance.
 *   @inst: The current instance.
 *   &returns: The instance.
 */
struct web_inst_t *web_inst_next(struct web_inst_t *inst)
{
	struct avltree_node_t *node;

	node = avltree_node_next(&inst->node);
	return node ? getparent(node, struct web_inst_t, node) : NULL;
}

/**
 * Find an instance from the server.
 *   @serv: The server.
 *   @id: The identifier.
 *   &returns: The instance or null.
 */
struct web_inst_t *web_serv_lookup(struct web_serv_t *serv, const char *id)
{
	struct avltree_node_t *node;

	node = avltree_root_lookup(&serv->inst, id);
	return node ? getparent(node, struct web_inst_t, node) : NULL;
}


/**
 * Create or reference a machine instance.
 *   @serv: The server.
 *   @id: The identifier.
 *   &returns: The instance.
 */
struct web_inst_t *web_serv_mach(struct web_serv_t *serv, const char *id)
{
	struct web_inst_t *inst;

	inst = web_serv_lookup(serv, id);
	if(inst == NULL) {
		char *name = strdup(id);

		inst = inst_new(serv, name, web_mach_v, (union web_inst_u){ .mach = web_mach_new(name) });
		avltree_root_insert(&serv->inst, &inst->node);
	}
	else
		inst->refcnt++;

	return inst;
}

/**
 * Create or reference a player instance.
 *   @serv: The server.
 *   @id: The identifier.
 *   &returns: The instance.
 */
struct web_inst_t *web_serv_player(struct web_serv_t *serv, const char *id)
{
	struct web_inst_t *inst;

	inst = web_serv_lookup(serv, id);
	if(inst == NULL) {
		char *name = strdup(id);

		inst = inst_new(serv, name, web_player_v, (union web_inst_u){ .player = web_player_new(name) });
		avltree_root_insert(&serv->inst, &inst->node);
	}
	else
		inst->refcnt++;

	return inst;
}


/**
 * Add a reference to the instance.
 *   @inst: The instance.
 *   &returns: The instance.
 */
struct web_inst_t *web_inst_ref(struct web_inst_t *inst)
{
	inst->refcnt++;

	return inst;
}

/**
 * Unreference an instance.
 *   @inst: The instance.
 */
void web_inst_unref(struct web_inst_t *inst)
{
	if(inst->refcnt-- > 1)
		return;

	switch(inst->type) {
	case web_mach_v: web_mach_delete(inst->data.mach); break;
	case web_player_v: web_player_delete(inst->data.player); break;
	}

	avltree_root_remove(&inst->serv->inst, inst->id);
	free(inst->id);
	free(inst);
}


/**
 * Handle information on an instance.
 *   @inst: The instance.
 *   @info: The information.
 */
void web_inst_info(struct web_inst_t *inst, struct amp_info_t info)
{
	switch(inst->type) {
	case web_mach_v:
		web_mach_info(inst->data.mach, info);
		break;

	case web_player_v:
		web_player_info(inst->data.player, info);
		break;

	default:
		fatal("Invalid instance type.");
	}
}

/**
 * Process an instance as a sequencer.
 *   @inst: The instance.
 *   @buf: The buffer.
 *   @time: The time.
 *   @len: The length.
 *   @queue: The action queue.
 *   &returns: The continuation flag.
 */
bool web_inst_seq(struct web_inst_t *inst, struct amp_time_t *time, unsigned int len, struct amp_queue_t *queue)
{
	bool ret;

	if(!sys_mutex_trylock(&inst->serv->lock)) {
		printf("tryfail\n");
		return false;
	}

	switch(inst->type) {
	case web_mach_v:
		ret = web_mach_proc(inst->data.mach, time, len, queue);
		break;

	case web_player_v:
		ret = web_player_proc(inst->data.player, time, len, queue);
		break;

	default:
		fatal("Invalid instance type.");
	}

	sys_mutex_unlock(&inst->serv->lock);

	return ret;
}


/**
 * Retreive the name of a type.
 *   @type: The type.
 *   &returns: The the name.
 */
const char *web_inst_type(enum web_inst_e type)
{
	switch(type) {
	case web_mach_v: return "mach";
	case web_player_v: return "player";
	}

	fatal("Invalid instance type.");
}

/**
 * Retrieve a chunk for the instance.
 *   @inst: The instance.
 *   &returns: The chunk.
 */
struct io_chunk_t web_inst_chunk(struct web_inst_t *inst)
{
	switch(inst->type) {
	case web_mach_v: return web_mach_chunk(inst->data.mach);
	case web_player_v: return web_player_chunk(inst->data.player);
	}

	fatal("Invalid instance type.");
}


char *fs_getfile_send(const char *path, struct io_file_t file)
{
	FILE *fp;
	size_t rd;
	uint8_t buf[4096];

	fp = fopen(path, "r");
	if(fp == NULL)
		return mprintf("Failed to open file '%s'. %s.", path, strerror(errno));

	while(true) {
		rd = fread(buf, 1, 4096, fp);
		if(rd == 0)
			break;

		io_file_write(file, buf, rd);
	}

	fclose(fp);

	return NULL;
}


/**
 * Asset structure.
 *   @req, path, type: The request, path, and type.
 */
struct http_asset_t {
	const char *req, *path, *type;
};

struct http_asset_t serv_assets[] = {
	{ "/",        "index.xhtml", "application/xhtml+xml" },
	{ "/draw.js", "draw.js",     "application/javascript" },
	{ "/gui.js",  "gui.js",      "application/javascript" },
	{ "/gui.css", "gui.css",     "text/css" },
	{ "/web.js",  "web.js",      "application/javascript" },
	{ "/web.css", "web.css",     "text/css" },
	{ NULL,       NULL,          NULL }
};

/**
 * Process an asset list.
 *   @assets: The asset list.
 *   @path: The path.
 *   @args: The argument.
 *   @prefix: The prefix.
 *   &returns: True if handled, false otherwise.
 */
bool http_asset_proc(struct http_asset_t *assets, const char *path, struct http_args_t *args, const char *prefix)
{
	while(assets->req != NULL) {
		if(strcmp(path, assets->req) == 0)
			break;

		assets++;
	}

	if(assets->req == NULL)
		return false;

	{
		unsigned int len = lprintf("%s%s", prefix, assets->path);
		char buf[len+1];

		sprintf(buf, "%s%s", prefix, assets->path);
		chkwarn(fs_getfile_send(buf, args->file));
	}

	http_head_add(&args->resp, "Content-Type", assets->type);

	return true;
}


/**
 * Handle requests on the server.
 *   @path: The path.
 *   @args: The arguments.
 *   @arg: The argument.
 *   &returns: True if handled.
 */
static bool serv_handler(const char *path, struct http_args_t *args, void *arg)
{
	bool ret;
	struct web_serv_t *serv = arg;

	if(http_asset_proc(serv_assets, path, args, SHAREDIR "/ampweb/"))
		return true;

	sys_mutex_lock(&serv->lock);
	ret = web_serv_req(arg, path, args);
	sys_mutex_unlock(&serv->lock);

	return ret;
}
