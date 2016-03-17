#include "../common.h"


/**
 * One shot structure.
 *   @dev, key: The device and key.
 *   @module: The module.
 *   @inst: The instance array.
 */

struct amp_shot_t {
	uint16_t dev, key;
	struct amp_module_t module;
};


/*
 * global variables
 */

const struct amp_module_i amp_shot_iface = {
	(amp_info_f)amp_shot_info,
	(amp_module_f)amp_shot_proc,
	(amp_copy_f)amp_shot_copy,
	(amp_delete_f)amp_shot_delete
};


/**
 * Create a one shot.
 *   @dev: The device.
 *   @key: The key.
 *   @module: Consumed. The module.
 *   &returns: The one shot.
 */

struct amp_shot_t *amp_shot_new(uint16_t dev, uint16_t key, struct amp_module_t module)
{
	struct amp_shot_t *shot;

	shot = malloc(sizeof(struct amp_shot_t));
	shot->dev = dev;
	shot->key = key;
	shot->module = module;

	return shot;
}

/**
 * Copy a one shot.
 *   @shot: The original one shot.
 *   &returns: The copied one shot.
 */

struct amp_shot_t *amp_shot_copy(struct amp_shot_t *shot)
{
	return amp_shot_new(shot->dev, shot->key, amp_module_copy(shot->module));
}

/**
 * Delete a one shot.
 *   @shot: The one shot.
 */

void amp_shot_delete(struct amp_shot_t *shot)
{
	amp_module_delete(shot->module);
	free(shot);
}


/**
 * Create a one shot from a value.
 *   @value: The value.
 *   @env: The environment.
 *   @err: The error.
 *   &returns: The value or null.
 */

struct ml_value_t *amp_shot_make(struct ml_value_t *value, struct ml_env_t *env, char **err)
{
	int dev, key;
	struct amp_module_t module;

	dev =1;
	*err = amp_match_unpack(value, "(d,M)", &key, &module);
	if(*err != NULL)
		return NULL;

	return amp_pack_module((struct amp_module_t){ amp_shot_new(dev, key, module), &amp_shot_iface });
}


/**
 * Handle information on a one shot.
 *   @shot: The one shot.
 *   @info: The information.
 */

void amp_shot_info(struct amp_shot_t *shot, struct amp_info_t info)
{
	if(info.type == amp_info_note_e) {
		if(info.data.note->key != shot->key)
			return;
	}

	amp_module_info(shot->module, info);
}

/**
 * Process a one shot.
 *   @shot: The one shot.
 *   @buf: The buffer.
 *   @time: The time.
 *   @len: The length.
 *   &returns: The continuation flag.
 */

bool amp_shot_proc(struct amp_shot_t *shot, double *buf, struct amp_time_t *time, unsigned int len)
{
	return amp_module_proc(shot->module, buf, time, len);
}
