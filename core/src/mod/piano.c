#include "../common.h"


/**
 * Piano velocity structure.
 *   @idx: The playback index.
 *   @n: The number or round-robins.
 *   @rr: The round-robin array.
 */
struct amp_piano_vel_t {
	int idx;
	unsigned int n;
	struct amp_file_t **rr;
};

/**
 * Piano key structure.
 *   @idx, n: The play index and number of velocities.
 *   @vel: The velocity array.
 */
struct amp_piano_key_t {
	unsigned int idx, n;
	struct amp_piano_vel_t *vel;
};

/**
 * Piano structure.
 *   @mul, rate: The decay multiplier and sample rate.
 *   @dev, pedal: The device and pedal key.
 *   @key: The keys array.
 *   @n: The play count.
 *   @play: The playback array.
 */
struct amp_piano_t {
	float mul, rate;
	uint16_t dev, pedal;
	struct amp_piano_key_t key[128];

	unsigned int n;
	struct amp_play_t *play;
};


/*
 * global variables
 */
const struct amp_module_i amp_piano_iface = {
	(amp_info_f)amp_piano_info,
	(amp_module_f)amp_piano_proc,
	(amp_copy_f)amp_piano_copy,
	(amp_delete_f)amp_piano_delete
};


/**
 * Create a piano.
 *   @dev: The device.
 *   @simul: The number of simultaneous playback.
 *   @rate: The sample rate.
 *   &returns: The piano.
 */
struct amp_piano_t *amp_piano_new(uint16_t dev, unsigned int simul, float rate)
{
	struct amp_piano_t *piano;
	unsigned int i;

	piano = malloc(sizeof(struct amp_piano_t));
	piano->dev = dev;
	piano->pedal = 128;
	piano->n = simul;
	piano->mul = 0.0f;
	piano->rate = rate;
	piano->play = malloc(simul * sizeof(struct amp_play_t));
	amp_play_init(piano->play, simul);

	for(i = 0; i < 128; i++)
		piano->key[i] = (struct amp_piano_key_t){ 0, 0, malloc(0) };

	return piano;
}

/**
 * Copy a piano.
 *   @piano: The original piano.
 *   &returns: The piano.
 */
struct amp_piano_t *amp_piano_copy(struct amp_piano_t *piano)
{
	struct amp_piano_t *copy;
	unsigned int i, j, k;
	struct amp_piano_vel_t *vel;
	
	copy = amp_piano_new(piano->dev, piano->n, piano->rate);
	copy->mul = piano->mul;
	copy->pedal = piano->pedal;

	for(i = 0; i < 128; i++) {
		for(j = 0; j < piano->key[i].n; j++) {
			vel = amp_piano_vel(&copy->key[i]);

			for(k = 0; k < piano->key[i].vel[j].n; k++)
				amp_piano_rr(vel, amp_file_copy(piano->key[i].vel[j].rr[k]));
		}
	}

	return copy;
}

/**
 * Delete a piano.
 *   @piano: The piano.
 */
void amp_piano_delete(struct amp_piano_t *piano)
{
	unsigned int i, j, k;

	for(i = 0; i < 128; i++) {
		for(j = 0; j < piano->key[i].n; j++) {
			for(k = 0; k < piano->key[i].vel[j].n; k++)
				amp_file_delete(piano->key[i].vel[j].rr[k]);

			free(piano->key[i].vel[j].rr);
		}

		free(piano->key[i].vel);
	}

	free(piano->play);
	free(piano);
}


/**
 * Create a piano from a value.
 *   @value: The value.
 *   @env: The environment.
 *   @err: The error.
 *   &returns: The value or null.
 */
