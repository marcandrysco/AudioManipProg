#include "common.h"


/**
 * Controller structure.
 *   @id: The identifier.
 *   @rt: The RT structure.
 *   @serv: The server.
 *   @enable, run, rec: Enable, run, and record flags.
 *   @sync, lock: Synchronization and run locks.
 *   @read, write: Asynchronous read and write.
 *   @queue: The queue of elements to add.
 *   @inst: The instance list.
 *   @event, cur: The event list and current event.
 *   @nbars, nbeats, ndivs: THe number of bars, beats, and divisions.
 */
struct web_ctrl_t {
	const char *id;
	struct amp_rt_t *rt;
	struct web_serv_t *serv;

	bool enable, run, rec;
	sys_mutex_t sync, lock;

	struct sys_task_t *task;
	struct amp_async_t *read, *write;
	struct web_ctrl_event_t *queue;

	struct web_ctrl_inst_t *inst;
	struct web_ctrl_event_t *event, **cur;

	float nbars, nbeats, ndivs;
};

/**
 * Control instance structure.
 *   @id: The instance name.
 *   @dev, key: The device and key.
 *   @next: The next instance.
 */
struct web_ctrl_inst_t {
	char *id;
	uint16_t dev, key;

	struct web_ctrl_inst_t *next;
};

/**
 * Control event structure.
 *   @loc: The location.
 *   @inst: The instance.
 *   @val: The value.
 *   @next: The next event.
 */
struct web_ctrl_event_t {
	struct amp_loc_t loc;

	struct web_ctrl_inst_t *inst;
	uint16_t val;

	struct web_ctrl_event_t *next;
};

/**
 * Read event structure.
 *   @idx, val: Index and value.
 */
struct web_ctrl_read_t {
	uint16_t idx, val;
};

/**
 * Write event structure.
 *   @loc: The location.
 *   @idx, val: The index and value.
 */
struct web_ctrl_write_t {
	struct amp_loc_t loc;
	uint16_t dev, key, val;
};


/*
 * local declarations
 */
static struct web_ctrl_inst_t *inst_new(char *id, uint16_t dev, uint16_t key);
static void inst_delete(struct web_ctrl_inst_t *inst);

static unsigned int inst_idx(struct web_ctrl_t *ctrl, struct web_ctrl_inst_t *inst);
static struct web_ctrl_inst_t *inst_get(struct web_ctrl_t *ctrl, unsigned int idx);
static struct web_ctrl_inst_t *inst_lookup(struct web_ctrl_t *ctrl, uint16_t dev, uint16_t key);

static void write_proc(sys_fd_t fd, void *arg);


/**
 * Create a new ctrler.
 *   @serv: The server.
 *   @id: The indetifier.
 *   &returns: The ctrler.
 */
struct web_ctrl_t *web_ctrl_new(struct web_serv_t *serv, const char *id)
{
	struct web_ctrl_t *ctrl;

	ctrl = malloc(sizeof(struct web_ctrl_t));
	ctrl->id = id;
	ctrl->rt = serv->rt;
	ctrl->serv = serv;

	ctrl->enable = true;
	ctrl->run = false;
	ctrl->rec = false;
	ctrl->sync = sys_mutex_init(0);
	ctrl->lock = sys_mutex_init(0);

	ctrl->read = amp_async_new(64 * sizeof(struct web_ctrl_read_t));
	ctrl->write = amp_async_new(256 * sizeof(struct web_ctrl_write_t));
	ctrl->task = sys_task_new(write_proc, ctrl);

	ctrl->inst = NULL;
	ctrl->event = NULL;
	ctrl->cur = &ctrl->event;

	ctrl->nbars = 100.0f;
	ctrl->nbeats = 4.0f;
	ctrl->ndivs = 4.0f;

	erase(web_ctrl_load(ctrl));

	if(ctrl->inst == NULL) {
		struct web_ctrl_inst_t **inst = &ctrl->inst;

		for(char i = 'A'; i <= 'E'; i++)
			*inst = inst_new(mprintf("Control %c", i), 1, 1), inst = &(*inst)->next;
	}

	if(!ctrl->enable)
		sys_mutex_lock(&ctrl->lock);

	return ctrl;
}

/**
 * Create an controller from a value.
 *   @ret: Ref. The returned value.
 *   @value: The value.
 *   @env: The environment.
 *   &returns: Error.
 */
char *web_ctrl_make(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env)
{
	if(value->type != ml_value_str_v)
		return mprintf("WebCtrl takes a string as input.");

	*ret = amp_pack_seq(amp_seq(web_serv_ctrl(web_serv, value->data.str), &web_iface_seq));
	return NULL;
}

/**
 * Delete a controller.
 *   @ctrl: The controller.
 */
