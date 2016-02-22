#include "../common.h"


/**
 * Instance structure.
 *   @delay: The delay.
 *   @note: The note.
 *   @module: The module.
 */

struct inst_t {
	int delay;
	struct amp_note_t note;
	struct amp_module_t module;
};

/**
 * Synthesizer structure.
 *   @dev: Listening device.
 *   @n: The width.
 *   @inst: The instance array.
 */

struct amp_synth_t {
	uint16_t dev;
	unsigned int n;

	struct inst_t *inst;
};


/*
 * global variables
 */

struct amp_effect_i amp_synth_iface = {
	(amp_info_f)amp_synth_info,
	(amp_effect_f)amp_synth_proc,
	(amp_copy_f)amp_synth_copy,
	(amp_delete_f)amp_synth_delete
};


/**
 * Create a synthesizer.
 *   @n: The number of instances.
 *   @module: Consumed. The module.
 *   &returns: The synthesizer.
 */

struct amp_synth_t *amp_synth_new(uint16_t dev, unsigned int n, struct amp_module_t module)
{
	unsigned int i;
	struct amp_synth_t *synth;

	assert(n > 0);

	synth = malloc(sizeof(struct amp_synth_t));
	synth->dev = dev;
	synth->n = n;
	synth->inst = malloc(n * sizeof(struct inst_t));
	synth->inst[0] = (struct inst_t){ -1, { 0.0, 0.0 }, module };

	for(i = 1; i < synth->n; i++)
		synth->inst[i] = (struct inst_t){ -1, { 0, 0.0, 0.0 }, amp_module_copy(module) };

	return synth;
}

/**
 * Copy a synthesizer.
 *   @synth: The original synthesizer.
 *   &returns: The copied synthesizer.
 */

struct amp_synth_t *amp_synth_copy(struct amp_synth_t *synth)
{
	return amp_synth_new(synth->dev, synth->n, amp_module_copy(synth->inst[0].module));
}

/**
 * Delete a synthesizer.
 *   @synth: The synthesizer.
 */

void amp_synth_delete(struct amp_synth_t *synth)
{
	unsigned int i;

	for(i = 0; i < synth->n; i++)
		amp_module_delete(synth->inst[i].module);

	free(synth->inst);
	free(synth);
}


/**
 * Create a synthesizer from a value.
 *   @value: The value.
 *   @env: The environment.
 *   @err: The error.
 *   &returns: The value or null.
 */

struct ml_value_t *amp_synth_make(struct ml_value_t *value, struct ml_env_t *env, char **err)
{
	int dev, n;
	struct amp_module_t module;

	*err = amp_match_unpack(value, "(d,d,M)", &dev, &n, &module);
	if(*err != NULL)
		return NULL;

	return amp_pack_effect((struct amp_effect_t){ amp_synth_new(dev, n, module), &amp_synth_iface });
}


/**
 * Handle information on a synthesizer.
 *   @synth: The synthesizer.
 *   @info: The information.
 */

void amp_synth_info(struct amp_synth_t *synth, struct amp_info_t info)
{
	if(info.type == amp_info_action_e) {
		bool init;
		unsigned int i;
		struct amp_action_t action = *info.data.action;

		if(action.event.dev != synth->dev)
			return;

		for(i = 0; i < synth->n; i++) {
			if(synth->inst[i].note.key == action.event.key)
				break;
		}

		if(i == synth->n) {
			init = true;

			for(i = 0; i < synth->n; i++) {
				if(synth->inst[i].delay < 0)
					break;
			}

			if(i == synth->n)
				return;
		}
		else
			init = false;

		if(i == synth->n)
			return;

		synth->inst[i].delay = action.delay;
		synth->inst[i].note.init = init;
		synth->inst[i].note.key = action.event.key;
		synth->inst[i].note.vel = (double)action.event.val / (double)UINT16_MAX;
		synth->inst[i].note.freq = amp_key_freq_f(action.event.key);

		amp_module_info(synth->inst[i].module, amp_info_note(&synth->inst[i].note));
	}
}

/**
 * Process a synthesizer.
 *   @synth: The synthesizer.
 *   @buf: The buffer.
 *   @time: The time.
 *   @len: The length.
 */

void amp_synth_proc(struct amp_synth_t *synth, double *buf, struct amp_time_t *time, unsigned int len)
{
	int delay;
	bool cont;
	unsigned int i;
	double tmp[len];

	dsp_zero_d(buf, len);

	for(i = 0; i < synth->n; i++) {
		if(synth->inst[i].delay < 0)
			continue;

		delay = synth->inst[i].delay;
		if(delay < len) {
			cont = amp_module_proc(synth->inst[i].module, tmp, time + delay, len - delay);
			synth->inst[i].delay = cont ? 0 : -1;

			dsp_add_d(buf + delay, tmp, len - delay);
		}
		else
			synth->inst[i].delay -= len;
	}
}
