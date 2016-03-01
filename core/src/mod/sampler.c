#include "../common.h"


/**
 * Play structure.
 *   @buf: The buffer.
 *   @vol: The volume and decay rate.
 *   @idx: The index.
 */

struct play_t {
	struct dsp_buf_t *buf;
	double vol, decay;
	int idx;
};

/**
 * Sampler structure.
 *   @head, tail: The head and tail instances.
 *   @n: The number of players.
 *   @play: The player array.
 */

struct amp_sampler_t {
	struct inst_t *head, *tail;

	unsigned int n;
	struct play_t play[];
};

/**
 * Instance structure.
 *   @file: The file cache.
 *   @buf: The sample buffer.
 *   @key: The key handler.
 *   @prev, next: The previous and next instances.
 */

struct inst_t {
	struct amp_file_t *file;
	struct dsp_buf_t *buf;
	struct amp_key_t key;

	struct inst_t *prev, *next;
};


/*
 * global variables
 */

const struct amp_module_i amp_sampler_iface = {
	(amp_info_f)amp_sampler_info,
	(amp_module_f)amp_sampler_proc,
	(amp_copy_f)amp_sampler_copy,
	(amp_delete_f)amp_sampler_delete
};


/**
 * Create a sampler.
 *   @n: The number of simultaneous samples.
 *   &returns: The sampler.
 */

struct amp_sampler_t *amp_sampler_new(unsigned int n)
{
	unsigned int i;
	struct amp_sampler_t *sampler;

	sampler = malloc(sizeof(struct amp_sampler_t) + n * sizeof(struct play_t));
	sampler->head = sampler->tail = NULL;
	sampler->n = n;

	for(i = 0; i < n; i++)
		sampler->play[i].buf = NULL;

	return sampler;
}

/**
 * Copy a sampler.
 *   @sampler: The original sampler.
 *   &returns: The copied sampler.
 */

struct amp_sampler_t *amp_sampler_copy(struct amp_sampler_t *sampler)
{
	return amp_sampler_new(sampler->n);
}

/**
 * Delete a sampler.
 *   @sampler: The sampler.
 */

void amp_sampler_delete(struct amp_sampler_t *sampler)
{
	struct inst_t *cur, *next;

	for(cur = sampler->head; cur != NULL; cur = next) {
		next = cur->next;

		amp_file_unref(cur->file);
		free(cur);
	}

	free(sampler);
}


/**
 * Create a sampler from a value.
 *   @value: The value.
 *   @env: The environment.
 *   @err: The error.
 *   &returns: The value or null.
 */

struct ml_value_t *amp_sampler_make(struct ml_value_t *value, struct ml_env_t *env, char **err)
{
#undef fail
#define fail(format, ...) do { *err = amp_printf(format ?: "Type error. Expected '[(Key,string)]'.", ##__VA_ARGS__); amp_sampler_delete(sampler); ml_value_delete(value); return NULL; } while(0);

	struct ml_link_t *link;
	struct amp_sampler_t *sampler;
	struct amp_cache_t *cache = amp_core_cache(env);

	sampler = amp_sampler_new(32);

	if(value->type != ml_value_list_e)
		fail(NULL);

	for(link = value->data.list.head; link != NULL; link = link->next) {
		struct ml_tuple_t tuple;
		struct amp_key_t key;
		struct amp_file_t *file;

		if(link->value->type != ml_value_tuple_e)
			fail(NULL);

		tuple = link->value->data.tuple;
		if(tuple.len != 2)
			fail(NULL);

		if(!amp_key_scan(&key, tuple.value[0]))
			fail(NULL);

		if(tuple.value[1]->type != ml_value_str_e)
			fail(NULL);

		file = amp_cache_open(cache, tuple.value[1]->data.str, 0);
		if(file == NULL)
			fail("Cannot open sample '%s'.", tuple.value[1]->data.str);

		amp_sampler_append(sampler, key, file);
	}

	ml_value_delete(value);

	return amp_pack_module((struct amp_module_t){ sampler, &amp_sampler_iface });
}


/**
 * Append a sample to the sampler.
 *   @sampler: The sampler.
 *   @key: Consumed. The key handler.
 *   @file: Consumed. The file cache.
 */

void amp_sampler_append(struct amp_sampler_t *sampler, struct amp_key_t key, struct amp_file_t *file)
{
	struct inst_t *inst;

	inst = malloc(sizeof(struct inst_t));
	inst->prev = inst->next = NULL;
	inst->file = file;
	inst->buf = amp_file_buf(file);
	inst->key = key;

	inst->prev = sampler->tail;
	inst->next = NULL;
	*(sampler->tail ? &sampler->tail->next : &sampler->head) = inst;
	sampler->tail = inst;
}


/**
 * Handle information on a sampler.
 *   @sampler: The sampler.
 *   @info: The information.
 */

void amp_sampler_info(struct amp_sampler_t *sampler, struct amp_info_t info)
{
	unsigned int i;

	for(i = 0; i < sampler->n; i++) {
	}
}

/**
 * Process a sampler.
 *   @sampler: The sampler.
 *   @buf: The buffer.
 *   @time: The time.
 *   @len: The length.
 *   &returns: The continuation flag.
 */

bool amp_sampler_proc(struct amp_sampler_t *sampler, double *buf, struct amp_time_t *time, unsigned int len)
{
	bool cont = false;
	unsigned int i;

	for(i = 0; i < sampler->n; i++) {
		if(sampler->play[i].buf == NULL)
			continue;

		cont = true;
	}

	return  cont;
}
