#include "common.h"


/**
 * Create a key handler.
 *   @min: The minimum velocity
 *   @max: The maximum velcoity.
 *   @exp: Exponential interpolation.
 *   @dev: The device.
 *   @id: The key identifier.
 *   &returns: The key handler.
 */

struct amp_key_t amp_key_init(double min, double max, bool exp, uint16_t dev, uint16_t id)
{
	return (struct amp_key_t){ exp, min, max, dev, id };
}

/**
 * Scan a key from a value.
 *   @key: The destination key.
 *   @value: The value.
 *   &returns: True if successful, false otherwise.
 */

bool amp_key_scan(struct amp_key_t *key, struct ml_value_t *value)
{
	double min, max;
	int dev, id;

	if(amp_match_unpack(ml_value_copy(value), "((f,f),(d,d))", &min, &max, &dev, &id) != NULL)
		return false;

	*key = amp_key_init(min, max, false, dev, id);

	return true;
}
