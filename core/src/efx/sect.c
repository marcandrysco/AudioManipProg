#include "../common.h"


/*
 * local declarations
 */

static struct amp_sect_inst_t *inst_new(double mix, struct amp_effect_t effect);
static void inst_delete(struct amp_sect_inst_t *inst);

/*
 * global variables
 */
const struct amp_effect_i amp_sect_iface = {
	(amp_info_f)amp_sect_info,
	(amp_effect_f)amp_sect_proc,
	(amp_copy_f)amp_sect_copy,
	(amp_delete_f)amp_sect_delete
};


/**
 * Create a section effect.
 *   &returns: The section.
 */
struct amp_sect_t *amp_sect_new(void)
{
	struct amp_sect_t *sect;

	sect = malloc(sizeof(struct amp_sect_t));
	sect->head = sect->tail = NULL;

	return sect;
}

/**
 * Copy a section effect.
 *   @sect: The original sect.
 *   &returns: The copied sect.
 */
struct amp_sect_t *amp_sect_copy(struct amp_sect_t *sect)
{
	struct amp_sect_inst_t *inst;
	struct amp_sect_t *copy;
	
	copy = amp_sect_new();

	for(inst = sect->head; inst != NULL; inst = inst->next)
		amp_sect_append(copy, inst->mix, amp_effect_copy(inst->effect));

	return copy;
}

/**
 * Delete a section effect.
 *   @sect: The section.
 */
void amp_sect_delete(struct amp_sect_t *sect)
{
	struct amp_sect_inst_t *cur, *next;

	for(cur = sect->head; cur != NULL; cur = next) {
		next = cur->next;
		inst_delete(cur);
	}

	free(sect);
}


/**
 * Prepend an effect to a section.
 *   @sect: The section.
 *   @mix: The mix.
 *   @effect: The effect.
 */
void amp_sect_prepend(struct amp_sect_t *sect, double mix, struct amp_effect_t effect)
{
	struct amp_sect_inst_t *inst;

	inst = inst_new(mix, effect);
	inst->next = sect->head;
	inst->prev = NULL;
	*(sect->head ? &sect->head->prev : &sect->tail) = inst;
	sect->head = inst;
}

/**
 * Append an effect to a section.
 *   @sect: The section.
 *   @mix: The mix.
 *   @effect: The effect.
 */
void amp_sect_append(struct amp_sect_t *sect, double mix, struct amp_effect_t effect)
{
	struct amp_sect_inst_t *inst;

	inst = inst_new(mix, effect);
	inst->prev = sect->tail;
	inst->next = NULL;
	*(sect->tail ? &sect->tail->next : &sect->head) = inst;
	sect->tail = inst;
}


/**
 * Create a section from a value.
 *   @value: The value.
 *   @env: The environment.
 *   @err: The rror.
 *   &returns: The value or null.
 */
struct ml_value_t *amp_sect_make(struct ml_value_t *value, struct ml_env_t *env, char **err)
{
#undef fail
#define fail(...) do { amp_sect_delete(sect); ml_value_delete(value); *err = amp_printf(__VA_ARGS__); return NULL; } while(0)

	struct ml_link_t *link;
	struct amp_sect_t *sect;
	struct amp_box_t *box;

	sect = amp_sect_new();

	if(value->type != ml_value_list_e)
		fail("Type error. Chain requires a list of effects as input.");

	for(link = value->data.list.head; link != NULL; link = link->next) {
		if(link->value->type == ml_value_tuple_e) {
			struct ml_tuple_t tuple = link->value->data.tuple;

			if(tuple.len != 2)
				fail("Type error. Section instance must take the form 'Effect' or '(num,Effect)'.");

			box = amp_unbox_value(tuple.value[1], amp_box_effect_e);
			if((tuple.value[0]->type != ml_value_num_e) || (box == NULL))
				fail("Type error. Section instance must take the form 'Effect' or '(num,Effect)'.");

			amp_sect_append(sect, tuple.value[0]->data.num, amp_effect_copy(box->data.effect));
		}
		else {
			box = amp_unbox_value(link->value, amp_box_effect_e);
			if(box == NULL)
				fail("Type error. Section instance must take the form 'Effect' or '(num,Effect)'.");

			amp_sect_append(sect, 1.0, amp_effect_copy(box->data.effect));
		}
	}

	ml_value_delete(value);

	return amp_pack_effect((struct amp_effect_t){ sect, &amp_sect_iface });
}


/**
 * Handle inforomation on a section.
 *   @sect: The section.
 *   @info: The info.
 */
void amp_sect_info(struct amp_sect_t *sect, struct amp_info_t info)
{
	struct amp_sect_inst_t *inst;

	for(inst = sect->head; inst != NULL; inst = inst->next)
		amp_effect_info(inst->effect, info);
}

/**
 * Process a section.
 *   @sect: The section.
 *   @buf: The buffer.
 *   @time: The time.
 *   @len: The length.
 *   &returns: The continuation flag.
 */
bool amp_sect_proc(struct amp_sect_t *sect, double *buf, struct amp_time_t *time, unsigned int len)
{
	bool cont = false;
	struct amp_sect_inst_t *inst;
	double in[len], out[len];

	dsp_copy_d(in, buf, len);
	dsp_zero_d(out, len);

	for(inst = sect->head; inst != NULL; inst = inst->next) {
		dsp_copy_d(buf, in, len);

		if(inst->mix == 1.0)
			cont |= amp_effect_proc(inst->effect, buf, time, len);
		else {
			unsigned int i;
			double mix = inst->mix, tmp[len];

			dsp_copy_d(tmp, buf, len);
			cont |= amp_effect_proc(inst->effect, tmp, time, len);

			for(i = 0; i < len; i++)
				buf[i] = mix * tmp[i] + (1.0 - mix) * buf[i];
		}

		dsp_add_d(out, buf, len);
	}

	dsp_copy_d(buf, out, len);

	return cont;
}


/**
 * Create an instance.
 *   @mix: The mix.
 *   @effect: The effect.
 *   &returns: The instance.
 */
static struct amp_sect_inst_t *inst_new(double mix, struct amp_effect_t effect)
{
	struct amp_sect_inst_t *inst;

	inst = malloc(sizeof(struct amp_sect_inst_t));
	inst->mix = mix;
	inst->effect = effect;

	return inst;
}

/**
 * Delete an instance.
 *   @inst: The instance.
 */
static void inst_delete(struct amp_sect_inst_t *inst)
{
	amp_effect_delete(inst->effect);
	free(inst);
}
