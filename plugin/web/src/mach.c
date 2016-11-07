#include "common.h"


/**
 * Machine structure.
 *   @id: The indetifier.
 *   @last: The last beat.
 *   @ndivs, nbeats, nbats: The number of divisions, beats and bars.
 *   @event: The event arrays.
 *   @inst: The instance list.
 */
struct web_mach_t {
	const char *id;

	double last;
	unsigned int ndivs, nbeats, nbars;
	struct web_mach_event_t **event[10];

	struct web_mach_inst_t *inst;
};

/**
 * Instance structure.
 *   @id: The identifier.
 *   @on, multi, rel: Enable, multikey, and release flags.
 *   @dev, key: The device
 *   @next: The next instance.
 */
struct web_mach_inst_t {
	char *id;
	bool on, multi, rel;
	uint16_t dev, key;

	struct web_mach_inst_t *next;
};

/**
 * Event structure.
 *   @key, vel: The key and velocity.
 *   @inst: The instace.
 *   @event: The event.
 */
struct web_mach_event_t {
	uint16_t key, vel;
	struct web_mach_inst_t *inst;

	struct web_mach_event_t *next;
};


/*
 * local declarations
 */
static void mach_proc(struct io_file_t file, void *arg);

static struct web_mach_inst_t *inst_new(char *id, uint16_t dev, uint16_t key);
static void inst_delete(struct web_mach_inst_t *inst);
static unsigned int inst_idx(struct web_mach_t *mach, struct web_mach_inst_t *inst);
static struct web_mach_inst_t *inst_get(struct web_mach_t *mach, unsigned int idx);


/**
 * Create a machine.
 *   @id: The identifier.
 */
struct web_mach_t *web_mach_new(const char *id)
{
	struct web_mach_t *mach;
	unsigned int i, n, len;

	len = 4 * 4 * 2;
	mach = malloc(sizeof(struct web_mach_t));
	mach->last = 0.0;
	mach->id = id;
	mach->ndivs = 4;
	mach->nbeats = 4;
	mach->nbars = 2;
	mach->inst = NULL;

	for(n = 0; n < 10; n++) {
		mach->event[n] = malloc(len * sizeof(struct web_mach_inst_t *));
		for(i = 0; i < len; i++)
			mach->event[n][i] = NULL;
	}

	chkwarn(web_mach_load(mach));

	if(mach->inst == NULL) {
		mach->inst = inst_new(strdup("Drum"), 1, 1);
		mach->inst->next = inst_new(strdup("Kick"), 1, 1);
		mach->inst->next->on = false;
		mach->inst->next->next = inst_new(strdup("Low Tom"), 1, 1);
	}

	chkwarn(web_mach_save(mach));

	return mach;
}

/**
 * Create a machine from a value.
 *   @ret: Ref. The returned value.
 *   @value: The value.
 *   @env: The environment.
 *   &returns: Error.
 */
char *web_mach_make(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env)
{
	if(value->type != ml_value_str_v)
		return mprintf("WebMach takes a string as input.");

	*ret = amp_pack_seq(amp_seq(web_serv_mach(web_serv, value->data.str), &web_iface_seq));
	return NULL;
}

/**
 * Delete a machine.
 *   @mach: The machine.
 */
void web_mach_delete(struct web_mach_t *mach)
{
	unsigned int i, n, len = web_mach_len(mach);
	struct web_mach_inst_t *inst;
	struct web_mach_event_t *event;

	for(n = 0; n < 10; n++) {
		for(i = 0; i < len; i++) {
			while(mach->event[n][i] != NULL) {
				event = mach->event[n][i];
				mach->event[n][i] = event->next;
				
				free(event);
			};
		}

		free(mach->event[n]);
	}

	while(mach->inst != NULL) {
		inst = mach->inst;
		mach->inst = inst->next;

		inst_delete(inst);
	}

	free(mach);
}


/**
 * Process information on a machine.
 *   @mach: The machine.
 *   @info: The information.
 */
void web_mach_info(struct web_mach_t *mach, struct amp_info_t info)
{
}

/**
 * Process a machine.
 *   @mach: The machine.
 *   @time: The time.
 *   @len: The length.
 *   @queue: The action queue.
 *   &returns: The continuation flag.
 */
