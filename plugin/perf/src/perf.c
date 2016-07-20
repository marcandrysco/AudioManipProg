#include "common.h"


/*
 * local declarations
 */
struct amp_poly_i perf_inst_iface = { (amp_poly_f)perf_inst_proc, (amp_info_f)perf_inst_info, (copy_f)perf_inst_copy, (delete_f)perf_inst_delete };


/**
 * Create a new performance tracker.
 *   @id: The identifier.
 *   @core: The core.
 *   &returns: The performance tracker.
 */
struct perf_inst_t *perf_inst_new(char *id, struct amp_core_t *core)
{
	struct perf_inst_t *perf;

	perf = malloc(sizeof(struct perf_inst_t));
	perf->id = id;
	perf->core = core;
	perf->refcnt = 1;
	perf->amp = amp_perf_init();
	perf_web_add(core->env, perf);

	return perf;
}

/**
 * Copy a performance tracker.
 *   @perf: The performance tracker.
 *   &returns: The copy.
 */
struct perf_inst_t *perf_inst_copy(struct perf_inst_t *perf)
{
	perf->refcnt++;

	return perf;
}

/**
 * Delete a performance tracker.
 *   @perf: The performance tracker.
 */
void perf_inst_delete(struct perf_inst_t *perf)
{
	if(--perf->refcnt > 0)
		return;

	perf_web_remove(perf->core->env, perf);
	free(perf->id);
	free(perf);
}


/**
 * Create a gain from a value.
 *   @ret: Ref. The returned value.
 *   @value: The value.
 *   @env: The environment.
 *   &returns: Error.
 */
char *perf_inst_make(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env)
{
#define onexit
	char *id;
	struct amp_box_t *box;

	chkfail(amp_match_unpack(value, "(s,*)", &id, &box));
	*ret = amp_poly_make(box, perf_inst_new(id, amp_core_get(env)), &perf_inst_iface);

	return NULL;
#undef onexit
}


/**
 * Process information on a performance tracker.
 *   @perf: The performance tracker.
 *   @info: The information.
 */
void perf_inst_info(struct perf_inst_t *perf, struct amp_info_t info)
{
}

/**
 * Process a performance tracker.
 *   @perf: The performance tracker.
 *   @poly: The polymorphic object.
 *   @info: The information.
 *   &returns: The continuation flag.
 */
bool perf_inst_proc(struct perf_inst_t *perf, struct amp_poly_t *poly, struct amp_polyinfo_t *info)
{
	bool cont;

	amp_perf_mark(&perf->amp);
	cont = amp_poly_proc(poly, info);
	amp_perf_mark(&perf->amp);

	return cont;
}
