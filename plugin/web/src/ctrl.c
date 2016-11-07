#include "common.h"


/**
 * Controller structure.
 *   @serv: The server.
 *   @id: The identifier.
 *   @enable: Enable flag.
 *   @lock: Run lock.
 *   @inst: The instance list.
 *   @event, cur: The event list and current event.
 *   @nbars, nbeats, ndivs: THe number of bars, beats, and divisions.
 */
struct web_ctrl_t {
	struct web_serv_t *serv;
	const char *id;

	bool enable;
	sys_mutex_t lock;

	struct web_ctrl_inst_t *inst;
	struct web_ctrl_event_t *event, *cur;

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
 *   @val: The value.
 *   @inst: The instance.
 *   @next: The next event.
 */
struct web_ctrl_event_t {
	struct amp_loc_t loc;

	uint16_t val;
	struct web_ctrl_t *inst;

	struct web_ctrl_event_t *next;
};


/*
 * local declarations
 */
static void ctrl_proc(struct io_file_t file, void *arg);

static struct web_ctrl_inst_t *inst_new(char *id, uint16_t dev, uint16_t key);
static void inst_delete(struct web_ctrl_inst_t *inst);


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
	ctrl->serv = serv;
	ctrl->id = id;
	ctrl->enable = true;
	ctrl->lock = sys_mutex_init(0);
	ctrl->inst = NULL;
	ctrl->event = NULL;
	ctrl->cur = NULL;
	ctrl->nbars = 100.0f;
	ctrl->nbeats = 4.0f;
	ctrl->ndivs = 4.0f;

	erase(web_ctrl_load(ctrl));
	if(ctrl->inst == NULL) {
		struct web_ctrl_inst_t **inst = &ctrl->inst;

		for(char i = 'A'; i <= 'E'; i++)
			*inst = inst_new(mprintf("Control %c", i), 1, 1), inst = &(*inst)->next;
	}

	if(ctrl->enable)
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

	if(ctrl->enable)
		sys_mutex_unlock(&ctrl->lock);

	erase(web_ctrl_save(ctrl));

	while(ctrl->inst != NULL) {
		inst = ctrl->inst;
		ctrl->inst = inst->next;

		inst_delete(inst);
	}

	sys_mutex_destroy(&ctrl->lock);
	free(ctrl);
}


/**
 * Process information on a controller.
 *   @ctrl: The controller.
 *   @info: The information.
 */
void web_ctrl_info(struct web_ctrl_t *ctrl, struct amp_info_t info)
{
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

	hprintf(file, "{\"inst\":[");

	for(inst = ctrl->inst; inst != NULL; inst = inst->next)
		hprintf(file, "{\"id\":\"%s\",\"dev\":%u,\"key\":%u}%s", inst->id, inst->dev, inst->key, inst->next ? "," : "");

	hprintf(file, "],\"event\":[");
	hprintf(file, "]}");
}

/**
 * Create a chunk for the controller.
 *   @ctrl: The controller.
 *   &returns: The chunk.
 */
struct io_chunk_t web_ctrl_chunk(struct web_ctrl_t *ctrl)
{
	return (struct io_chunk_t){ ctrl_proc, ctrl };
}
static void ctrl_proc(struct io_file_t file, void *arg)
{
	web_ctrl_print(arg, file);
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
	unsigned int lineno = 1;
	char path[strlen(ctrl->id) + 9];

	sprintf(path, "web.dat/%s", ctrl->id);

	file = fopen(path, "r");
	if(file == NULL)
		return mprintf("Cannot read from '%s'.", path);

	while(true) {
		chkfail(cfg_line_parse(&line, file, &lineno));
		if(line == NULL)
			break;

		if(strcmp(line->key, "Keys") == 0) {
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
