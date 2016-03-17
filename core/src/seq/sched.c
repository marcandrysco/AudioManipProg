#include "../common.h"


/**
 * Schedule structure.
 *   @cur, head, tail: The current, head, and tail instances.
 */

struct amp_sched_t {
	struct inst_t *cur, *head, *tail;
};

/**
 * Instance structure.
 *   @time: The time.
 *   @event: The event
 *   @prev, next: The previous and next instances.
 */

struct inst_t {
	struct amp_time_t time;
	struct amp_event_t event;

	struct inst_t *prev, *next;
};


/*
 * local declarations
 */

static struct inst_t *inst_before(struct amp_sched_t *sched, struct amp_time_t time);

/*
 * global variables
 */

const struct amp_seq_i amp_sched_iface = {
	(amp_info_f)amp_sched_info,
	(amp_seq_f)amp_sched_proc,
	(amp_copy_f)amp_sched_copy,
	(amp_delete_f)amp_sched_delete
};


bool amp_scan_time(struct amp_time_t *time, struct ml_value_t *value)
{
	struct ml_tuple_t tuple;

	if(value->type != ml_value_tuple_e)
		return false;

	tuple = value->data.tuple;
	if(tuple.len != 2)
		return false;

	if((tuple.value[0]->type != ml_value_num_e) || (tuple.value[1]->type != ml_value_num_e))
		return false;

	*time = (struct amp_time_t){ 0, tuple.value[0]->data.num, tuple.value[1]->data.num };

	return true;
}

bool amp_scan_event(struct amp_event_t *event, struct ml_value_t *value)
{
	struct ml_tuple_t tuple;

	if(value->type != ml_value_tuple_e)
		return false;

	tuple = value->data.tuple;
	if(tuple.len != 3)
		return false;

	if((tuple.value[0]->type != ml_value_num_e) || (tuple.value[1]->type != ml_value_num_e) || (tuple.value[2]->type != ml_value_num_e))
		return false;

	*event = (struct amp_event_t){ tuple.value[0]->data.num, tuple.value[1]->data.num, tuple.value[2]->data.num };

	return true;
}

/**
 * Create a schedule from a value.
 *   @value: The value.
 *   @env: The environment.
 *   @err: The rror.
 *   &returns: The value or null.
 */

struct ml_value_t *amp_sched_make(struct ml_value_t *value, struct ml_env_t *env, char **err)
{
#undef fail
#define fail() do { amp_sched_delete(sched); ml_value_delete(value); *err = amp_printf("Type error. Expected '[((Num,Num),(Num,Num,Num))]'."); return NULL; } while(0)

	struct ml_link_t *link;
	struct ml_tuple_t tuple;
	struct amp_sched_t *sched;
	struct amp_time_t time;
	struct amp_event_t event;

	sched = amp_sched_new();

	if(value->type != ml_value_list_e)
		fail();

	for(link = value->data.list.head; link != NULL; link = link->next) {
		if(link->value->type != ml_value_tuple_e)
			fail();

		tuple = link->value->data.tuple;
		if(tuple.len != 2)
			fail();

		if(!amp_scan_time(&time, tuple.value[0]) || !amp_scan_event(&event, tuple.value[1]))
			fail();

		amp_sched_add(sched, time, event);
	}

	ml_value_delete(value);

	return amp_pack_seq((struct amp_seq_t){ sched, &amp_sched_iface });
}


/**
 * Create a new schedule.
 *   &returns: The schedule.
 */

struct amp_sched_t *amp_sched_new(void)
{
	struct amp_sched_t *sched;

	sched = malloc(sizeof(struct amp_sched_t));
	sched->cur = sched->head = sched->tail = NULL;

	return sched;
}

/**
 * Copy a schedule.
 *   @schedule: The original schedule.
 *   &returns: The copied schedule.
 */

struct amp_sched_t *amp_sched_copy(struct amp_sched_t *sched)
{
	struct inst_t *inst;
	struct amp_sched_t *copy;

	copy = amp_sched_new();

	for(inst = sched->head; inst != NULL; inst = inst->next)
		amp_sched_add(copy, inst->time, inst->event);

	return copy;
}

/**
 * Delete a schedule.
 *   @sched: The schedule.
 */

void amp_sched_delete(struct amp_sched_t *sched)
{
	struct inst_t *cur, *next;

	for(cur = sched->head; cur != NULL; cur = next) {
		next = cur->next;
		free(cur);
	}

	free(sched);
}


/**
 * Add an event to the schedule.
 *   @sched: The schedule.
 *   @time: The time.
 *   @event: The event.
 */

void amp_sched_add(struct amp_sched_t *sched, struct amp_time_t time, struct amp_event_t event)
{
	struct inst_t *inst, *cur;

	inst = malloc(sizeof(struct inst_t));
	inst->time = time;
	inst->event = event;

	if(sched->tail != NULL) {
		cur = inst_before(sched, time);
		if(cur == NULL) {
			inst->next = sched->head;
			inst->prev = NULL;
			*(sched->head ? &sched->head->prev : &sched->tail) = inst;
			sched->head = sched->cur = inst;
		}
		else {
			*(cur->next ? &cur->next->prev : &sched->tail) = inst;
			inst->prev = cur;
			inst->next = cur->next;
			cur->next = inst;
		}
	}
	else {
		inst->prev = inst->next = NULL;
		sched->head = sched->tail = sched->cur = inst;
	}
}

/**
 * Process information on a schedule.
 *   @sched: The schedule.
 *   @queue: The queue.
 *   @time: The time.
 *   @len: The length.
 */

void amp_sched_info(struct amp_sched_t *sched, struct amp_info_t info)
{
}

/**
 * Process a schedule.
 *   @sched: The schedule.
 *   @queue: The queue.
 *   @time: The time.
 *   @len: The length.
 */

void amp_sched_proc(struct amp_sched_t *sched, struct amp_queue_t *queue, struct amp_time_t *time, unsigned int len)
{
	unsigned int i;
	struct inst_t *cur;

	if(sched->cur == NULL)
		return;

	if(amp_time_cmp(time[0], time[len]) == 0)
		return;

	for(i = 0; i < len; i++) {
		cur = sched->cur;

		do {
			if(!amp_time_between(cur->time, time[i], time[i+1]))
				break;

			amp_queue_add(queue, (struct amp_action_t){ i, cur->event });

			cur = cur->next ?: sched->head;
		} while(cur != sched->cur);

		sched->cur = cur;
	}

}


/**
 * Fine the last instance before the given time.
 *   @sched: The schedule.
 *   @time: The time.
 *   &returns: The instance or null.
 */

static struct inst_t *inst_before(struct amp_sched_t *sched, struct amp_time_t time)
{
	struct inst_t *inst;

	for(inst = sched->tail; inst != NULL; inst = inst->prev) {
		if(amp_time_cmp(inst->time, time) < 0)
			return inst;
	}

	return NULL;
}