void web_ctrl_delete(struct web_ctrl_t *ctrl)
{
	struct web_ctrl_inst_t *inst;
	struct web_ctrl_event_t *event;

	if(!ctrl->enable)
		sys_mutex_unlock(&ctrl->lock);

	sys_task_delete(ctrl->task);
	erase(web_ctrl_save(ctrl));

	while(ctrl->inst != NULL) {
		inst = ctrl->inst;
		ctrl->inst = inst->next;

		inst_delete(inst);
	}

	while(ctrl->event != NULL) {
		event = ctrl->event;
		ctrl->event = event->next;

		free(event);
	}

	sys_mutex_destroy(&ctrl->sync);
	sys_mutex_destroy(&ctrl->lock);
	amp_async_delete(ctrl->read);
	amp_async_delete(ctrl->write);
	free(ctrl);
}


/**
 * Add to the controller.
 *   @ctrl: The controller.
 *   @loc: The location.
 *   @inst: The instance.
 *   @val: The value.
 */
void web_ctrl_add(struct web_ctrl_t *ctrl, struct amp_loc_t loc, struct web_ctrl_inst_t *inst, uint16_t val)
{
	struct web_ctrl_event_t *event, **ref;

	event = malloc(sizeof(struct web_ctrl_event_t));
	event->loc = loc;
	event->inst = inst;
	event->val = val;

	for(ref = &ctrl->event; *ref != NULL; ref = &(*ref)->next) {
		if(amp_loc_cmp((*ref)->loc, loc) > 0)
			break;
	}

	event->next = *ref;
	*ref = event;

	/*
	if(amp_loc_cmp(begin, ctrl->last) >= 0) {
		if((ctrl->left == NULL) || (amp_loc_cmp(begin, ctrl->left->begin) < 0))
			ctrl->left = event;
	}
	*/
}

void web_ctrl_remove(struct web_ctrl_t *ctrl, struct amp_loc_t loc, struct web_ctrl_inst_t *inst, uint16_t val)
{
	struct web_ctrl_event_t *rem, **event;

	for(event = &ctrl->event; *event != NULL; event = &(*event)->next) {
		if((amp_loc_cmp((*event)->loc, loc) == 0) && ((*event)->inst == inst) && ((*event)->val == val))
			break;
	}

	if(*event == NULL)
		return;

	rem = *event;
	*event = rem->next;
	free(rem);
}


/**
 * Process information on a controller.
 *   @ctrl: The controller.
 *   @info: The information.
 */
void web_ctrl_info(struct web_ctrl_t *ctrl, struct amp_info_t info)
{
	switch(info.type) {
	case amp_info_start_v:
		sys_mutex_lock(&ctrl->sync);
		ctrl->run = true;
		ctrl->queue = NULL;
		sys_mutex_unlock(&ctrl->sync);

		break;

	case amp_info_stop_v:
		sys_mutex_lock(&ctrl->sync);
		ctrl->run = false;

		{
			struct web_ctrl_event_t *event;

			while(ctrl->queue != NULL) {
				event = ctrl->queue;
				ctrl->queue = event->next;

				web_ctrl_add(ctrl, event->loc, event->inst, event->val);
				free(event);
			}
		}

		sys_mutex_unlock(&ctrl->sync);

		break;

	default:
		break;
	}
}

/**
 * Process data on a controller.
 *   @ctrl: The controller.
 *   @time: The time.
 *   @len: The length.
 *   @queue: The action queue.
 *   &returns: The continuation flag.
 */
bool web_ctrl_proc(struct web_ctrl_t *ctrl, struct amp_time_t *time, unsigned int len, struct amp_queue_t *queue)
{
	unsigned int i, n = 0;
	struct amp_event_t *event;
	struct web_ctrl_read_t read;
	struct web_ctrl_write_t write;
	struct web_ctrl_inst_t *inst;

	if(!sys_mutex_trylock(&ctrl->lock))
		return false;

	while(amp_async_read(ctrl->read, &read, sizeof(struct web_ctrl_read_t))) {
		inst = inst_get(ctrl, read.idx);
		amp_queue_add(queue, amp_action(0, amp_event(inst->dev, inst->key, read.val), queue));
	}

	for(i = 0; i < len; i++) {
		while((event = amp_queue_event(queue, &n, i)) != NULL) {
			write.loc = amp_loc(time[i].bar, time[i].beat);
			write.dev = event->dev;
			write.key = event->key;
			write.val = event->val;
			amp_async_write(ctrl->write, &write, sizeof(struct web_ctrl_write_t));
		}
	}

	sys_mutex_unlock(&ctrl->lock);

	return false;
}


/**
 * Retrieve the information from a controller.
 *   @ctrl: The controller.
 *   @file: The file.
 */
