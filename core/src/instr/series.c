#include "../common.h"


/*
 * local declarations
 */
struct amp_instr_i amp_series_iface = {
	(amp_info_f)amp_series_info,
	(amp_instr_f)amp_series_proc,
	(amp_copy_f)amp_series_copy,
	(amp_delete_f)amp_series_delete
};


/**
 * Create a new series.
 *   &returns: The series.
 */
struct amp_series_t *amp_series_new(void)
{
	struct amp_series_t *series;

	series = malloc(sizeof(struct amp_series_t));
	series->head = series->tail = NULL;

	return series;
}

/**
 * Copy a series.
 *   @series: The original series.
 *   &returns: The copied series.
 */
struct amp_series_t *amp_series_copy(struct amp_series_t *series)
{
	struct amp_series_inst_t *inst;
	struct amp_series_t *copy;

	copy = amp_series_new();

	for(inst = series->head; inst != NULL; inst = inst->next)
		amp_series_append(copy, amp_instr_copy(inst->instr));

	return copy;
}

/**
 * Delete a series.
 *   @series: The series.
 */
void amp_series_delete(struct amp_series_t *series)
{
	struct amp_series_inst_t *cur, *next;

	for(cur = series->head; cur != NULL; cur = next) {
		next = cur->next;

		amp_instr_delete(cur->instr);
		free(cur);
	}

	free(series);
}


/**
 * Create a series from a value.
 *   @ret: Ref. The returned value.
 *   @value: The value.
 *   @env: The environment.
 *   &returns: Error.
 */
char *amp_series_make(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env)
{
#define onexit ml_value_delete(value); amp_series_delete(series);
	struct amp_series_t *series;
	struct ml_link_t *link;

	series = amp_series_new();

	if(value->type != ml_value_list_v)
		fail("Type error. Instrument series requires a list of instrument as input.");

	for(link = value->data.list->head; link != NULL; link = link->next) {
		if(amp_unbox_value(link->value, amp_box_instr_e) == NULL)
			fail("Type error. Instrument series requires a list of instrument as input.");
	}

	for(link = value->data.list->head; link != NULL; link = link->next)
		amp_series_append(series, amp_instr_copy(amp_unbox_value(link->value, amp_box_instr_e)->data.instr));

	*ret = amp_pack_instr((struct amp_instr_t){ series, &amp_series_iface });
	ml_value_delete(value);

	return NULL;
#undef onexit
}


/**
 * Prepend an instrument onto the series.
 *   @series: The series.
 *   @instr: The instrument.
 */
void amp_series_prepend(struct amp_series_t *series, struct amp_instr_t instr)
{
	struct amp_series_inst_t *inst;

	inst = malloc(sizeof(struct amp_series_inst_t));
	inst->instr = instr;
	inst->next = series->head;
	inst->prev = NULL;
	*(series->head ? &series->head->prev : &series->tail) = inst;
	series->head = inst;
}

/**
 * Append an instrument onto the series.
 *   @series: The series.
 *   @instr: The instrument.
 */
void amp_series_append(struct amp_series_t *series, struct amp_instr_t instr)
{
	struct amp_series_inst_t *inst;

	inst = malloc(sizeof(struct amp_series_inst_t));
	inst->instr = instr;
	inst->prev = series->tail;
	inst->next = NULL;
	*(series->tail ? &series->tail->next : &series->head) = inst;
	series->tail = inst;
}


/**
 * Handle information on the series.
 *   @series: The series.
 *   @info: The information.
 */
void amp_series_info(struct amp_series_t *series, struct amp_info_t info)
{
	struct amp_series_inst_t *inst;

	for(inst = series->head; inst != NULL; inst = inst->next)
		amp_instr_info(inst->instr, info);
}

/**
 * Process a series.
 *   @series: The series.
 *   @buf: The buffer.
 *   @time: The time.
 *   @len: The length.
 *   @queue: The action queue.
 */
void amp_series_proc(struct amp_series_t *series, double **buf, struct amp_time_t *time, unsigned int len, struct amp_queue_t *queue)
{
	struct amp_series_inst_t *inst;

	for(inst = series->head; inst != NULL; inst = inst->next)
		amp_instr_proc(inst->instr, buf, time, len, queue);
}