char *amp_piano_make(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env)
{
#define onexit if(piano != NULL) amp_piano_delete(piano);
	int dev, idx = 0;
	struct ml_link_t *iter1, *iter2, *link;
	struct ml_value_t *list, *opt;
	struct amp_piano_t *piano = NULL;
	struct amp_piano_key_t *key;
	struct amp_piano_vel_t *vel;
	struct amp_cache_t *cache = amp_core_cache(env);

	chkfail(amp_match_unpack(value, "(d,O,O)", &dev, &list, &opt));
	piano = amp_piano_new(dev, 24, amp_core_rate(env));

	if(list->type != ml_value_list_v)
		fail("%C: Type mismatch.", ml_tag_chunk(&value->tag));

	for(iter1 = list->data.list->head; iter1 != NULL; iter1 = iter1->next, idx++) {
		if(iter1->value->type == ml_value_nil_v)
			continue;
		else if(iter1->value->type != ml_value_list_v)
			fail("%C: Type mismatch.", ml_tag_chunk(&value->tag));

		if((idx < 0) || (idx >= 128))
			fail("%C: Too many keys.", ml_tag_chunk(&value->tag));

		key = amp_piano_key(piano, idx);

		for(iter2 = iter1->value->data.list->head; iter2 != NULL; iter2 = iter2->next) {
			if(iter2->value->type != ml_value_list_v)
				fail("%C: Type mismatch.", ml_tag_chunk(&value->tag));

			vel = amp_piano_vel(key);

			for(link = iter2->value->data.list->head; link != NULL; link = link->next) {
				struct amp_file_t *file;

				if(link->value->type != ml_value_str_v)
					fail("%C: Type mismatch.", ml_tag_chunk(&value->tag));

				file = amp_cache_open(cache, link->value->data.str, 0, amp_core_rate(env));
				if(file == NULL)
					fail("%C: Unable to open file '%s'.", ml_tag_chunk(&value->tag), link->value->data.str);

				amp_piano_rr(vel, file);
			}
		}
	}

	ml_value_delete(list);
	ml_value_delete(opt);

	*ret = amp_pack_module(amp_module(piano, &amp_piano_iface));
	return NULL;
#undef onexit
}


/**
 * Retrieve the key given the key number.
 *   @piano: The piano.
 *   @num: The key number.
 *   &returns: The key or null.
 */
struct amp_piano_key_t *amp_piano_key(struct amp_piano_t *piano, uint16_t num)
{
	return &piano->key[num];
}

/**
 * Add a velocity to the key.
 *   @key: The key.
 *   &returns: The velocity.
 */
struct amp_piano_vel_t *amp_piano_vel(struct amp_piano_key_t *key)
{
	key->vel = realloc(key->vel, (key->n + 1) * sizeof(struct amp_piano_vel_t));
	key->vel[key->n] = (struct amp_piano_vel_t){ 0, 0, malloc(0) };

	return &key->vel[key->n++];
}

/**
 * Add a round-robin to the velocity.
 *   @vel: The velocity.
 *   @file: Consumed. The file.
 */
void amp_piano_rr(struct amp_piano_vel_t *vel, struct amp_file_t *file)
{
	vel->rr = realloc(vel->rr, (vel->n + 1) * sizeof(void *));
	vel->rr[vel->n] = file;
	vel->n++;
}


/**
 * Process information on the piano.
 *   @info: The information.
 */
void amp_piano_info(struct amp_piano_t *piano, struct amp_info_t info)
{
}

/**
 * Process a piano.
 *   @piano: The piano.
 *   @buf: The buffer.
 *   @time: The time.
 *   @len: The length.
 *   @queue: The action queue.
 *   &returns: The continuation flag.
 */
bool amp_piano_proc(struct amp_piano_t *piano, double *buf, struct amp_time_t *time, unsigned int len, struct amp_queue_t *queue)
{
	unsigned int i, n = 0;
	struct amp_event_t *event;

	dsp_zero_d(buf, len);

	for(i = 0; i < len; i++) {
		while((event = amp_queue_event(queue, &n, i)) != NULL) {
			struct amp_piano_key_t *key;

			if(event->dev != piano->dev)
				continue;
			else if(event->key >= 128)
				continue;

			key = &piano->key[event->key];
			if(key->n == 0)
				continue;

			if(event->val > 0) {
				unsigned int vel;
				struct acw_buf_t buf;

				vel = key->n;
				vel = event->val / ((UINT16_MAX + vel) / vel);

				buf = amp_file_buf(key->vel[vel].rr[0]);
				key->idx = amp_play_add(piano->play, piano->n, buf, 1.0f, piano->rate);

				printf("vel: %d %.1f\n", vel, amp_key_freq_f(event->key));
			}
			else {
				piano->play[key->idx].mul = 0.9998f; // TODO :decay constant
				key->idx = -1;
			}
		}

		buf[i] = amp_play_proc(piano->play, piano->n);
	}

	return false;
}