void web_ctrl_print(struct web_ctrl_t *ctrl, struct io_file_t file)
{
	struct web_ctrl_inst_t *inst;
	struct web_ctrl_event_t *event;

	hprintf(file, "{\"inst\":[");

	for(inst = ctrl->inst; inst != NULL; inst = inst->next)
		hprintf(file, "{\"id\":\"%s\",\"dev\":%u,\"key\":%u}%s", inst->id, inst->dev, inst->key, inst->next ? "," : "");

	hprintf(file, "],\"events\":[");

	for(event = ctrl->event; event != NULL; event = event->next)
		hprintf(file, "{\"loc\":{\"bar\":%d,\"beat\":%.17e},\"idx\":%u,\"val\":%u}%s", event->loc.bar, event->loc.beat, inst_idx(ctrl, event->inst), event->val, event->next ? "," : "");

	hprintf(file, "]}");
}

/**
 * Handle a controller requeust.
 *   @ctrl: The controller.
 *   @args: The arguments.
 *   @json: The json object.
 *   &returns: True if handled.
 */
bool web_ctrl_req(struct web_ctrl_t *ctrl, struct http_args_t *args, struct json_t *json)
{
	const char *type;

	chk(json_chk_obj(json, "type", "data", NULL));
	chk(json_str_objget(json->data.obj, "type", &type));
	json = json_obj_getval(json->data.obj, "data");

	if(strcmp(type, "action") == 0) {
		struct web_ctrl_read_t read;

		chk(chkbool(json_getf(json, "{idx:u16,val:u16$}", &read.idx, &read.val)));

		if(ctrl->enable)
			amp_async_write(ctrl->read, &read, sizeof(struct web_ctrl_read_t));
	}
	else if(strcmp(type, "enable") == 0) {
		if(json->type == json_true_v) {
			if(ctrl->enable)
				return false;

			ctrl->enable = true;
			sys_mutex_unlock(&ctrl->lock);
		}
		else if(json->type == json_false_v) {
			if(!ctrl->enable)
				return false;

			ctrl->enable = false;
			sys_mutex_lock(&ctrl->lock);
		}
		else
			return false;

		web_serv_put(ctrl->serv, ctrl->id, mprintf("{\"type\":\"enable\",\"data\":%s}", ctrl->enable ? "true" : "false"));
	}
	else if(strcmp(type, "rec") == 0) {
		if(amp_rt_status(ctrl->rt))
			return false;

		if(json->type == json_true_v)
			ctrl->rec = true;
		else if(json->type == json_false_v)
			ctrl->rec = false;
		else
			return false;

		web_serv_put(ctrl->serv, ctrl->id, mprintf("{\"type\":\"rec\",\"data\":%s}", ctrl->rec ? "true" : "false"));
	}
	else if(strcmp(type, "rem") == 0) {
		unsigned int i;

		chk(json->type == json_arr_v);

		for(i = 0; i < json->data.arr->len; i++) {
			struct amp_loc_t loc;
			unsigned int idx;
			uint16_t val;
			struct web_ctrl_inst_t *inst;

			chk(chkwarn(json_getf(json->data.arr->vec[i], "{loc:{bar:d,beat:f$},idx:u,val:u16$}", &loc.bar, &loc.beat, &idx, &val)));

			inst = inst_get(ctrl, idx);
			if(inst != NULL)
				web_ctrl_remove(ctrl, loc, inst, val);
		}
	}
	else
		return false;

	web_ctrl_save(ctrl);

	return true;
}


/**
 * Load a controller from disk.
 *   @ctrl: The control.
 *   &returns: Error.
 */
char *web_ctrl_load(struct web_ctrl_t *ctrl)
{
#define onexit fclose(file); cfg_line_erase(line);
	FILE *file;
	struct cfg_line_t *line;
	unsigned int cnt = 0, lineno = 1;
	char path[strlen(ctrl->id) + 9];
	struct web_ctrl_inst_t **inst = &ctrl->inst;

	sprintf(path, "web.dat/%s", ctrl->id);

	file = fopen(path, "r");
	if(file == NULL)
		return mprintf("Cannot read from '%s'.", path);

	while(true) {
		chkfail(cfg_line_parse(&line, file, &lineno));
		if(line == NULL)
			break;

		if(strcmp(line->key, "Inst") == 0) {
			const char *id;
			uint16_t dev, key;

			chkfail(cfg_readf(line, "s u16 u16 $", &id, &dev, &key));

			cnt++;
			*inst = inst_new(strdup(id), dev, key);
			inst = &(*inst)->next;
		}
		else if(strcmp(line->key, "Event") == 0) {
			uint16_t val;
			unsigned int idx;
			struct amp_loc_t loc;

			chkfail(cfg_readf(line, "d f u u16 $", &loc.bar, &loc.beat, &idx, &val));
			if(idx >= cnt)
				return mprintf("Invalid instance ID.");

			web_ctrl_add(ctrl, loc, inst_get(ctrl, idx), val);
		}
		else
			fail("Unknown directive '%s'.", line->key);

		cfg_line_delete(line);
	}

	fclose(file);
	return NULL;
}

