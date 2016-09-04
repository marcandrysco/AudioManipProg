#include "common.h"


/**
 * Instance structure.
 *   @key, vel: The key and velocity.
 *   @delay: The delay.
 *   @next: The next instance.
 */
struct inst_t {
	uint16_t key, vel;
	unsigned int delay;

	struct inst_t *next;
};

/**
 * Machine structure.
 *   @id: The indetifier.
 *   @last: The last beat.
 *   @ndivs, nbeats, nbats: The number of divisions, beats and bars.
 *   @inst: The instance array.
 */
struct web_mach_t {
	const char *id;

	double last;
	unsigned int ndivs, nbeats, nbars;
	struct inst_t **inst;
};


/*
 * local declarations
 */
static void mach_proc(struct io_file_t file, void *arg);


/**
 * Create a machine.
 *   @id: The identifier.
 */
	#include <sys/stat.h>
struct web_mach_t *web_mach_new(const char *id)
{
	struct web_mach_t *mach;
	unsigned int i, len;

	len = 4 * 4 * 2;
	mach = malloc(sizeof(struct web_mach_t));
	mach->last = 0.0;
	mach->id = id;
	mach->ndivs = 4;
	mach->nbeats = 4;
	mach->nbars = 2;
	mach->inst = malloc(len * sizeof(void *));

	for(i = 0; i < len; i++)
		mach->inst[i] = NULL;

	{
		char path[strlen(mach->id) + 9];

		sprintf(path, "web.dat/%s", id);
		mkdir("web.dat", 0777);
		web_mach_load(mach, path);
	}

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
	struct inst_t *inst;
	unsigned int i, len = web_mach_len(mach);

	for(i = 0; i < len; i++) {
		while(mach->inst[i] != NULL) {
			inst = mach->inst[i];
			mach->inst[i] = inst->next;
			free(inst);
		}
	}

	free(mach->inst);
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

		if((left != 0.0) && (left <= right))
			continue;

		struct inst_t *inst;
		unsigned int bar, beat, div, idx;

		bar = (unsigned int)time[i].bar % mach->nbars;
		beat = (unsigned int)time[i].beat;
		div = (time[i].beat - beat) * mach->ndivs;
		idx = div + (beat + bar * mach->nbeats) * mach->ndivs;

		for(inst = mach->inst[idx]; inst != NULL; inst = inst->next)
			amp_queue_add(queue, (struct amp_action_t){ i, { 0, inst->key+1, inst->vel }, queue });
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
 *   @idx: The index.
 *   @key: The key.
 *   @vel: The velocity.
 */
void web_mach_set(struct web_mach_t *mach, unsigned int idx, uint16_t key, uint16_t vel)
{
	struct inst_t *tmp, **inst;

	inst = &mach->inst[idx];
	while(*inst != NULL) {
		if((*inst)->key == key) {
			tmp = *inst;
			*inst = tmp->next;
			free(tmp);
		}
		else
			inst = &(*inst)->next;
	}

	if(vel > 0) {
		*inst = malloc(sizeof(struct inst_t));
		**inst = (struct inst_t){ key, vel, 0, NULL };;
	}
}


/**
 * Retrieve the information from a machine.
 *   @mach: The machine.
 *   @file: The file.
 */
void web_mach_print(struct web_mach_t *mach, struct io_file_t file)
{
	unsigned int i, len;
	struct inst_t *inst;
	const char *comma = "";

	hprintf(file, "{\"ndivs\":%u,\"nbeats\":%u,\"nbars\":%u,\"data\":[", mach->ndivs, mach->nbeats, mach->nbars);

	len = web_mach_len(mach);
	for(i = 0; i < len; i++) {
		for(inst = mach->inst[i]; inst != NULL; inst = inst->next) {
			hprintf(file, "%s{\"idx\":%u,\"key\":%u,\"vel\":%u}", comma, i, inst->key, inst->vel);
			comma = ",";
		}
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
 * Save a machine.
 *   @mach: The machine.
 *   @path: The path.
 */
void web_mach_save(struct web_mach_t *mach, const char *path)
{
	FILE *file;
	unsigned int i, len;

	file = fopen(path, "w");
	if(file == NULL)
		fatal("Failed to save to path '%s'", path);

	fprintf(file, "%u,%u,%u\n", mach->ndivs, mach->nbeats, mach->nbars);

	len = web_mach_len(mach);
	for(i = 0; i < len; i++) {
		struct inst_t *inst;

		for(inst = mach->inst[i]; inst != NULL; inst = inst->next)
			fprintf(file, "%u:%u,%u\n", i, inst->key, inst->vel);
	}

	fclose(file);
}

char *web_mach_load(struct web_mach_t *mach, const char *path)
{
#define onexit fclose(file);
	FILE *file;

	file = fopen(path, "r");
	if(file == NULL)
		return mprintf("Failed to open machine at '%s'.", path);

	if(fscanf(file, "%u,%u,%u\n", &mach->ndivs, &mach->nbeats, &mach->nbars) < 3)
		fail("Invalid machine file.");

	while(true) {
		unsigned int idx, key, vel;

		if(fscanf(file, "%u:%u,%u\n", &idx, &key, &vel) < 3)
			break;

		if((idx >= web_mach_len(mach)) || (key > UINT16_MAX) || (vel > UINT16_MAX))
			continue;

		web_mach_set(mach, idx, key, vel);
	}

	fclose(file);

	return NULL;
#undef onexit
}


/**
 * Handle a machine requeust.
 *   @mach: The machine.
 *   @path: The path.
 *   @args: The arguments.
 *   &returns: True if handled.
 */
bool web_mach_req(struct web_mach_t *mach, const char *path, struct http_args_t *args)
{
	unsigned int len, idx, key, vel, n;

	if(sscanf(path, "/set/%u/%u/%u%n", &idx, &key, &vel, &n) >= 3) {
		len = web_mach_len(mach);
		if((idx >= len) || (key > UINT16_MAX) || (vel > UINT16_MAX))
			return false;

		struct inst_t *tmp, **inst;

		inst = &mach->inst[idx];
		while(*inst != NULL) {
			if((*inst)->key == key) {
				tmp = *inst;
				*inst = tmp->next;
				free(tmp);
			}
			else
				inst = &(*inst)->next;
		}

		if(vel > 0) {
			*inst = malloc(sizeof(struct inst_t));
			**inst = (struct inst_t){ key, vel, 0, NULL };;
		}

		http_head_add(&args->resp, "Content-Type", "application/json");
		hprintf(args->file, "\"ok\"");

		{
			char path[strlen(mach->id) + 9];

			sprintf(path, "web.dat/%s", mach->id);
			mkdir("web.dat", 0777);
			web_mach_save(mach, path);
		}

		return true;
	}
	else
		return false;
}
