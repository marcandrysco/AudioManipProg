#include "common.h"


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
static bool req_handler(const char *path, struct http_args_t *args, void *arg);
static bool req_proc(struct web_serv_t *serv, struct http_args_t *args, struct json_t *json);
static bool req_time(struct web_serv_t *serv, struct http_args_t *args, struct json_obj_t *obj);



/**
 * Create the server.
 *   @core: The RT core.
 *   &returns: The server.
 */
struct web_serv_t *web_serv_new(struct amp_rt_t *rt)
{
	struct web_serv_t *serv;

	serv = malloc(sizeof(struct web_serv_t));
	serv->rt = rt;
	serv->lock = sys_mutex_init(0);
	serv->inst = avltree_root_init(compare_str);
	serv->task = http_server_async(8080, req_handler, serv);

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

		inst = inst_new(serv, name, web_player_v, (union web_inst_u){ .player = web_player_new(serv, name) });
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
	{  "/",               "index.xhtml",    "application/xhtml+xml"   },
	{  "/draw.js",        "draw.js",        "application/javascript"  },
	{  "/gui.js",         "gui.js",         "application/javascript"  },
	{  "/gui.css",        "gui.css",        "text/css"                },
	{  "/web.js",         "web.js",         "application/javascript"  },
	{  "/web.base.js",    "web.base.js",    "application/javascript"  },
	{  "/web.player.js",  "web.player.js",  "application/javascript"  },
	{  "/web.time.js",    "web.time.js",    "application/javascript"  },
	{  "/web.css",        "web.css",        "text/css"                },
	{  "/web.time.css",   "web.time.css",   "text/css"                },
	{  NULL,              NULL,             NULL                      }
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
static bool req_handler(const char *path, struct http_args_t *args, void *arg)
{
	struct web_serv_t *serv = arg;

	if(http_asset_proc(serv_assets, path, args, SHAREDIR "/ampweb/"))
		return true;
	else if(strcmp(path, "/init") == 0) {
		bool run;
		struct amp_loc_t loc;
		struct web_inst_t *inst;

		run = amp_rt_status(serv->rt);
		amp_clock_info(serv->rt->engine->clock, amp_info_loc(&loc));

		hprintf(args->file, "[");

		hprintf(args->file, "{\"id\":\"Time\",\"type\":\"time\",\"data\":{\"run\":%s,\"loc\":{\"bar\":%d,\"beat\":%.8f}}}", run ? "true" : "false", loc.bar, loc.beat);

		for(inst = web_inst_first(web_serv); inst != NULL; inst = web_inst_next(inst))
			hprintf(args->file, ",{\"id\":\"%s\",\"type\":\"%s\",\"data\":%C}", inst->id, web_inst_type(inst->type), web_inst_chunk(inst));

		hprintf(args->file, "]");
		http_head_add(&args->resp, "Content-Type", "application/json");

		return true;
	}
	else if(strcmp(path, "/get") == 0) {
		struct amp_loc_t loc;

		amp_clock_info(serv->rt->engine->clock, amp_info_loc(&loc));

		http_head_add(&args->resp, "Content-Type", "application/json");
		hprintf(args->file, "{");

		hprintf(args->file, "\"time\": { \"bar\": %d, \"beat\": %.8f }", loc.bar, loc.beat);

		hprintf(args->file, "}");

		return true;
	}
	else if(strcmp(path, "/put") == 0) {
		bool suc;
		struct json_t *json;

		if(!chkbool(json_parse_str(&json, args->body)))
			return false;

		suc = req_proc(arg, args, json);
		json_delete(json);

		return suc;
	}
	else
		return false;
}

/**
 * Process the request.
 *   @serv: The server.
 *   @args: The arguments.
 *   @json: The JSON object.
 *   &returns: True if handled.
 */
static bool req_proc(struct web_serv_t *serv, struct http_args_t *args, struct json_t *json)
{
	int id, idx;
	struct web_inst_t *inst;

	if(!json_chk_obj(json, "id", "idx", "data", NULL))
		return mprintf("Invalid request.");

	chk(json_get_int(json_obj_getval(json->data.obj, "id"), &id));
	chk(json_get_int(json_obj_getval(json->data.obj, "idx"), &idx));

	json = json_obj_getval(json->data.obj, "data");
	if(json->type != json_obj_v)
		return false;

	if(idx == 0)
		return req_time(serv, args, json->data.obj);

	idx--;
	for(inst = web_inst_first(serv); inst != NULL; inst = web_inst_next(inst)) {
		if(idx-- == 0)
			break;
	}

	if(inst == NULL)
		return false;

	switch(inst->type) {
	case web_mach_v: return false; //web_mach_req(inst->data.mach, path + n, args);
	case web_player_v: return web_player_req(inst->data.player, args, json);
	}

	return false;
}

/**
 * Process a time request.
 *   @serv: The server.
 *   @args: The arguments.
 *   @obj: The JSON request object.
 *   &returns: True if handled.
 */
static bool req_time(struct web_serv_t *serv, struct http_args_t *args, struct json_obj_t *obj)
{
	const char *type;

	chk(json_str_objget(obj, "type", &type));

	if(strcasecmp(type, "start") == 0)
		amp_rt_start(serv->rt);
	else if(strcasecmp(type, "stop") == 0)
		amp_rt_stop(serv->rt);
	else
		return false;
	
	hprintf(args->file, "ok");

	return true;
}


/**
 * Handle a request on the server.
 *   @serv: The server.
 *   @path: The path.
 *   @args: The arguments.
 *   &returns: True if handled.
static bool req_handle(struct web_serv_t *serv, const char *path, struct http_args_t *args)
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
	else if(strcmp(path, "/time") == 0) {
		if(strcasecmp(args->req.verb, "GET") == 0) {
			struct amp_loc_t loc;

			amp_clock_info(serv->rt->engine->clock, amp_info_loc(&loc));

			hprintf(args->file, "{ \"bar\": %d, \"beat\": %.8f }", loc.bar, loc.beat);
			http_head_add(&args->resp, "Content-Type", "application/json");

			return true;
		}
		else if(strcasecmp(args->req.verb, "POST") == 0) {
			if(strcasecmp(args->body, "start") == 0)
				amp_rt_start(serv->rt);
			else if(strcasecmp(args->body, "stop") == 0)
				amp_rt_stop(serv->rt);
			else
				return false;

			hprintf(args->file, "ok");
			return true;
		}
		else
			return false;
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
 */