/**
 * Save a controller to disk.
 *   @ctrl: The control.
 *   &returns: Error.
 */
char *web_ctrl_save(struct web_ctrl_t *ctrl)
{
	FILE *file;
	struct web_ctrl_inst_t *inst;
	struct web_ctrl_event_t *event;
	char path[strlen(ctrl->id) + 9];

	fs_trymkdir("web.dat", 0775);
	sprintf(path, "web.dat/%s", ctrl->id);

	file = fopen(path, "w");
	if(file == NULL)
		fatal("Failed to save to path '%s'.", path);

	for(inst = ctrl->inst; inst != NULL; inst = inst->next)
		cfg_writef(file, "Inst", "s u16 u16", inst->id, inst->dev, inst->key);

	for(event = ctrl->event; event != NULL; event = event->next)
		cfg_writef(file, "Event", "d f u u16", event->loc.bar, event->loc.beat, inst_idx(ctrl, event->inst), event->val);

	fclose(file);

	return NULL;
}


/**
 * Create a controller instance.
 *   @id: The identifier.
 *   @dev: The device.
 *   @key: The key.
 *   &returns: The instance.
 */
static struct web_ctrl_inst_t *inst_new(char *id, uint16_t dev, uint16_t key)
{
	struct web_ctrl_inst_t *inst;

	inst = malloc(sizeof(struct web_ctrl_inst_t));
	*inst = (struct web_ctrl_inst_t){ id, dev, key, NULL };

	return inst;
}

/**
 * Delete a controller instance.
 *   @inst: The instance.
 */
static void inst_delete(struct web_ctrl_inst_t *inst)
{
	free(inst->id);
	free(inst);
}


/**
 * Retrieve the index of the instance.
 *   @ctrl: The controller.
 *   @inst: The instance.
 *   &returns: The index.
 */
static unsigned int inst_idx(struct web_ctrl_t *ctrl, struct web_ctrl_inst_t *inst)
{
	unsigned int idx = 0;
	struct web_ctrl_inst_t *iter;

	for(iter = ctrl->inst; iter != inst; iter = iter->next)
		idx++;

	return idx;
}

/**
 * Retrieve the instance at a given index.
 *   @ctrl: The controller.
 *   @idx: The index.
 *   &returns: The instance or null.
 */
static struct web_ctrl_inst_t *inst_get(struct web_ctrl_t *ctrl, unsigned int idx)
{
	struct web_ctrl_inst_t *inst = ctrl->inst;

	while((idx-- > 0) && (inst != NULL))
		inst = inst->next;

	return inst;
}

/**
 * Lookup an instance by device and key.
 *   @ctrl: The controller.
 *   @dev: The device.
 *   @key: The key.
 *   &returns: The instance.
 */
static struct web_ctrl_inst_t *inst_lookup(struct web_ctrl_t *ctrl, uint16_t dev, uint16_t key)
{
	struct web_ctrl_inst_t *inst;

	for(inst = ctrl->inst; inst != NULL; inst = inst->next) {
		if((inst->dev == dev) && (inst->key == key))
			return inst;
	}

	return NULL;
}



/**
 * Process the write task
 *   @fd: The file descriptor.
 *   @arg: The argument.
 */
static void write_proc(sys_fd_t fd, void *arg)
{
	struct web_ctrl_t *ctrl = arg;
	struct web_ctrl_write_t write;

	while(sys_poll1(fd, sys_poll_in_e, 100) == 0) {
		while(amp_async_read(ctrl->write, &write, sizeof(struct web_ctrl_write_t))) {
			sys_mutex_lock(&ctrl->sync);

			if((ctrl->run) && (ctrl->rec)) {
				struct web_ctrl_inst_t *inst;

				inst = inst_lookup(ctrl, write.dev, write.key);
				if(inst != NULL) {
					struct web_ctrl_event_t *event;

					event = malloc(sizeof(struct web_ctrl_event_t));
					event->loc = write.loc;
					event->inst = inst;
					event->val = write.val;
					event->next = ctrl->queue;
					ctrl->queue = event;

					web_serv_write(ctrl->serv, ctrl->id, mprintf("{\"type\":\"event\",\"data\":{\"loc\":%C,\"idx\":%u,\"val\":%u}}", web_pack_loc(&event->loc), inst_idx(ctrl, inst), event->val));
				}
			}

			sys_mutex_unlock(&ctrl->sync);
		}
	}
}
