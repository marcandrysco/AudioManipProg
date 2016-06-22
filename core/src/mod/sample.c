#include "../common.h"


/**
 * Play structure.
 *   @buf: The buffer.
 *   @vol: The volume.
 *   @idx: The index.
 */
struct play_t {
	struct dsp_buf_t *buf;
	double vol;
	int idx;
};

/**
 * Sample structure.
 *   @decay: The decay rate.
 *   @len: The velocity array length.
 *   @vel: The velocity array.
 *   @i, n: The current index and number of players.
 *   @play: The player array.
 */
struct amp_sample_t {
	double decay;

	unsigned int len;
	struct amp_sample_vel_t *vel;

	unsigned int i, n;
	struct play_t play[];
};

/**
 * Sample velocity.
 *   @rr, len: The current round-robin and length.
 *   @inst: The instance.
 */
struct amp_sample_vel_t {
	unsigned int rr, len;
	struct amp_sample_inst_t *inst;
};

/**
 * Sample instance structure.
 *   @file: The file.
 *   @buf: The buffer.
 */
struct amp_sample_inst_t {
	struct amp_file_t *file;
	struct dsp_buf_t *buf;
};


/*
 * global variables
 */
const struct amp_module_i amp_sample_iface = {
	(amp_info_f)amp_sample_info,
	(amp_module_f)amp_sample_proc,
	(amp_copy_f)amp_sample_copy,
	(amp_delete_f)amp_sample_delete
};


/**
 * Create a sample.
 *   @n: The number of simultaneous samples.
 *   @decay: The decay rate.
 *   &returns: The sample.
 */
struct amp_sample_t *amp_sample_new(unsigned int n, double decay)
{
	unsigned int i;
	struct amp_sample_t *sample;

	sample = malloc(sizeof(struct amp_sample_t) + n * sizeof(struct play_t));
	sample->len = 0;
	sample->vel = malloc(0);
	sample->i = 0;
	sample->n = n;
	sample->decay = decay;

	for(i = 0; i < n; i++)
		sample->play[i].idx = INT_MAX;

	return sample;
}

/**
 * Copy a sample.
 *   @sample: The original sample.
 *   &returns: The copied sample.
 */
struct amp_sample_t *amp_sample_copy(struct amp_sample_t *sample)
{
	struct amp_sample_t *copy;
	unsigned int i, j;
	
	copy = amp_sample_new(sample->n, sample->decay);

	for(i = 0; i < sample->len; i++) {
		struct amp_sample_vel_t *vel = amp_sample_vel(copy);

		for(j = 0; j < sample->vel[i].len; j++)
			amp_sample_inst(vel, amp_file_copy(sample->vel[i].inst[j].file));
	}

	return copy;
}

/**
 * Delete a sample.
 *   @sample: The sample.
 */
void amp_sample_delete(struct amp_sample_t *sample)
{
	unsigned int i, j;

	for(i = 0; i < sample->len; i++) {
		struct amp_sample_vel_t *vel = &sample->vel[i];

		for(j = 0; j < vel->len; j++)
			amp_file_unref(vel->inst[j].file);

		free(vel->inst);
	}

	free(sample->vel);
	free(sample);
}


/**
 * Create a sample from a value.
 *   @ret: Ref. The return value.
 *   @value: The value.
 *   @env: The environment.
 *   &returns: Error
 */
