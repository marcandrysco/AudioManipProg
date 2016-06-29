#ifndef INST_H
#define INST_H

/**
 * Instance structure.
 *   @id: The identifier.
 *   @core: The core.
 *   @amp: The internal AMP tracker.
 *   @refcnt: Reference count.
 */
struct perf_inst_t {
	char *id;
	struct amp_core_t *core;
	struct amp_perf_t amp;

	unsigned int refcnt;
};


/*
 * instance declarations
 */
struct perf_inst_t *perf_inst_new(char *id, struct amp_core_t *core);
struct perf_inst_t *perf_inst_copy(struct perf_inst_t *perf);
void perf_inst_delete(struct perf_inst_t *perf);

char *perf_inst_make(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env);

bool perf_inst_proc(struct perf_inst_t *perf, struct amp_poly_t *poly, struct amp_polyinfo_t *info);

#endif
