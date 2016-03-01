#ifndef MIDI_H
#define MIDI_H

/**
 * Key handler structure.
 *   @exp: Exponential interpolation.
 *   @min, max: The minimum and maximum velcoity.
 *   @dev, id: The device and key identifier.
 */

struct amp_key_t {
	bool exp;
	double min, max;
	uint16_t dev, id;
};

/*
 * key handler declarations
 */

struct amp_key_t amp_key_init(double min, double max, bool exp, uint16_t dev, uint16_t id);
bool amp_key_scan(struct amp_key_t *key, struct ml_value_t *value);


static inline bool amp_key_proc(struct amp_key_t *key, struct amp_action_t *action, double *vel)
{
	if((action->event.dev != key->dev) || (action->event.key != key->id))
		return false;

	*vel = key->min + ((double)action->event.val / (double)UINT16_MAX) * (key->max - key->min);

	return true;
}

#endif