bool web_mach_proc(struct web_mach_t *mach, struct amp_time_t *time, unsigned int len, struct amp_queue_t *queue)
{
	unsigned int i;
	double left, right;

	if(time[0].bar == time[len].bar)
		return false;

	right = mach->last;

	for(i = 1; i <= len; i++) {
		left = right;
		right = fmod(time[i].beat, 1.0 / mach->ndivs);

		if(!amp_bar_between(0.0, left, right))
			continue;

		struct web_mach_inst_t *inst;
		struct web_mach_event_t *event;
		unsigned int bar, beat, div, off;

		bar = (unsigned int)time[i].bar % mach->nbars;
		beat = (unsigned int)time[i].beat;
		div = (time[i].beat - beat) * mach->ndivs;
		off = div + (beat + bar * mach->nbeats) * mach->ndivs;

		for(event = mach->event[0/*TODO*/][off]; event != NULL; event = event->next) {
			inst = event->inst;
			if(!inst->on)
				continue;

			printf("event: %d\n", event->vel);
		}
			//amp_queue_add(queue, (struct amp_action_t){ i, { 0, inst->key+1, inst->vel }, queue });
	}

	mach->last = right;

	return false;
}


/**
 * Retrieve the length of a machine.
 *   @mach: The machine.
 *   &returns: The length.
 */
unsigned int web_mach_len(struct web_mach_t *mach)
{
	return mach->ndivs * mach->nbeats * mach->nbars;
}

/**
 * Set a key-velocity pair on the machine.
 *   @mach: The machine.
 *   @sel: The selected preset.
 *   @inst: The instance.
 *   @off: The offset.
 *   @key: The key.
 *   @vel: The velocity.
 */
void web_mach_set(struct web_mach_t *mach, unsigned int sel, struct web_mach_inst_t *inst, unsigned int off, uint16_t key, uint16_t vel)
{
	struct web_mach_event_t **event, *tmp;

	for(event = &mach->event[sel][off]; *event != NULL; event = &(*event)->next) {
		if((*event)->inst == inst)
			break;
	}

	if(*event != NULL) {
		tmp = *event;
		*event = tmp->next;

		free(tmp);
	}

	if(vel > 0) {
		tmp = malloc(sizeof(struct web_mach_event_t));
		*tmp = (struct web_mach_event_t){ key, vel, inst, *event };
		*event = tmp;
	}
}


/**
 * Retrieve the information from a machine.
 *   @mach: The machine.
 *   @file: The file.
 */
void web_mach_print(struct web_mach_t *mach, struct io_file_t file)
{
	struct web_mach_inst_t *inst;
	struct web_mach_event_t *event;
	unsigned int i, n, len;

	hprintf(file, "{\"conf\":{\"ndivs\":%u,\"nbeats\":%u,\"nbars\":%u},\"inst\":[", mach->ndivs, mach->nbeats, mach->nbars);

	for(inst = mach->inst; inst != NULL; inst = inst->next)
		hprintf(file, "{\"id\":\"%s\",\"on\":%s,\"multi\":%s,\"rel\":%s,\"dev\":%u,\"key\":%u}%s", inst->id, inst->on ? "true" : "false", inst->multi ? "true" : "false", inst->rel ? "true" : "false", inst->dev, inst->key, inst->next ? "," : "");

	hprintf(file, "],\"events\":[");

	len = web_mach_len(mach);
	for(n = 0; n < 10; n++) {
		hprintf(file, (n > 0) ? ",[" : "[");

		for(i = 0; i < len; i++) {
			hprintf(file, (i > 0) ? ",[" : "[");

			for(event = mach->event[n][i]; event != NULL; event = event->next)
				hprintf(file, "{\"key\":%u,\"vel\":%u,\"idx\":%u}%s", event->key, event->vel, inst_idx(mach, event->inst), event->next ? "," : "");

			hprintf(file, "]");
		}

		hprintf(file, "]");
	}

	hprintf(file, "]}");
}

/**
 * Create a chunk for the machine.
 *   @mach: The machine.
 *   &returns: The chunk.
 */
struct io_chunk_t web_mach_chunk(struct web_mach_t *mach)
{
	return (struct io_chunk_t){ mach_proc, mach };
}
static void mach_proc(struct io_file_t file, void *arg)
{
	web_mach_print(arg, file);
}


/**
 * Handle a machine requeust.
 *   @mach: The machine.
 *   @args: The arguments.
 *   @json: The json object.
 *   &returns: True if handled.
 */
bool web_mach_req(struct web_mach_t *mach, struct http_args_t *args, struct json_t *json)
{
	const char *type;

	chk(json_chk_obj(json, "type", "data", NULL));
	chk(json_str_objget(json->data.obj, "type", &type));
	json = json_obj_getval(json->data.obj, "data");

	if(strcmp(type, "edit") == 0) {
		uint16_t key, vel;
		unsigned int sel, idx, off;
		struct web_mach_inst_t *inst;

		chk(chkwarn(json_getf(json, "{sel:u,idx:u,off:u,key:u16,vel:u16$}", &sel, &idx, &off, &key, &vel)));
		chk((sel < 10) && (off < web_mach_len(mach)));
		chk((inst = inst_get(mach, sel)) != NULL);

		web_mach_set(mach, sel, inst, off, key, vel);
	}
	else
		return false;

	web_mach_save(mach);

	return true;
}


