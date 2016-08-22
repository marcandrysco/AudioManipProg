#include "../common.h"


/**
 * Oscillator structure.
 *   @reset: Reset flag.
 *   @type: The type.
 *   @phase: The phase.
 */
struct amp_osc_t {
	bool reset;
	enum amp_osc_e type;
	struct amp_module_t phase;
};


/*
 * global variables
 */
const struct amp_module_i amp_osc_iface = {
	(amp_info_f)amp_osc_info,
	(amp_effect_f)amp_osc_proc,
	(amp_copy_f)amp_osc_copy,
	(amp_delete_f)amp_osc_delete
};


/**
 * Create an oscillator.
 *   @type: The type.
 *   @phase: Consumed. The phaseic signal.
 *   &returns: The oscillator.
 */
struct amp_osc_t *amp_osc_new(enum amp_osc_e type, struct amp_module_t phase)
{
	struct amp_osc_t *osc;

	osc = malloc(sizeof(struct amp_osc_t));
	osc->reset = true;
	osc->type = type;
	osc->phase = phase;

	return osc;
}

/**
 * Copy an oscillator.
 *   @osc: The original oscillator.
 *   &returns: The copied oscillator.
 */
struct amp_osc_t *amp_osc_copy(struct amp_osc_t *osc)
{
	return amp_osc_new(osc->type, amp_module_copy(osc->phase));
}

/**
 * Delete an oscillator.
 *   @osc: The oscillator.
 */
void amp_osc_delete(struct amp_osc_t *osc)
{
	amp_module_delete(osc->phase);
	free(osc);
}


/**
 * Create a sine oscillator from a value.
 *   @ret: Ref. The return value.
 *   @value: The value.
 *   @env: The environment.
 *   &returns: Error
 */
char *amp_sine_make(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env)
{
#define onexit
	struct amp_module_t phase;

	chkfail(amp_match_unpack(value, "M", &phase));
	
	*ret = amp_pack_module((struct amp_module_t){ amp_osc_new(amp_osc_sine_v, phase), &amp_osc_iface });
	return NULL;
#undef onexit
}

/**
 * Create a triangle oscillator from a value.
 *   @ret: Ref. The return value.
 *   @value: The value.
 *   @env: The environment.
 *   &returns: Error
 */
char *amp_tri_make(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env)
{
#define onexit
	struct amp_module_t phase;

	chkfail(amp_match_unpack(value, "M", &phase));
	
	*ret = amp_pack_module((struct amp_module_t){ amp_osc_new(amp_osc_tri_v, phase), &amp_osc_iface });
	return NULL;
#undef onexit
}

/**
 * Create a square oscillator from a value.
 *   @ret: Ref. The return value.
 *   @value: The value.
 *   @env: The environment.
 *   &returns: Error
 */
char *amp_square_make(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env)
{
#define onexit
	struct amp_module_t phase;

	chkfail(amp_match_unpack(value, "M", &phase));
	
	*ret = amp_pack_module((struct amp_module_t){ amp_osc_new(amp_osc_square_v, phase), &amp_osc_iface });
	return NULL;
#undef onexit
}

/**
 * Create an impulse oscillator from a value.
 *   @ret: Ref. The return value.
 *   @value: The value.
 *   @env: The environment.
 *   &returns: Error
 */
char *amp_impulse_make(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env)
{
#define onexit
	struct amp_module_t phase;

	chkfail(amp_match_unpack(value, "M", &phase));
	
	*ret = amp_pack_module((struct amp_module_t){ amp_osc_new(amp_osc_impulse_v, phase), &amp_osc_iface });
	return NULL;
#undef onexit
}


/**
 * Handle information on an oscillator.
 *   @osc: The oscillator.
 *   @info: The information.
 */
void amp_osc_info(struct amp_osc_t *osc, struct amp_info_t info)
{
	if((info.type == amp_info_note_e) && (info.data.note->init))
		osc->reset = true;

	amp_module_info(osc->phase, info);
}

/**
 * Process an oscillator.
 *   @osc: The oscillator.
 *   @buf: The buffer.
 *   @time: The time.
 *   @len: The length.
 *   @queue: The action queue.
 *   &returns: The continuation flag.
 */
bool amp_osc_proc(struct amp_osc_t *osc, double *buf, struct amp_time_t *time, unsigned int len, struct amp_queue_t *queue)
{
	bool cont;
	double phase[len];
	unsigned int i;

	cont = amp_module_proc(osc->phase, phase, time, len, queue);

	switch(osc->type) {
	case amp_osc_sine_v:
		for(i = 0; i < len; i++)
			buf[i] = dsp_osc_sine_f(phase[i]);

		break;

	case amp_osc_tri_v:
		for(i = 0; i < len; i++)
			buf[i] = dsp_osc_tri(phase[i]);

		break;

	case amp_osc_square_v:
		for(i = 0; i < len; i++)
			buf[i] = dsp_osc_square(phase[i]);

		break;

	case amp_osc_impulse_v:
		{
			bool reset = osc->reset;

			for(i = 0; i < len; i++) {
				buf[i] = 0.0;

				if(reset && (phase[i] < 0.25)) {
					reset = false;
					buf[i] = 1.0;
				}
				else if(!reset && (phase[i] > 0.75))
					reset = true;
			}

			osc->reset = reset;
		}

		break;
	}

	return cont;
}


/**
 * Convert a string oscillator type into a value.
 *   @str: The string.
 *   &returns: The enumerated value.
 */
int amp_osc_type(const char *str)
{
	if(strcmp(str, "sine") == 0)
		return amp_osc_sine_v;
	else if(strcmp(str, "tri") == 0)
		return amp_osc_tri_v;
	else if(strcmp(str, "square") == 0)
		return amp_osc_square_v;
	else
		return -1;
}
