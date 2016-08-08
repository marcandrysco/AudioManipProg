#include "common.h"


/**
 * Create a control handler.
 *   @init: The initial value.
 *   @low: The low value.
 *   @high: The high value.
 *   @exp: The exponential value.
 *   &returns: The control handler.
 */
struct amp_ctrl_t *amp_ctrl_new(double val, double low, double high, bool exp, uint16_t dev, uint16_t key)
{
	struct amp_ctrl_t *ctrl;

	ctrl = malloc(sizeof(struct amp_ctrl_t));
	ctrl->val = val;
	ctrl->low = low;
	ctrl->high = high;
	ctrl->exp = false;
	ctrl->dev = dev;
	ctrl->key = key;

	return ctrl;
}

/**
 * Copy a control handler.
 *   @ctrl: The original control handler.
 *   &returns: The copy.
 */
struct amp_ctrl_t *amp_ctrl_copy(struct amp_ctrl_t *ctrl)
{
	return amp_ctrl_new(ctrl->val, ctrl->low, ctrl->high, ctrl->exp, ctrl->dev, ctrl->key);
}

/**
 * Delete a control handler.
 *   @ctrl: The control handler.
 */
void amp_ctrl_delete(struct amp_ctrl_t *ctrl)
{
	free(ctrl);
}


/**
 * Create a control handler from a value.
 *   @value: The value.
 *   @env: The environment.
 *   @err: The rror.
 *   &returns: The valueor null.
 */
char *amp_ctrl_make(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env)
{
#define onexit
	int dev, key;
	double low, high, def;

	chkfail(amp_match_unpack(value, "((d,d),(d,d),d)", &dev, &key, &low, &high, &def));

	if((dev < 0) || (dev > UINT16_MAX) || (key < 0) || (key > UINT16_MAX))
		fail("Invalid device ID.");

	*ret = amp_pack_ctrl(amp_ctrl_new(def, low, high, false, dev, key));

	return NULL;
#undef onexit
}


/**
 * Process the control handler.
 *   @ctrl: The control handler.
 *   @event: The event.
 *   &returns: The updated value.
 */
double amp_ctrl_proc(struct amp_ctrl_t *ctrl, struct amp_event_t event)
{
	double val;

	printf("proc?");
	if((event.dev != ctrl->dev) || (event.key != ctrl->key))
		return ctrl->val;

	val = (double)event.val / (double)UINT16_MAX;
	if(ctrl->exp)
		ctrl->val = ctrl->low * powf(ctrl->high / ctrl->low, val);
	else
		ctrl->val = ctrl->low + (ctrl->high - ctrl->low) * val;

	return ctrl->val;
}
