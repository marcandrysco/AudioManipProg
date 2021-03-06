#include "common.h"


/**
 * Instance data union.
 *   @audit: Audit.
 *   @ctrl: Controller.
 *   @mach: Machine.
 *   @mulrec: Multitrack recorder
 *   @player: Player.
 */
union web_inst_u {
	struct web_audit_t *audit;
	struct web_ctrl_t *ctrl;
	struct web_mach_t *mach;
	struct web_mulrec_t *mulrec;
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
const struct amp_effect_i web_iface_effect = {
	(amp_info_f)web_inst_info,
	(amp_effect_f)web_inst_effect,
	(amp_copy_f)web_inst_ref,
	(amp_delete_f)web_inst_unref
};
const struct amp_seq_i web_iface_seq = {
	(amp_info_f)web_inst_info,
	(amp_seq_f)web_inst_seq,
	(amp_copy_f)web_inst_ref,
	(amp_delete_f)web_inst_unref
};

/*
 * local declarations
 */
static void inst_print(struct io_file_t file, void *arg);

static bool req_handler(const char *path, struct http_args_t *args, void *arg);
static bool req_init(struct web_serv_t *serv, struct http_args_t *args);
static bool req_get(struct web_serv_t *serv, struct web_client_t *client, struct http_args_t *args);
static bool req_put(struct web_serv_t *serv, struct http_args_t *args, struct json_t *json);
static bool req_time(struct web_serv_t *serv, struct http_args_t *args, struct json_obj_t *obj);

static struct http_asset_t serv_assets[] = {
	{  "/",               "index.xhtml",    "application/xhtml+xml;charset=utf-8"   },
	{  "/extend.js",      "extend.js",      "application/javascript"                },
	{  "/draw.js",        "draw.js",        "application/javascript"                },
	{  "/gui.js",         "gui.js",         "application/javascript"                },
	{  "/gui.css",        "gui.css",        "text/css"                              },
	{  "/color.js",       "color.js",       "application/javascript"                },
	{  "/web.js",         "web.js",         "application/javascript"                },
	{  "/web.css",        "web.css",        "text/css"                              },
	{  "/web.base.js",    "web.base.js",    "application/javascript"                },
	{  "/web.audit.js",   "web.audit.js",   "application/javascript"                },
	{  "/web.audit.css",  "web.audit.css",  "text/css"                              },
	{  "/web.ctrl.js",    "web.ctrl.js",    "application/javascript"                },
	{  "/web.ctrl.css",   "web.ctrl.css",   "text/css"                              },
	{  "/web.grid.js",    "web.grid.js",    "application/javascript"                },
	{  "/web.key.css",    "web.key.css",    "text/css"                              },
	{  "/web.mach.js",    "web.mach.js",    "application/javascript"                },
	{  "/web.mach.css",   "web.mach.css",   "text/css"                              },
	{  "/web.mulrec.js",  "web.mulrec.js",  "application/javascript"                },
	{  "/web.mulrec.css", "web.mulrec.css", "text/css"                              },
	{  "/web.player.js",  "web.player.js",  "application/javascript"                },
	{  "/web.player.css", "web.player.css", "text/css"                              },
	{  "/web.status.js",  "web.status.js",  "application/javascript"                },
	{  "/web.status.css", "web.status.css", "text/css"                              },
	{  "/web.time.js",    "web.time.js",    "application/javascript"                },
	{  "/web.time.css",   "web.time.css",   "text/css"                              },
	{  NULL,              NULL,             NULL                                    }
};



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
	serv->sync = sys_mutex_init(0);
	serv->lock = sys_mutex_init(0);

	serv->inst = avltree_root_init(compare_str);
	serv->task = http_server_async(8080, req_handler, serv);

	serv->client = NULL;

	return serv;
}

/**
 * Delete the server.
 *   @serv: The server.
 */
void web_serv_delete(struct web_serv_t *serv)
{
	sys_task_delete(serv->task);
	sys_mutex_destroy(&serv->sync);
	sys_mutex_destroy(&serv->lock);
	web_client_destroy(serv->client);
	free(serv);
}


/**
 * Write a JSON value to all clients.
 *   @serv: The server.
 *   @id: The target instance.
 *   @json: The JSON string.
 */
