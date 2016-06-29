#ifndef PLUGIN_H
#define PLUGIN_H

/*
 * performance declarations
 */
struct web_t *perf_web_get(struct ml_env_t *env);

void perf_web_add(struct ml_env_t *env, struct perf_inst_t *inst);
void perf_web_remove(struct ml_env_t *env, struct perf_inst_t *inst);

#endif
