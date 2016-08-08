#include "../common.h"


/**
 * Basic clock structure.
 *   @run: The running flag.
 *   @idx: The current index.
 *   @cur: The current time.
 *   @rate: The sample rate.
 *   @bpm, nbeats: The beats-per-minutes and beats-per-measure.
 */
struct amp_basic_t {
	bool run;

	int idx;
	struct amp_time_t cur;
	unsigned int rate;

	double bpm, nbeats;
};


/*
 * global variables
 */
const struct amp_clock_i amp_basic_iface = {
	(amp_info_f)amp_basic_info,
	(amp_clock_f)amp_basic_proc,
	(amp_copy_f)amp_basic_copy,
	(amp_delete_f)amp_basic_delete
};


/**
 * Create a basic clock.
 *   @bpm: The initial beats-per-minute.
 *   @nbeats: The initial beats-per-measure.
 *   @rate: The sample rate.
 *   &returns: The basic clock.
 */
struct amp_basic_t *amp_basic_new(double bpm, double nbeats, unsigned int rate)
{
	struct amp_basic_t *basic;

	basic = malloc(sizeof(struct amp_basic_t));
	basic->run = false;
	basic->idx = 0;
	basic->bpm = bpm;
	basic->nbeats = nbeats;
	basic->rate = rate;
	basic->cur = (struct amp_time_t){ 0, 0, 0.0 };

	return basic;
}

/**
 * Copy a basic clock.
 *   @basic: The original basic clock.
 *   &returns: The copied basic clock.
 */
struct amp_basic_t *amp_basic_copy(struct amp_basic_t *basic)
{
	return amp_basic_new(basic->bpm, basic->nbeats, basic->rate);
}

/**
 * Delete a basic clock.
 *   @basic: The basic clock.
 */
void amp_basic_delete(struct amp_basic_t *basic)
{
	free(basic);
}


/**
 * Seek to a given bar.
 *   @basic: The basic clock.
 *   @bar: The bar.
 */
void amp_basic_seek(struct amp_basic_t *basic, double bar)
{
	basic->idx = bar / basic->bpm * basic->nbeats * 60 * basic->rate;
}


/**
 * Create a basic clock from a value.
 *   @ret: Ref. The returned value.
 *   @value: The value.
 *   @env: The environment.
 *   &returns: Error.
 */
char *amp_basic_make(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env)
{
#define onexit
#define error() fail("%C: Expected '(Num,Num,[int])'.", ml_tag_chunk(&value->tag))
	struct ml_list_t *tuple;
	struct amp_basic_t *basic;
	double bpm, nbeats;

	if(value->type != ml_value_tuple_v)
		error();

	tuple = value->data.list;
	if(tuple->len != 3)
		error();

	if(!ml_value_isnum(ml_list_get(tuple, 0)->value) || !ml_value_isnum(ml_list_get(tuple, 1)->value))
		error();

	bpm = ml_value_getflt(ml_list_get(tuple, 0)->value);
	nbeats = ml_value_getflt(ml_list_get(tuple, 1)->value);

	basic = amp_basic_new(bpm, nbeats, amp_core_rate(env));
	*ret = amp_pack_clock(amp_basic_clock(basic));

	return NULL;
#undef error
#undef onexit
}


/**
 * Handle information on the basic clock.
 *   @basic: The basic clock.
 *   @info: The info.
 */
void amp_basic_info(struct amp_basic_t *basic, struct amp_info_t info)
{
	switch(info.type) {
	case amp_info_tell_e:
		*info.data.flt = basic->cur.bar;
		break;

	case amp_info_seek_e:
		amp_basic_seek(basic, *info.data.flt);
		basic->cur = amp_time_calc(basic->idx, basic->bpm, basic->nbeats, basic->rate);
		break;

	case amp_info_start_e:
		basic->run = true;
		info.data.seek->idx = basic->idx;
		info.data.seek->time = amp_time_calc(basic->idx, basic->bpm, basic->nbeats, basic->rate);
		break;

	case amp_info_stop_e:
		basic->run = false;
		info.data.seek->idx = basic->idx;
		info.data.seek->time = amp_time_calc(basic->idx, basic->bpm, basic->nbeats, basic->rate);
		break;

	default:
		return;
	}
}

/**
 * Process the basic clock.
 *   @basic: The basic clock.
 *   @time: The time.
 *   @len: The length.
 */
void amp_basic_proc(struct amp_basic_t *basic, struct amp_time_t *time, unsigned int len)
{
	unsigned int i;

	time[0] = basic->cur;

	if(basic->run) {
		basic->idx++;

		for(i = 1; i < len; i++)
			time[i] = amp_time_calc(basic->idx++, basic->bpm, basic->nbeats, basic->rate);

		basic->cur = time[i] = amp_time_calc(basic->idx, basic->bpm, basic->nbeats, basic->rate);
	}
	else {
		for(i = 1; i <= len; i++)
			time[i] = basic->cur;
	}
}
