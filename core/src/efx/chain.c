#include "../common.h"


/*
 * local declarations
 */

static struct amp_chain_inst_t *inst_new(struct amp_effect_t effect);
static void inst_delete(struct amp_chain_inst_t *inst);

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
	struct amp_chain_inst_t *inst;
	struct amp_chain_t *copy;
	
	copy = amp_chain_new();

	for(inst = chain->head; inst != NULL; inst = inst->next)
		amp_chain_append(copy, amp_effect_copy(inst->effect));

	return copy;
}

/**
 * Delete a chain effect.
 *   @chain: The chain.
 */
void amp_chain_delete(struct amp_chain_t *chain)
{
	struct amp_chain_inst_t *cur, *next;

	for(cur = chain->head; cur != NULL; cur = next) {
		next = cur->next;
		inst_delete(cur);
	}

	free(chain);
}


/**
 * Prepend an effect to a chain.
 *   @chain: The chain.
 *   @effect: The effect.
 */
void amp_chain_prepend(struct amp_chain_t *chain, struct amp_effect_t effect)
{
	struct amp_chain_inst_t *inst;

	inst = inst_new(effect);
	inst->next = chain->head;
	inst->prev = NULL;
	*(chain->head ? &chain->head->prev : &chain->tail) = inst;
	chain->head = inst;
}

/**
 * Append an effect to a chain.
 *   @chain: The chain.
 *   @effect: The effect.
 */
void amp_chain_append(struct amp_chain_t *chain, struct amp_effect_t effect)
{
	struct amp_chain_inst_t *inst;

	inst = inst_new(effect);
	inst->prev = chain->tail;
	inst->next = NULL;
	*(chain->tail ? &chain->tail->next : &chain->head) = inst;
	chain->tail = inst;
}


/**
 * Create a chain from a value.
 *   @ret: Ref. The returned value.
 *   @value: The value.
 *   @env: The environment.
 *   &returns: Error.
 */
char *amp_chain_make(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env)
{
#define onexit amp_chain_delete(chain);
#define error() fail("%C: Type error. Chain requires a list of effects as input.", ml_tag_chunk(&value->tag));
	struct ml_link_t *link;
	struct amp_chain_t *chain;
	struct amp_box_t *box;

	chain = amp_chain_new();

	if(value->type != ml_value_list_v)
		error();

	for(link = value->data.list->head; link != NULL; link = link->next) {
		box = amp_unbox_value(link->value, amp_box_effect_e);
		if(box == NULL)
			error();

		amp_chain_append(chain, amp_effect_copy(box->data.effect));
	}

	*ret = amp_pack_effect((struct amp_effect_t){ chain, &amp_chain_iface });

	return NULL;
#undef error
#undef onexit
}


/**
 * Handle inforomation on a chain.
 *   @chain: The chain.
 *   @info: The info.
 */
void amp_chain_info(struct amp_chain_t *chain, struct amp_info_t info)
{
	struct amp_chain_inst_t *inst;

	for(inst = chain->head; inst != NULL; inst = inst->next)
		amp_effect_info(inst->effect, info);
}

/**
 * Process a chain.
 *   @chain: The chain.
 *   @buf: The buffer.
 *   @time: The time.
 *   @len: The length.
 *   @queue: The action queue.
 *   &returns: The continuation flag.
 */
bool amp_chain_proc(struct amp_chain_t *chain, double *buf, struct amp_time_t *time, unsigned int len, struct amp_queue_t *queue)
{
	bool cont = false;
	struct amp_chain_inst_t *inst;

	for(inst = chain->head; inst != NULL; inst = inst->next)
		cont |= amp_effect_proc(inst->effect, buf, time, len, queue);

	return cont;
}


/**
 * Create an instance.
 *   @effect: The effect.
 *   &returns: The instance.
 */
static struct amp_chain_inst_t *inst_new(struct amp_effect_t effect)
{
	struct amp_chain_inst_t *inst;

	inst = malloc(sizeof(struct amp_chain_inst_t));
	inst->effect = effect;

	return inst;
}

/**
 * Delete an instance.
 *   @inst: The instance.
 */
static void inst_delete(struct amp_chain_inst_t *inst)
{
	amp_effect_delete(inst->effect);
	free(inst);
}