void web_serv_put(struct web_serv_t *serv, const char *id, char *json)
{
	struct web_inst_t *inst;
	unsigned int idx;

	for(inst = web_inst_first(serv), idx = 1; inst != NULL; inst = web_inst_next(inst), idx++) {
		if(strcmp(inst->id, id) == 0)
			break;
	}

	assert(inst != NULL);

	web_client_put(serv->client, idx, json);

	sys_mutex_unlock(&serv->sync);
}

/**
 * Write a JSON value to all clients and lock.
 *   @serv: The server.
 *   @id: The target instance.
 *   @json: The JSON string.
 */
void web_serv_write(struct web_serv_t *serv, const char *id, char *json)
{
	sys_mutex_lock(&serv->sync);
	web_serv_put(serv, id, json);
	sys_mutex_unlock(&serv->sync);
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
 * Create or reference an audit instance.
 *   @serv: The server.
 *   @id: The identifier.
 *   &returns: The instance.
 */
struct web_inst_t *web_serv_audit(struct web_serv_t *serv, const char *id)
{
	struct web_inst_t *inst;

	inst = web_serv_lookup(serv, id);
	if(inst == NULL) {
		char *name = strdup(id);

		inst = inst_new(serv, name, web_audit_v, (union web_inst_u){ .audit = web_audit_new(serv, name) });
		avltree_root_insert(&serv->inst, &inst->node);
	}
	else
		inst->refcnt++;

	return inst;
}

/**
 * Create or reference an controller instance.
 *   @serv: The server.
 *   @id: The identifier.
 *   &returns: The instance.
 */
struct web_inst_t *web_serv_ctrl(struct web_serv_t *serv, const char *id)
{
	struct web_inst_t *inst;

	inst = web_serv_lookup(serv, id);
	if(inst == NULL) {
		char *name = strdup(id);

		inst = inst_new(serv, name, web_ctrl_v, (union web_inst_u){ .ctrl = web_ctrl_new(serv, name) });
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
	case web_audit_v: web_audit_delete(inst->data.audit); break;
	case web_ctrl_v: web_ctrl_delete(inst->data.ctrl); break;
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
	case web_audit_v:
		web_audit_info(inst->data.audit, info);
		break;

	case web_ctrl_v:
		web_ctrl_info(inst->data.ctrl, info);
		break;

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
 * Process an instance as an effect.
 *   @inst: The instance.
 *   @buf: The buffer.
 *   @time: The time.
 *   @len: The length.
 *   @queue: The action queue.
 *   &returns: The continuation flag.
 */
bool web_inst_effect(struct web_inst_t *inst, double *buf, struct amp_time_t *time, unsigned int len, struct amp_queue_t *queue)
{
	bool cont;

	if(!sys_mutex_trylock(&inst->serv->lock))
		return false;

	switch(inst->type) {
	case web_audit_v:
		cont = web_audit_proc(inst->data.audit, buf, time, len, queue);
		break;

	default:
		fatal("Invalid instance type.");
	}

	sys_mutex_unlock(&inst->serv->lock);

	return cont;
}

/**
 * Process an instance as a sequencer.
 *   @inst: The instance.
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
	case web_ctrl_v:
		ret = web_ctrl_proc(inst->data.ctrl, time, len, queue);
		break;

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
	case web_audit_v: return "audit";
	case web_ctrl_v: return "ctrl";
	case web_mach_v: return "mach";
	case web_player_v: return "player";
	}

	fatal("Invalid instance type.");
}

/**
 * Print an instance.
 *   @inst: The instance.
 *   @file: The file.
 */
void web_inst_print(struct web_inst_t *inst, struct io_file_t file)
{
	switch(inst->type) {
	case web_audit_v: return web_audit_print(inst->data.audit, file);
	case web_ctrl_v: return web_ctrl_print(inst->data.ctrl, file);
	case web_mach_v: return web_mach_print(inst->data.mach, file);
	case web_player_v: return web_player_print(inst->data.player, file);
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
	return (struct io_chunk_t){ inst_print, inst };
}
static void inst_print(struct io_file_t file, void *arg)
{
	web_inst_print(arg, file);
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

	web_client_clean(&serv->client);

#ifdef WINDOWS
	if(http_asset_proc(serv_assets, path, args, "ampweb/"))
#else
	if(http_asset_proc(serv_assets, path, args, SHAREDIR "/ampweb/"))
#endif
		return true;

	if(strcmp(path, "/init") == 0) {
		bool suc;

		sys_mutex_lock(&serv->sync);
		suc = req_init(serv, args);
		sys_mutex_unlock(&serv->sync);

		return suc;
	}
	else if(strcmp(path, "/get") == 0) {
		unsigned int id;
		struct web_client_t *client;

		sys_mutex_lock(&serv->sync);

		id = strtoul(args->body, NULL, 10);
		client = web_client_get(serv->client, id);
		if(client != NULL) {
			web_client_alive(client);
			req_get(serv, client, args);
		}
		else
			hprintf(args->file, "null");

		sys_mutex_unlock(&serv->sync);

		http_head_add(&args->resp, "Content-Type", "application/json");

		return true;
	}
	else if(strcmp(path, "/put") == 0) {
		bool suc;
		struct json_t *json;

		if(!chkbool(json_parse_str(&json, args->body)))
			return false;

		sys_mutex_lock(&serv->sync);
		suc = req_put(arg, args, json);
		sys_mutex_unlock(&serv->sync);

		json_delete(json);

		return suc;
	}
	else
		return false;
}

/**
 * Process an init request.
 *   @serv: The server.
 *   @args: The arguments.
 *   &returns: True if handled.
 */
static bool req_init(struct web_serv_t *serv, struct http_args_t *args)
{
	const char *run;
	struct amp_loc_t loc;
	struct web_inst_t *inst;

	run = amp_rt_status(serv->rt) ? "true" : "false";
	amp_clock_info(serv->rt->engine->clock, amp_info_loc(&loc));

	hprintf(args->file, "{\"client\":%u,\"data\":[", web_client_add(&serv->client));

	hprintf(args->file, "{\"id\":\"Time\",\"type\":\"time\",\"data\":{\"run\":%s,\"loc\":{\"bar\":%d,\"beat\":%.8f}}}", run, loc.bar, loc.beat);

	for(inst = web_inst_first(web_serv); inst != NULL; inst = web_inst_next(inst))
		hprintf(args->file, ",{\"id\":\"%s\",\"type\":\"%s\",\"data\":%C}", inst->id, web_inst_type(inst->type), web_inst_chunk(inst));

	hprintf(args->file, "]}");
	http_head_add(&args->resp, "Content-Type", "application/json");

	return true;
}

/**
 * Process a get request.
 *   @serv: The server.
 *   @client: The client.
 *   @args: The arguments.
 *   &returns: True if handled.
 */
static bool req_get(struct web_serv_t *serv, struct web_client_t *client, struct http_args_t *args)
{
	const char *run;
	unsigned int idx;
	struct amp_loc_t loc;
	struct web_inst_t *inst;
	struct web_msg_t **msg, *cur;

	run = amp_rt_status(serv->rt) ? "true" : "false";
	amp_clock_info(serv->rt->engine->clock, amp_info_loc(&loc));

	hprintf(args->file, "[{ \"run\": %s, \"loc\": { \"bar\": %d, \"beat\": %.8f } }", run, loc.bar, loc.beat);

	for(inst = web_inst_first(web_serv), idx = 1; inst != NULL; inst = web_inst_next(inst), idx++) {
		bool comma = false;

		hprintf(args->file, ",[");

		msg = &client->msg;
		while(*msg != NULL) {
			if((*msg)->idx == idx) {
				cur = *msg;
				*msg = cur->next;

				hprintf(args->file, "%s%s", comma ? "," : "", cur->json);
				comma = true;

				free(cur->json);
				free(cur);
			}
			else
				msg = &(*msg)->next;
		}

		hprintf(args->file, "]");
	}

	hprintf(args->file, "]");

	return true;
}

/**
 * Process a put request.
 *   @serv: The server.
 *   @args: The arguments.
 *   @json: The JSON object.
 *   &returns: True if handled.
 */
static bool req_put(struct web_serv_t *serv, struct http_args_t *args, struct json_t *json)
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
	case web_mach_v: return web_mach_req(inst->data.mach, args, json);
	case web_player_v: return web_player_req(inst->data.player, args, json);
	case web_audit_v: return false;
	case web_ctrl_v: return web_ctrl_req(inst->data.ctrl, args, json);;
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
	else if(strcasecmp(type, "seek") == 0) {
		double bar;
		struct json_t *json;

		json = json_obj_getval(obj, "bar");
		if(json == NULL)
			return false;

		chk(json_get_double(json, &bar));
		amp_rt_seek(serv->rt, bar);
	}
	else
		return false;
	
	hprintf(args->file, "ok");

	return true;
}
