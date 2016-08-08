#include "../common.h"


/**
 * Fold structure.
 *   @type: The fold type.
 *   @head, tail: The head and tail instances.
 */
struct amp_fold_t {
	enum amp_fold_e type;
	struct inst_t *head, *tail;
};

/**
 * Instance structure.
 *   @param: The parameter.
 *   @prev, next: The previous and next instances.
 */
struct inst_t {
	struct amp_param_t *param;
	struct inst_t *prev, *next;
};


/*
 * global variables
 */
const struct amp_module_i amp_fold_iface = {
	(amp_info_f)amp_fold_info,
	(amp_module_f)amp_fold_proc,
	(amp_copy_f)amp_fold_copy,
	(amp_delete_f)amp_fold_delete
};


/**
 * Create a fold.
 *   @type; The type.
 *   &returns: The fold.
 */
struct amp_fold_t *amp_fold_new(enum amp_fold_e type)
{
	struct amp_fold_t *fold;

	fold = malloc(sizeof(struct amp_fold_t));
	fold->type = type;
	fold->head = fold->tail = NULL;

	return fold;
}

/**
 * Copy a fold.
 *   @fold: The original fold.
 *   &returns: The copied fold.
 */
struct amp_fold_t *amp_fold_copy(struct amp_fold_t *fold)
{
	struct inst_t *inst;
	struct amp_fold_t *copy;
	
	copy = amp_fold_new(fold->type);

	for(inst = fold->head; inst != NULL; inst = inst->next)
		amp_fold_append(copy, amp_param_copy(inst->param));

	return copy;
}

/**
 * Delete a fold.
 *   @fold: The fold.
 */
void amp_fold_delete(struct amp_fold_t *fold)
{
	struct inst_t *cur, *next;

	for(cur = fold->head; cur != NULL; cur = next) {
		next = cur->next;

		amp_param_delete(cur->param);
		free(cur);
	}

	free(fold);
}


/**
 * Create a fold from a value.
 *   @ret: Ref. The returned value.
 *   @value: The value.
 *   @env: The environment.
 *   &returns: Error.
 */
char *amp_fold_make(struct ml_value_t **ret, struct ml_value_t *value, enum amp_fold_e type)
{
#define onexit amp_fold_delete(sum);
#define error() fail("Type error. Sum requires a list of parameters as input.")
	struct amp_fold_t *sum;
	struct ml_link_t *link;

	sum = amp_fold_new(type);

	if(value->type != ml_value_list_v)
		error();

	for(link = value->data.list->head; link != NULL; link = link->next) {
		if(!amp_unbox_isparam(link->value))
			error();

		amp_fold_append(sum, amp_unbox_param(link->value));
	}

	*ret = amp_pack_module((struct amp_module_t){ sum, &amp_fold_iface });

	return NULL;
#undef onexit
}

/**
 * Create a sum from a value.
 *   @ret: Ref. The returned value.
 *   @value: The value.
 *   @env: The environment.
 *   &returns: Error.
 */
char *amp_add_make(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env)
{
	return amp_fold_make(ret, value, amp_fold_add_v);
}

/**
 * Create a multiply from a value.
 *   @ret: Ref. The returned value.
 *   @value: The value.
 *   @env: The environment.
 *   &returns: Error.
 */
char *amp_mul_make(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env)
{
	return amp_fold_make(ret, value, amp_fold_mul_v);
}


/**
 * Prepend a parameter onto the fold.
 *   @fold: The fold.
 *   @param: The parameter.
 */
void amp_fold_prepend(struct amp_fold_t *fold, struct amp_param_t *param)
{
	struct inst_t *inst;

	inst = malloc(sizeof(struct inst_t));
	inst->param = param;
	inst->next = fold->head;
	inst->prev = NULL;
	*(fold->head ? &fold->head->prev : &fold->tail) = inst;
	fold->head = inst;
}

/**
 * Append a parameter onto the fold.
 *   @fold: The fold.
 *   @param: The parameter.
 */
void amp_fold_append(struct amp_fold_t *fold, struct amp_param_t *param)
{
	struct inst_t *inst;

	inst = malloc(sizeof(struct inst_t));
	inst->param = param;
	inst->prev = fold->tail;
	inst->next = NULL;
	*(fold->tail ? &fold->tail->next : &fold->head) = inst;
	fold->tail = inst;
}


/**
 * Handle information on a fold.
 *   @fold: The fold.
 *   @info: The information.
 */
void amp_fold_info(struct amp_fold_t *fold, struct amp_info_t info)
{
	struct inst_t *inst;

	for(inst = fold->head; inst != NULL; inst = inst->next)
		amp_param_info(inst->param, info);
}

/**
 * Process a fold.
 *   @fold: The fold.
 *   @buf: The buffer.
 *   @time: The time.
 *   @len: The length.
 *   @queue: The action queue.
 *   &returns: The continuation flag.
 */
bool amp_fold_proc(struct amp_fold_t *fold, double *buf, struct amp_time_t *time, unsigned int len, struct amp_queue_t *queue)
{
	bool cont = false;
	struct inst_t *inst;
	double tmp[len];

	switch(fold->type) {
	case amp_fold_add_v:
		dsp_zero_d(buf, len);

		for(inst = fold->head; inst != NULL; inst = inst->next) {
			dsp_zero_d(tmp, len);
			cont |= amp_param_proc(inst->param, tmp, time, len, queue);
			dsp_add_d(buf, tmp, len);
		}

		break;

	case amp_fold_mul_v:
		dsp_one_d(buf, len);

		for(inst = fold->head; inst != NULL; inst = inst->next) {
			dsp_zero_d(tmp, len);
			cont |= amp_param_proc(inst->param, tmp, time, len, queue);
			dsp_mul_d(buf, tmp, len);
		}

		break;
	}

	return cont;
}
