#ifndef HANLDER_CTRL_H
#define HANLDER_CTRL_H

/*
 * control declarations
 */

struct amp_ctrl_t;

extern struct amp_handler_i amp_ctrl_iface;

struct amp_ctrl_t *amp_ctrl_new(double val, double low, double high, bool exp, uint16_t dev, uint16_t key);
struct amp_ctrl_t *amp_ctrl_copy(struct amp_ctrl_t *ctrl);
void amp_ctrl_delete(struct amp_ctrl_t *ctrl);

struct ml_value_t *amp_ctrl_make(struct ml_value_t *value, struct ml_env_t *env, char **err);

double amp_ctrl_proc(struct amp_ctrl_t *ctrl, struct amp_event_t event);


/**
 * Retrieve the handler structure for a control handler.
 *   @ctrl: The control handler.
 *   &returns: The handler structure.
 */

static inline struct amp_handler_t amp_ctrl_handler(struct amp_ctrl_t *ctrl)
{
	return (struct amp_handler_t){ ctrl, &amp_ctrl_iface };
}

#endif
