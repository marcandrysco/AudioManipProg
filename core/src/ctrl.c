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
struct ml_value_t *amp_ctrl_make(struct ml_value_t *value, struct ml_env_t *env, char **err)
{
	/*
#undef fail
#define fail() do { ml_value_delete(value); *err = strdup("Type mismatch. Control constructor requires the form '(num:Default, (num:Low, num:High), (num:Device, num:Key))'."); return NULL; } while(0)

	double val, low, high, dev, key;
	struct ml_tuple_t tuple, range, instr;

	if(value->type != ml_value_tuple_v)
		fail();

	tuple = value->data.tuple;
	if(tuple.len != 3)
		fail();

	if((tuple.value[0]->type != ml_value_num_v) || (tuple.value[1]->type != ml_value_tuple_v) || (tuple.value[2]->type != ml_value_tuple_v))
		fail();

	range = tuple.value[1]->data.tuple;
	instr = tuple.value[2]->data.tuple;
	if((range.len != 2) || (instr.len != 2))
		fail();

	if((range.value[0]->type != ml_value_num_v) || (range.value[1]->type != ml_value_num_v))
		fail();

	if((instr.value[0]->type != ml_value_num_v) || (instr.value[1]->type != ml_value_num_v))
		fail();

	val = tuple.value[0]->data.num;
	low = range.value[0]->data.num;
	high = range.value[1]->data.num;
	dev = instr.value[0]->data.num;
	key = instr.value[1]->data.num;

#undef fail
#define fail(...) do { ml_value_delete(value); amp_eprintf(err, __VA_ARGS__); return NULL; } while(0)

	if((val < low) || (val > high))
		fail("Default value does not fall in range.");
	else if((dev < 0) || (dev > UINT16_MAX))
		fail("Invalid device.");
	else if((key < 0) || (key > UINT16_MAX))
		fail("Invalid key.");

	ml_value_delete(value);

	return amp_pack_ctrl(amp_ctrl_new(val, low, high, false, dev, key));
	*/
	fatal("stub");
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

	if((event.dev != ctrl->dev) || (event.key != ctrl->key))
		return ctrl->val;

	val = (double)event.val / (double)UINT16_MAX;
	if(ctrl->exp)
		ctrl->val = ctrl->low * powf(ctrl->high / ctrl->low, val);
	else
		ctrl->val = ctrl->low + (ctrl->high - ctrl->low) * val;

	return ctrl->val;
}
