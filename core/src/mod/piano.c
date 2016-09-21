#include "../common.h"


/**
 * Playback structure.
 *   @buf: The buffer.
 *   @vol: The volume.
 *   @idx: The index.
 */
struct amp_play_t {
	struct dsp_buf_t *buf;
	double vol;
	int idx;
};

/**
 * Initialize the playback.
 *   @play: The playback array.
 *   @n: The number of players.
 */
static inline void amp_play_init(struct amp_play_t *play, unsigned int n)
{
	unsigned int i;

	for(i = 0; i < n; i++)
		play[i] = (struct amp_play_t){ NULL, 0.0, 0 };
}

/**
 * Process the playback.
 *   @play: The playback array.
 *   @n: The number of players.
 *   &returns: The value.
 */
static inline double amp_play_proc(struct amp_play_t *play, unsigned int n)
{
	unsigned int i;
	double v = 0.0;

	for(i = 0; i < n; i++) {
		if(play[n].buf == NULL)
			continue;

		if(play->idx > 0)
			v += play->buf->arr[play->idx];

		play->idx++;
		if(play->idx >= play->buf->len)
			play->buf = NULL;
	}
	
	return v;
}


/**
 * Piano velocity structure.
 *   @idx, n: The current index and number or round-robins.
 *   @rr: The round-robin array.
 */
struct amp_piano_vel_t {
	unsigned int idx, n;
	struct dsp_buf_t **rr;
};

/**
 * Piano key structure.
 *   @n: The number of velocities.
 *   @vel: The velocity array.
 */
struct amp_piano_key_t {
	unsigned int n;
	struct amp_piano_vel_t *vel;
};

/**
 * Piano structure.
 *   @dev, pedal: The device and pedal key.
 *   @key: The keys array.
 *   @n: The play count.
 *   @play: The playback array.
 */
struct amp_piano_t {
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
 *   @pedal: The pedal key.
 *   &returns: The piano.
 */
struct amp_piano_t *amp_piano_new(uint16_t dev, uint16_t pedal)
{
	struct amp_piano_t *piano;
	unsigned int i;

	piano = malloc(sizeof(struct amp_piano_t));
	piano->dev = dev;
	piano->pedal = pedal;

	for(i = 0; i < 128; i++)
		piano->key[i] = (struct amp_piano_key_t){ 0, malloc(0) };

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
	
	copy = amp_piano_new(piano->dev, piano->pedal);

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
				dsp_buf_delete(piano->key[i].vel[j].rr[k]);

			free(piano->key[i].vel[j].rr);
		}

		free(piano->key[i].vel);
	}

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
	uint16_t pedal = 256;
	struct ml_link_t *iter1, *iter2, *link;
	struct ml_value_t *list, *opt;
	struct amp_piano_t *piano = NULL;
	struct amp_piano_key_t *key;
	struct amp_piano_vel_t *vel;

	chkfail(amp_match_unpack(value, "(d,O,O)", &dev, &list, &opt));
	piano = amp_piano_new(dev, pedal);

	if(list->type != ml_value_list_v)
		fail("%C: Type mismatch.", ml_tag_chunk(&value->tag));

	for(iter1 = list->data.list->head; iter1 != NULL; iter1 = iter1->next) {
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
				if(link->value->type != ml_value_str_v)
					fail("%C: Type mismatch.", ml_tag_chunk(&value->tag));

				amp_piano_rr(vel, dsp_buf_load(link->value->data.str, 0, amp_core_rate(env)));
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
	key->vel[key->n] = (struct amp_piano_vel_t){ 0, 0, NULL };

	return &key->vel[key->n++];
}

/**
 * Add a round-robin to the velocity.
 *   @vel: The velocity.
 *   @buf: Consumed. The buffer.
 */
void amp_piano_rr(struct amp_piano_vel_t *vel, struct dsp_buf_t *buf)
{
	vel->rr = realloc(vel->rr, (vel->n + 1) * sizeof(void *));
	vel->rr[vel->n] = buf;
}



void amp_piano_info(struct amp_piano_t *piano, struct amp_info_t info)
{
}

bool amp_piano_proc(struct amp_piano_t *piano, double *buf, struct amp_time_t *time, unsigned int len, struct amp_queue_t *queue)
{
	return false;
}