/**
 * Save a machine.
 *   @mach: The machine.
 *   &returns: Error.
 */
char *web_mach_save(struct web_mach_t *mach)
{
	FILE *file;
	unsigned int i, n, len;
	struct web_mach_inst_t *inst;
	struct web_mach_event_t *event;
	char path[strlen(mach->id) + 9];

	fs_trymkdir("web.dat", 0775);
	sprintf(path, "web.dat/%s", mach->id);

	file = fopen(path, "w");
	if(file == NULL)
		fatal("Failed to save to path '%s'.", path);

	for(inst = mach->inst; inst != NULL; inst = inst->next)
		cfg_writef(file, "Inst", "s b b b u16 u16", inst->id, inst->on, inst->multi, inst->rel, inst->dev, inst->key);

	len = web_mach_len(mach);
	for(n = 0; n < 10; n++) {
		for(i = 0; i < len; i++) {
			for(event = mach->event[n][i]; event != NULL; event = event->next)
				cfg_writef(file, "Event", "u u u u16 u16", n, i, inst_idx(mach, event->inst), event->key, event->vel);
		}
	}

	fclose(file);

	return NULL;
}

/**
 * Load a drum machine.
 *   @mach: The machine.
 *   &returns: Error.
 */
char *web_mach_load(struct web_mach_t *mach)
{
#define onexit fclose(file); cfg_line_delete(line);
	FILE *file;
	struct cfg_line_t *line;
	unsigned int cnt = 0, lineno = 1;
	char path[strlen(mach->id) + 9];
	struct web_mach_inst_t **inst = &mach->inst;

	sprintf(path, "web.dat/%s", mach->id);

	file = fopen(path, "r");
	if(file == NULL)
		return mprintf("Failed to open machine at '%s'.", path);

	while(true) {
		chkfail(cfg_line_parse(&line, file, &lineno));
		if(line == NULL)
			break;

		if(strcmp(line->key, "Inst") == 0) {
			const char *id;
			bool on, multi, rel;
			uint16_t dev, key;

			chkfail(cfg_readf(line, "s b b b u16 u16 $", &id, &on, &multi, &rel, &dev, &key));

			cnt++;
			*inst = inst_new(strdup(id), dev, key);
			(*inst)->on = on;
			(*inst)->multi = multi;
			(*inst)->rel = rel;
			inst = &(*inst)->next;
		}
		else if(strcmp(line->key, "Event") == 0) {
			unsigned int n, i, idx;
			uint16_t key, vel;

			chkfail(cfg_readf(line, "u u u u16 u16", &n, &i, &idx, &key, &vel));
			if(idx >= cnt)
				return mprintf("Invalid instance ID.");

			web_mach_set(mach, n, inst_get(mach, idx), i, key, vel);
		}
		else
			fail("Unknown directive '%s'.", line->key);

		cfg_line_delete(line);
	}

	*inst = NULL;
	fclose(file);

	return NULL;
#undef onexit
}


/**
 * Create a new instance.
 *   @id: The identifier.
 *   @dev: The device.
 *   @key: The key.
 *   &returns: The instance.
 */
static struct web_mach_inst_t *inst_new(char *id, uint16_t dev, uint16_t key)
{
	struct web_mach_inst_t *inst;

	inst = malloc(sizeof(struct web_mach_inst_t));
	*inst = (struct web_mach_inst_t){ id, true, false, false, dev, key, NULL };

	return inst;
}

/**
 * Delete an instance.
 *   @inst: The instance.
 */
static void inst_delete(struct web_mach_inst_t *inst)
{
	free(inst->id);
	free(inst);
}

/**
 * Retrieve the index of the instance.
 *   @head: The head instance.
 *   @inst: The instance.
 *   &returns: The index.
 */
static unsigned int inst_idx(struct web_mach_t *mach, struct web_mach_inst_t *inst)
{
	unsigned int idx = 0;
	struct web_mach_inst_t *iter;

	for(iter = mach->inst; iter != inst; iter = iter->next)
		idx++;

	return idx;
}

/**
 * Retrieve the instance at a given index.
 *   @mach: The machine.
 *   @idx: The index.
 *   &returns: The instance or null.
 */
static struct web_mach_inst_t *inst_get(struct web_mach_t *mach, unsigned int idx)
{
	struct web_mach_inst_t *inst = mach->inst;

	while((idx-- > 0) && (inst != NULL))
		inst = inst->next;

	return inst;
}
