#include "common.h"


/**
 * Create a new binding.
 *   @id: Consumed. The identifier.
 *   @value: Consumed. The value.
 *   &returns: The binding.
 */

struct ml_bind_t *ml_bind_new(char *id, struct ml_value_t *value)
{
	struct ml_bind_t *bind;

	bind = malloc(sizeof(struct ml_bind_t));
	bind->id = id;
	bind->value = value;

	return bind;
}

/**
 * Delete a binding.
 *   @bind: The binding.
 */

void ml_bind_delete(struct ml_bind_t *bind)
{
	ml_value_delete(bind->value);
	free(bind->id);
	free(bind);
}


/**
 * Create a blank environment.
 *   &returns: The environment.
 */

struct ml_env_t *ml_env_new(void)
{
	struct ml_env_t *env;

	env = malloc(sizeof(struct ml_env_t));
	env->bind = NULL;

	return env;
}

/**
 * Copy an environemnt.
 *   @env: The original environment.
 *   &returns: The copy.
 */

struct ml_env_t *ml_env_copy(struct ml_env_t *env)
{
	struct ml_env_t *copy;
	struct ml_bind_t **bind, *cur;

	copy = malloc(sizeof(struct ml_env_t));
	bind = &copy->bind;

	for(cur = env->bind; cur != NULL; cur = cur->next) {
		*bind = ml_bind_new(strdup(cur->id), ml_value_copy(cur->value));
		bind = &(*bind)->next;
	}

	*bind = NULL;

	return copy;
}

/**
 * Delete an environment.
 *   @env: The environment.
 */

void ml_env_delete(struct ml_env_t *env)
{
	struct ml_bind_t *cur, *next;

	for(cur = env->bind; cur != NULL; cur = next) {
		next = cur->next;
		ml_bind_delete(cur);
	}

	free(env);
}


/**
 * Process a file path against an environment.
 *   @path: The path.
 *   @env: The environment.
 *   &returns: The error if failed, null if successful.
 */

char *ml_env_proc(const char *path, struct ml_env_t *env)
{
	char *err = NULL;
	struct ml_token_t *token;

	token = ml_token_load(path, &err);

	if(token == NULL)
		return err;

	ml_parse_top(token, env, &err);
	ml_token_clean(token);

	return err;
}


/**
 * Add a binding to the environment.
 *   @env: The environment.
 *   @id: Consumed. The identifier.
 *   @value: Consumed. The value.
 */

void ml_env_add(struct ml_env_t *env, char *id, struct ml_value_t *value)
{
	struct ml_bind_t *bind;

	bind = ml_bind_new(id, value);
	bind->next = env->bind;
	env->bind = bind;
}

/**
 * Look up an value in the environment.
 *   @env: The environment.
 *   @id: The identifier.
 *   &returns: The valueor null.
 */

struct ml_value_t *ml_env_lookup(struct ml_env_t *env, const char *id)
{
	struct ml_bind_t *bind;

	for(bind = env->bind; bind != NULL; bind = bind->next) {
		if(strcmp(bind->id, id) == 0)
			return bind->value;
	}

	return NULL;
}
