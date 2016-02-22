#include "../common.h"


/**
 * Sum structure.
 *   @head, tail: The head and tail instances.
 */

struct amp_sum_t {
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

const struct amp_module_i amp_sum_iface = {
	(amp_info_f)amp_sum_info,
	(amp_module_f)amp_sum_proc,
	(amp_copy_f)amp_sum_copy,
	(amp_delete_f)amp_sum_delete
};


/**
 * Create a sum.
 *   &returns: The sum.
 */

struct amp_sum_t *amp_sum_new(void)
{
	struct amp_sum_t *sum;

	sum = malloc(sizeof(struct amp_sum_t));
	sum->head = sum->tail = NULL;

	return sum;
}

/**
 * Copy a sum.
 *   @sum: The original sum.
 *   &returns: The copied sum.
 */

struct amp_sum_t *amp_sum_copy(struct amp_sum_t *sum)
{
	struct inst_t *inst;
	struct amp_sum_t *copy;
	
	copy = amp_sum_new();

	for(inst = sum->head; inst != NULL; inst = inst->next)
		amp_sum_append(copy, amp_param_copy(inst->param));

	return copy;
}

/**
 * Delete a sum.
 *   @sum: The sum.
 */

void amp_sum_delete(struct amp_sum_t *sum)
{
	struct inst_t *cur, *next;

	for(cur = sum->head; cur != NULL; cur = next) {
		next = cur->next;

		amp_param_delete(cur->param);
		free(cur);
	}

	free(sum);
}


/**
 * Create a sum from a value.
 *   @value: The value.
 *   @env: The environment.
 *   @err: The error.
 *   &returns: The value or null.
 */

struct ml_value_t *amp_sum_make(struct ml_value_t *value, struct ml_env_t *env, char **err)
{
#undef fail
#define fail(...) do { ml_value_delete(value); *err = amp_printf(__VA_ARGS__); return NULL; } while(0)

	struct amp_sum_t *sum;
	struct ml_link_t *link;

	if(value->type != ml_value_list_e)
		fail("Type error. Sum requires a list of parameters as input.");

	for(link = value->data.list.head; link != NULL; link = link->next) {
		if(!amp_unbox_isparam(link->value))
			fail("Type error. Sum requires a list of parameters as input.");
	}

	sum = amp_sum_new();

	for(link = value->data.list.head; link != NULL; link = link->next)
		amp_sum_append(sum, amp_unbox_param(link->value));

	ml_value_delete(value);

	return amp_pack_module((struct amp_module_t){ sum, &amp_sum_iface });
}


/**
 * Prepend a parameter onto the sum.
 *   @sum: The sum.
 *   @param: The parameter.
 */

void amp_sum_prepend(struct amp_sum_t *sum, struct amp_param_t *param)
{
	struct inst_t *inst;

	inst = malloc(sizeof(struct inst_t));
	inst->param = param;
	inst->next = sum->head;
	inst->prev = NULL;
	*(sum->head ? &sum->head->prev : &sum->tail) = inst;
	sum->head = inst;
}

/**
 * Append a parameter onto the sum.
 *   @sum: The sum.
 *   @param: The parameter.
 */

void amp_sum_append(struct amp_sum_t *sum, struct amp_param_t *param)
{
	struct inst_t *inst;

	inst = malloc(sizeof(struct inst_t));
	inst->param = param;
	inst->prev = sum->tail;
	inst->next = NULL;
	*(sum->tail ? &sum->tail->next : &sum->head) = inst;
	sum->tail = inst;
}


/**
 * Handle information on a sum.
 *   @sum: The sum.
 *   @info: The information.
 */

void amp_sum_info(struct amp_sum_t *sum, struct amp_info_t info)
{
	struct inst_t *inst;

	for(inst = sum->head; inst != NULL; inst = inst->next)
		amp_param_info(inst->param, info);
}

/**
 * Process a sum.
 *   @sum: The sum.
 *   @buf: The buffer.
 *   @time: The time.
 *   @len: The length.
 *   &returns: The continuation flag.
 */

bool amp_sum_proc(struct amp_sum_t *sum, double *buf, struct amp_time_t *time, unsigned int len)
{
	bool cont = false;
	struct inst_t *inst;
	double tmp[len];

		dsp_zero_d(buf, len);
	for(inst = sum->head; inst != NULL; inst = inst->next) {
		dsp_zero_d(tmp, len);
		cont |= amp_param_proc(inst->param, tmp, time, len);
		dsp_add_d(buf, tmp, len);
	}

	return cont;
}
