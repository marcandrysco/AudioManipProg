#include "../common.h"


/**
 * Play structure.
 *   @vol: The volume;
 *   @idx: The index.
 */

struct play_t {
	double vol;
	int idx;
};

/**
 * Sample structure.
 *   @file: The file.
 *   @buf: The buffer.
 *   @key: The key.
 *   @decay: The decay rate.
 *   @n: The number of players.
 *   @play: The player array.
 */

struct amp_sample_t {
	struct amp_file_t *file;
	struct dsp_buf_t *buf;
	struct amp_key_t key;
	double decay;

	unsigned int n;
	struct play_t play[];
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
 *   @file: The file.
 *   @n: The number of simultaneous samples.
 *   @decay: The decay rate.
 *   &returns: The sample.
 */

struct amp_sample_t *amp_sample_new(struct amp_file_t *file, unsigned int n, double decay, struct amp_key_t key)
{
	unsigned int i;
	struct amp_sample_t *sample;

	sample = malloc(sizeof(struct amp_sample_t) + n * sizeof(struct play_t));
	sample->file = file;
	sample->buf = amp_file_buf(file);
	sample->n = n;
	sample->decay = decay;
	sample->key = key;

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
	return amp_sample_new(amp_file_copy(sample->file), sample->n, sample->decay, sample->key);
}

/**
 * Delete a sample.
 *   @sample: The sample.
 */

void amp_sample_delete(struct amp_sample_t *sample)
{
	amp_file_unref(sample->file);
	free(sample);
}


/**
 * Create a sample from a value.
 *   @value: The value.
 *   @env: The environment.
 *   @err: The error.
 *   &returns: The value or null.
 */

struct ml_value_t *amp_sample_make(struct ml_value_t *value, struct ml_env_t *env, char **err)
{
#undef fail
#define fail(format, ...) do { *err = amp_printf(format ?: "Type error. Expected '(string,int,num,Key)'.", ##__VA_ARGS__); ml_value_delete(value); return NULL; } while(0);

	struct ml_tuple_t tuple;
	struct amp_key_t key;
	struct amp_file_t *file;
	struct amp_sample_t *sample;
	struct amp_cache_t *cache = amp_core_cache(env);

	if(value->type != ml_value_tuple_e)
		fail(NULL);

	tuple = value->data.tuple;
	if(tuple.len != 4)
		fail(NULL);

	if((tuple.value[0]->type != ml_value_str_e) || (tuple.value[1]->type != ml_value_num_e) || (tuple.value[2]->type != ml_value_num_e))
		fail(NULL);

	if(!amp_key_scan(&key, tuple.value[3]))
		fail(NULL);

	file = amp_cache_open(cache, tuple.value[0]->data.str, 0);
	if(file == NULL)
		fail("Cannot open sample '%s'.", tuple.value[1]->data.str);

	sample = amp_sample_new(file, tuple.value[1]->data.num, tuple.value[1]->data.num, key);;
	ml_value_delete(value);

	return amp_pack_module((struct amp_module_t){ sample, &amp_sample_iface });
}


/**
 * Handle information on a sample.
 *   @sample: The sample.
 *   @info: The information.
 */

void amp_sample_info(struct amp_sample_t *sample, struct amp_info_t info)
{
	if(info.type == amp_info_action_e) {
		double vel;
		unsigned int i;

		if(!amp_key_proc(&sample->key, info.data.action, &vel))
			return;

		for(i = 0; i < sample->n; i++) {
			if(sample->play[i].idx == INT_MAX)
				break;
		}

		if(i == sample->n)
			return;

		sample->play[i].idx = -info.data.action->delay;
	}
}

/**
 * Process a sample.
 *   @sample: The sample.
 *   @buf: The buffer.
 *   @time: The time.
 *   @len: The length.
 *   &returns: The continuation flag.
 */

bool amp_sample_proc(struct amp_sample_t *sample, double *buf, struct amp_time_t *time, unsigned int len)
{
	int idx;
	unsigned int i, j;

	for(j = 0; j < sample->n; j++) {
		struct play_t *play = &sample->play[j];

		if(play->idx == INT_MAX)
			continue;

		idx = play->idx;

		for(i = 0; i < len; i++, idx++) {
			if(idx >= (int)sample->buf->len)
				break;
			else if(idx < 0)
				continue;

			buf[i] += sample->buf->arr[idx];
		}

		play->idx = (idx < (int)sample->buf->len) ? idx : INT_MAX;
	}

	return false;
}
