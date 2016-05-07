#include "../common.h"


/*
 * global variables
 */
struct amp_effect_t amp_effect_null = { NULL, NULL };

struct amp_effect_t amp_effect_copy(struct amp_effect_t effect)
{
	return (struct amp_effect_t){ effect.iface->copy(effect.ref), effect.iface };
}
