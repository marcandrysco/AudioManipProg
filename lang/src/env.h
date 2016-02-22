#ifndef ENV_H
#define ENV_H

/**
 * Binding structure.
 *   @id: The identifier.
 *   @value: The value.
 *   @next: The next binding.
 */

struct ml_bind_t {
	char *id;

	struct ml_value_t *value;
	struct ml_bind_t *next;
};

/**
 * Environment structure.
 *   @bind: The bindings.
 *   @impl: The implementations.
 */

struct ml_env_t {
	struct ml_bind_t *bind;
};


/*
 * bind declarations
 */

struct ml_bind_t *ml_bind_new(char *id, struct ml_value_t *value);
void ml_bind_delete(struct ml_bind_t *bind);

/*
 * environment declarations
 */

struct ml_env_t;

struct ml_env_t *ml_env_new(void);
struct ml_env_t *ml_env_copy(struct ml_env_t *env);
void ml_env_delete(struct ml_env_t *env);

char *ml_env_proc(const char *path, struct ml_env_t *env);

void ml_env_add(struct ml_env_t *env, char *id, struct ml_value_t *value);
struct ml_value_t *ml_env_lookup(struct ml_env_t *env, const char *id);

#endif
