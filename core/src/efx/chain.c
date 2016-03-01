#include "../common.h"


/**
 * Chain structure.
 *   @head, tail: The head and tail.
 */

struct amp_chain_t {
	struct inst_t *head, *tail;
};

/**
 * Instance structure.
 *   @mix: The mix.
 *   @effect: The effect.
 *   @prev, next: The previous and next instances.
 */

struct inst_t {
	double mix;
	struct amp_effect_t effect;
	struct inst_t *prev, *next;
};


/*
 * local declarations
 */

static struct inst_t *inst_new(double mix, struct amp_effect_t effect);
static void inst_delete(struct inst_t *inst);

/*
 * global variables
 */

const struct amp_effect_i amp_chain_iface = {
	(amp_info_f)amp_chain_info,
	(amp_effect_f)amp_chain_proc,
	(amp_copy_f)amp_chain_copy,
	(amp_delete_f)amp_chain_delete
};


/**
 * Create a chain effect.
 *   &returns: The chain.
 */

struct amp_chain_t *amp_chain_new(void)
{
	struct amp_chain_t *chain;

	chain = malloc(sizeof(struct amp_chain_t));
	chain->head = chain->tail = NULL;

	return chain;
}

/**
 * Copy a chain effect.
 *   @chain: The original chain.
 *   &returns: The copied chain.
 */

struct amp_chain_t *amp_chain_copy(struct amp_chain_t *chain)
{
	struct inst_t *inst;
	struct amp_chain_t *copy;
	
	copy = amp_chain_new();

	for(inst = chain->head; inst != NULL; inst = inst->next)
		amp_chain_append(copy, inst->mix, amp_effect_copy(inst->effect));

	return copy;
}

/**
 * Delete a chain effect.
 *   @chain: The chain.
 */

void amp_chain_delete(struct amp_chain_t *chain)
{
	struct inst_t *cur, *next;

	for(cur = chain->head; cur != NULL; cur = next) {
		next = cur->next;
		inst_delete(cur);
	}

	free(chain);
}


/**
 * Prepend an effect to a chain.
 *   @chain: The chain.
 *   @mix: The mix.
 *   @effect: The effect.
 */

void amp_chain_prepend(struct amp_chain_t *chain, double mix, struct amp_effect_t effect)
{
	struct inst_t *inst;

	inst = inst_new(mix, effect);
	inst->next = chain->head;
	inst->prev = NULL;
	*(chain->head ? &chain->head->prev : &chain->tail) = inst;
	chain->head = inst;
}

/**
 * Append an effect to a chain.
 *   @chain: The chain.
 *   @mix: The mix.
 *   @effect: The effect.
 */

void amp_chain_append(struct amp_chain_t *chain, double mix, struct amp_effect_t effect)
{
	struct inst_t *inst;

	inst = inst_new(mix, effect);
	inst->prev = chain->tail;
	inst->next = NULL;
	*(chain->tail ? &chain->tail->next : &chain->head) = inst;
	chain->tail = inst;
}


/**
 * Create a chain from a value.
 *   @value: The value.
 *   @env: The environment.
 *   @err: The rror.
 *   &returns: The value or null.
 */

struct ml_value_t *amp_chain_make(struct ml_value_t *value, struct ml_env_t *env, char **err)
{
#undef fail
#define fail(...) do { amp_chain_delete(chain); ml_value_delete(value); *err = amp_printf(__VA_ARGS__); return NULL; } while(0)

	struct ml_link_t *link;
	struct amp_chain_t *chain;
	struct amp_box_t *box;

	chain = amp_chain_new();

	if(value->type != ml_value_list_e)
		fail("Type error. Chain requires a list of effects as input.");

	for(link = value->data.list.head; link != NULL; link = link->next) {
		if(link->value->type == ml_value_tuple_e) {
			struct ml_tuple_t tuple = link->value->data.tuple;

			if(tuple.len != 2)
				fail("Type error. Effects chain instance must take the form 'Effect' or '(num,Effect)'.");

			box = amp_unbox_value(tuple.value[1], amp_box_effect_e);
			if((tuple.value[0]->type != ml_value_num_e) || (box == NULL))
				fail("Type error. Effects chain instance must take the form 'Effect' or '(num,Effect)'.");

			amp_chain_append(chain, tuple.value[0]->data.num, amp_effect_copy(box->data.effect));
		}
		else {
			box = amp_unbox_value(link->value, amp_box_effect_e);
			if(box == NULL)
				fail("Type error. Effects chain instance must take the form 'Effect' or '(num,Effect)'.");

			amp_chain_append(chain, 1.0, amp_effect_copy(box->data.effect));
		}
	}

	ml_value_delete(value);

	return amp_pack_effect((struct amp_effect_t){ chain, &amp_chain_iface });
}


/**
 * Handle inforomation on a chain.
 *   @chain: The chain.
 *   @info: The info.
 */

void amp_chain_info(struct amp_chain_t *chain, struct amp_info_t info)
{
	struct inst_t *inst;

	for(inst = chain->head; inst != NULL; inst = inst->next)
		amp_effect_info(inst->effect, info);
}

/**
 * Process a chain.
 *   @chain: The chain.
 *   @buf: The buffer.
 *   @time: The time.
 *   @len: The length.
 */

void amp_chain_proc(struct amp_chain_t *chain, double *buf, struct amp_time_t *time, unsigned int len)
{
	struct inst_t *inst;

	for(inst = chain->head; inst != NULL; inst = inst->next) {
		if(inst->mix == 1.0)
			amp_effect_proc(inst->effect, buf, time, len);
		else {
			unsigned int i;
			double mix = inst->mix, tmp[len];

			dsp_copy_d(tmp, buf, len);
			amp_effect_proc(inst->effect, tmp, time, len);

			for(i = 0; i < len; i++)
				buf[i] = mix * tmp[i] + (1.0 - mix) * buf[i];
		}
	}
}


/**
 * Create an instance.
 *   @mix: The mix.
 *   @effect: The effect.
 *   &returns: The instance.
 */

static struct inst_t *inst_new(double mix, struct amp_effect_t effect)
{
	struct inst_t *inst;

	inst = malloc(sizeof(struct inst_t));
	inst->mix = mix;
	inst->effect = effect;

	return inst;
}

/**
 * Delete an instance.
 *   @inst: The instance.
 */

static void inst_delete(struct inst_t *inst)
{
	amp_effect_delete(inst->effect);
	free(inst);
}