char *amp_sample_make(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env)
{
#define onexit if(sample != NULL) amp_sample_delete(sample);
#define error() fail("%C: Type error. Expected (Int,Float,[[String]]).", ml_tag_chunk(&value->tag))
	struct ml_list_t *list;
	struct ml_link_t *link, *inst;
	struct amp_sample_t *sample = NULL;
	struct amp_cache_t *cache = amp_core_cache(env);

	if(value->type != ml_value_tuple_v)
		error();

	list = value->data.list;
	if(list->len != 3)
		error();

	if((ml_list_getv(list, 0)->type != ml_value_num_v) || !ml_value_isnum(ml_list_getv(list, 1)) || (ml_list_getv(list, 2)->type != ml_value_list_v))
		error();

	sample = amp_sample_new(ml_list_getv(list, 0)->data.num, ml_value_getflt(ml_list_getv(list, 1)));

	for(link = ml_list_getv(list, 2)->data.list->head; link != NULL; link = link->next) {
		struct amp_file_t *file;
		struct amp_sample_vel_t *vel;

		if(link->value->type != ml_value_list_v)
			error();

		vel = amp_sample_vel(sample);

		for(inst = link->value->data.list->head; inst != NULL; inst = inst->next) {
			if(inst->value->type != ml_value_str_v)
				error();

			file = amp_cache_open(cache, inst->value->data.str, 0, amp_core_rate(env));
			if(file == NULL)
				fail("Cannot open '%s'.", inst->value->data.str);

			amp_sample_inst(vel, file);
		}
	}

	*ret = amp_pack_module((struct amp_module_t){ sample, &amp_sample_iface });

	return NULL;
#undef onexit
}


/**
 * Handle information on a sample.
 *   @sample: The sample.
 *   @info: The information.
 */
void amp_sample_info(struct amp_sample_t *sample, struct amp_info_t info)
{
}

/**
 * Process a sample.
 *   @sample: The sample.
 *   @buf: The buffer.
 *   @time: The time.
 *   @len: The length.
 *   @queue: The action queue.
 *   &returns: The continuation flag.
 */
bool amp_sample_proc(struct amp_sample_t *sample, double *buf, struct amp_time_t *time, unsigned int len, struct amp_queue_t *queue)
{
	bool cont = false;
	unsigned int i, j, n = 0;
	struct amp_event_t *event;

	for(i = 0; i < len; i++) {
		while((event = amp_queue_event(queue, &n, i)) != NULL) {
			unsigned int n;
			struct amp_sample_vel_t *vel;

			if(event->val == 0)
				continue;

			n = ((float)event->val / (float)UINT16_MAX) * sample->len;
			vel = &sample->vel[(n >= sample->len) ? (sample->len - 1) : n];

			sample->play[sample->i].buf = vel->inst[vel->rr].buf;
			sample->play[sample->i].vol = 1.0;
			sample->play[sample->i].idx = 0;
			sample->play[(sample->i + sample->n - 1) % sample->n].vol *= sample->decay;

			sample->i = (sample->i + 1) % sample->n;
			vel->rr = (vel->rr + 1) % vel->len;
		}

		buf[i] = 0.0;

		for(j = 0; j < sample->n; j++) {
			struct play_t *play = &sample->play[j];

			if((play->idx == INT_MAX) || (play->idx >= play->buf->len))
				continue;

			cont = true;
			buf[i] += play->vol * play->buf->arr[play->idx];
			play->idx++;

			if(play->vol < 1.0)
				play->vol *= sample->decay;
		}
	}

	return cont;
}


/**
 * Add a velocity to the sample.
 *   @sample: The sample.
 *   &returns: The velocity.
 */
struct amp_sample_vel_t *amp_sample_vel(struct amp_sample_t *sample)
{
	struct amp_sample_vel_t *vel;

	sample->vel = realloc(sample->vel, (sample->len + 1) * sizeof(struct amp_sample_vel_t));
	vel = &sample->vel[sample->len++];
	vel->rr = 0;
	vel->len = 0;
	vel->inst = malloc(0);

	return vel;
}

/**
 * Add an instance to the velocity.
 *   @vel: The velocity.
 *   @file: The file.
 */
void amp_sample_inst(struct amp_sample_vel_t *vel, struct amp_file_t *file)
{
	struct amp_sample_inst_t *inst;

	vel->inst = realloc(vel->inst, (vel->len + 1) * sizeof(struct amp_sample_inst_t));
	inst = &vel->inst[vel->len++];
	inst->file = file;
	inst->buf = amp_file_buf(file);
}
