#include "../common.h"


/**
 * Piano structure.
 *   @dev, pedal, init, len: The device, pedal, initial key, and key length.
 *   @on, arr: The pedal on flag and note array.
 */
struct amp_piano_t {
	uint16_t dev, pedal, init, len;
	uint8_t on, *arr;
};

/*
 * global variables
 */
const struct amp_seq_i amp_piano_iface = {
	(amp_info_f)amp_piano_info,
	(amp_seq_f)amp_piano_proc,
	(amp_copy_f)amp_piano_copy,
	(amp_delete_f)amp_piano_delete
};


/**
 * Create a piano from a value.
 *   @ret: Ref. The returned value.
 *   @value: The value.
 *   @env: The environment.
 *   &returns: Error.
 */
char *amp_piano_make(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env)
{
	int dev, pedal, init, len;

	chkret(amp_match_unpack(value, "(d,d,d,d)", &dev, &pedal, &init, &len));
	if((dev < 0) || (dev >= UINT16_MAX))
		return mprintf("Invalid device parameter for Piano.");
	else if((pedal < 0) || (pedal >= UINT16_MAX))
		return mprintf("Invalid pedal parameter for Piano.");
	else if((init < 0) || (init >= UINT16_MAX))
		return mprintf("Invalid initial key parameter for Piano.");
	else if((len < 0) || (len >= UINT16_MAX) || ((init + len) >= UINT16_MAX))
		return mprintf("Invalid key length parameter for Piano.");

	*ret = amp_pack_seq(amp_seq(amp_piano_new(dev, pedal, init, len), &amp_piano_iface));
	return NULL;
}


/**
 * Create a piano.
 *   @dev: The device.
 *   @pedal: The pedal key.
 *   @init: The initial key.
 *   @len: The number of keys.
 *   &returns: The piano.
 */
struct amp_piano_t *amp_piano_new(uint16_t dev, uint16_t pedal, uint16_t init, uint16_t len)
{
	unsigned int i;
	struct amp_piano_t *piano;

	piano = malloc(sizeof(struct amp_piano_t));
	piano->dev = dev;
	piano->pedal = pedal;
	piano->init = init;
	piano->len = len;
	piano->on = 0;
	piano->arr = malloc(len);

	for(i = 0; i < len; i++)
		piano->arr[i] = 0;

	return piano;
}

/**
 * Copy a piano.
 *   @piano: The original piano.
 *   &returns: The piano.
 */
struct amp_piano_t *amp_piano_copy(struct amp_piano_t *piano)
{
	return amp_piano_new(piano->dev, piano->pedal, piano->init, piano->len);
}

/**
 * Delete a piano.
 *   @piano: The piano.
 */
void amp_piano_delete(struct amp_piano_t *piano)
{
	free(piano->arr);
	free(piano);
}


/**
 * Process information on a piano.
 *   @piano: The piano.
 *   @queue: The queue.
 *   @time: The time.
 *   @len: The length.
 */
void amp_piano_info(struct amp_piano_t *piano, struct amp_info_t info)
{
}

/**
 * Process a piano.
 *   @piano: The piano.
 *   @time: The time.
 *   @len: The length.
 *   @queue: The action queue.
 */
void amp_piano_proc(struct amp_piano_t *piano, struct amp_time_t *time, unsigned int len, struct amp_queue_t *queue)
{
	unsigned int i, n = 0;
	struct amp_action_t *action;

	for(i = 0; i < len; i++) {
		while((action = amp_queue_get(queue, n, i)) != NULL) {
			uint16_t key, n;

			if(action->event.dev != piano->dev)
				continue;

			key = action->event.key;
			n = key - piano->init;
			if(key == piano->pedal) {
			}
			else if((key >= piano->init) && (n < piano->len)) {
				if(action->event.val > 0)
					piano->arr[n] |= 0x1;
				else
					piano->arr[n] &= ~0x1;
			}
		}
	}
}
