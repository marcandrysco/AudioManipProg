#include "../common.h"


/**
 * Mixer structure.
 *   @head, tail: The head and tail instances.
 */

struct amp_mixer_t {
	struct inst_t *head, *tail;
};

/**
 * Instance structure.
 *   @instr: The instrument.
 *   @prev, next: The previous and next instances.
 */

struct inst_t {
	struct amp_instr_t instr;

	struct inst_t *prev, *next;
};


/*
 * local declarations
 */

struct amp_instr_i amp_mixer_iface = {
	(amp_info_f)amp_mixer_info,
	(amp_instr_f)amp_mixer_proc,
	(amp_copy_f)amp_mixer_copy,
	(amp_delete_f)amp_mixer_delete
};


/**
 * Create a new mixer.
 *   &returns: The mixer.
 */

struct amp_mixer_t *amp_mixer_new(void)
{
	struct amp_mixer_t *mixer;

	mixer = malloc(sizeof(struct amp_mixer_t));
	mixer->head = mixer->tail = NULL;

	return mixer;
}

/**
 * Copy a mixer.
 *   @mixer: The original mixer.
 *   &returns: The copied mixer.
 */

struct amp_mixer_t *amp_mixer_copy(struct amp_mixer_t *mixer)
{
	struct inst_t *inst;
	struct amp_mixer_t *copy;

	copy = amp_mixer_new();

	for(inst = mixer->head; inst != NULL; inst = inst->next)
		amp_mixer_append(copy, amp_instr_copy(inst->instr));

	return copy;
}

/**
 * Delete a mixer.
 *   @mixer: The mixer.
 */

void amp_mixer_delete(struct amp_mixer_t *mixer)
{
	struct inst_t *cur, *next;

	for(cur = mixer->head; cur != NULL; cur = next) {
		next = cur->next;

		amp_instr_delete(cur->instr);
		free(cur);
	}

	free(mixer);
}


/**
 * Create a mixer from a value.
 *   @value: The value.
 *   @env: The environment.
 *   @err: The rror.
 *   &returns: The value or null.
 */

struct ml_value_t *amp_mixer_make(struct ml_value_t *value, struct ml_env_t *env, char **err)
{
#undef fail
#define fail(...) do { ml_value_delete(value); *err = amp_printf(__VA_ARGS__); return NULL; } while(0)

	struct amp_mixer_t *mixer;
	struct ml_link_t *link;

	if(value->type != ml_value_list_e)
		fail("Type error. Instrument mixer requires a list of instrument as input.");

	for(link = value->data.list.head; link != NULL; link = link->next) {
		if(amp_unbox_value(link->value, amp_box_instr_e) == NULL)
			fail("Type error. Instrument mixer requires a list of instrument as input.");
	}

	mixer = amp_mixer_new();

	for(link = value->data.list.head; link != NULL; link = link->next)
		amp_mixer_append(mixer, amp_instr_copy(amp_unbox_value(link->value, amp_box_instr_e)->data.instr));

	ml_value_delete(value);

	return amp_pack_instr((struct amp_instr_t){ mixer, &amp_mixer_iface });
}


/**
 * Prepend an instrument onto the mixer.
 *   @mixer: The mixer.
 *   @instr: The instrument.
 */

void amp_mixer_prepend(struct amp_mixer_t *mixer, struct amp_instr_t instr)
{
	struct inst_t *inst;

	inst = malloc(sizeof(struct inst_t));
	inst->instr = instr;
	inst->next = mixer->head;
	inst->prev = NULL;
	*(mixer->head ? &mixer->head->prev : &mixer->tail) = inst;
	mixer->head = inst;
}

/**
 * Append an instrument onto the mixer.
 *   @mixer: The mixer.
 *   @instr: The instrument.
 */

void amp_mixer_append(struct amp_mixer_t *mixer, struct amp_instr_t instr)
{
	struct inst_t *inst;

	inst = malloc(sizeof(struct inst_t));
	inst->instr = instr;
	inst->prev = mixer->tail;
	inst->next = NULL;
	*(mixer->tail ? &mixer->tail->next : &mixer->head) = inst;
	mixer->tail = inst;
}


/**
 * Handle information on the mixer.
 *   @mixer: The mixer.
 *   @info: The information.
 */

void amp_mixer_info(struct amp_mixer_t *mixer, struct amp_info_t info)
{
	struct inst_t *inst;

	for(inst = mixer->head; inst != NULL; inst = inst->next)
		amp_instr_info(inst->instr, info);
}

/**
 * Process a mixer.
 *   @mixer: The mixer.
 *   @buf: The buffer.
 *   @time: The time.
 *   @len: The length.
 */

void amp_mixer_proc(struct amp_mixer_t *mixer, double **buf, struct amp_time_t *time, unsigned int len)
{
	double in[2][len], out[2][len];
	struct inst_t *inst;

	dsp_copy_d(in[0], buf[0], len);
	dsp_copy_d(in[1], buf[1], len);

	dsp_zero_d(out[0], len);
	dsp_zero_d(out[1], len);

	for(inst = mixer->head; inst != NULL; inst = inst->next) {
		dsp_copy_d(buf[0], in[0], len);
		dsp_copy_d(buf[1], in[1], len);

		amp_instr_proc(inst->instr, buf, time, len);

		dsp_add_d(out[0], buf[0], len);
		dsp_add_d(out[1], buf[1], len);
	}

	dsp_copy_d(buf[0], out[0], len);
	dsp_copy_d(buf[1], out[1], len);
}
