#ifndef CORE_H
#define CORE_H

/**
 * Core structure.
 *   @env: The environemnt.
 */

struct amp_core_t {
	struct ml_env_t *env;
};


/*
 * core declarations
 */

struct amp_core_t *amp_core_new(unsigned int rate);
void amp_core_delete(struct amp_core_t *core);

struct ml_env_t *amp_core_eval(struct amp_core_t *core, const char *path, char **err);

unsigned int amp_core_rate(struct ml_env_t *env);

#endif
