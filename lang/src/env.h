#ifndef ENV_H
#define ENV_H

/**
 * Environment structure.
 *   @id: The identifier.
 *   @value: The value.
 *   @refcnt: The reference count.
 *   @up: The previous environment.
 */

struct ml_env_t {
	char *id;
	struct ml_value_t *value;

	unsigned int refcnt;
	struct ml_env_t *up;
};


/*
 * environment declarations
 */

struct ml_env_t;

struct ml_env_t *ml_env_new(void);
struct ml_env_t *ml_env_copy(struct ml_env_t *env);
void ml_env_delete(struct ml_env_t *env);

char *ml_env_proc(const char *path, struct ml_env_t **env);

void ml_env_add(struct ml_env_t **env, char *id, struct ml_value_t *value);
struct ml_value_t *ml_env_lookup(struct ml_env_t *env, const char *id);

#endif
