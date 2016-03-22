#include "common.h"


/**
 * Create a blank environment.
 *   &returns: The environment.
 */
struct ml_env_t *ml_env_new(void)
{
	return NULL;
}

/**
 * Copy an environemnt.
 *   @env: The original environment.
 *   &returns: The copy.
 */
struct ml_env_t *ml_env_copy(struct ml_env_t *env)
{
	if(env != NULL)
		env->refcnt++;

	return env;
}

/**
 * Delete an environment.
 *   @env: The environment.
 */
void ml_env_delete(struct ml_env_t *env)
{
	struct ml_env_t *up;

	while(true) {
		if(env == NULL)
			break;
		else if(--env->refcnt > 0)
			break;

		up = env->up;

		ml_value_delete(env->value);
		free(env->id);
		free(env);

		env = up;
	}
}


/**
 * Process a file path against an environment.
 *   @path: The path.
 *   @env: The environment pointer.
 *   &returns: The error if failed, null if successful.
 */
char *ml_env_proc(const char *path, struct ml_env_t **env)
{
	char *err = NULL;
	struct ml_token_t *token;

	token = ml_token_load(path, &err);

	if(token == NULL)
		return err;

	err = ml_parse_top(token, env, path);
	ml_token_clean(token);

	return err;
}


/**
 * Add a binding to the environment.
 *   @env: The environment.
 *   @id: Consumed. The identifier.
 *   @value: Consumed. The value.
 */
void ml_env_add(struct ml_env_t **env, char *id, struct ml_value_t *value)
{
	struct ml_env_t *next;

	next = malloc(sizeof(struct ml_env_t));
	next->id = id;
	next->value = value;
	next->up = *env;
	next->refcnt = 1;

	*env = next;
}

/**
 * Look up an value in the environment.
 *   @env: The environment.
 *   @id: The identifier.
 *   &returns: The valueor null.
 */
struct ml_value_t *ml_env_lookup(struct ml_env_t *env, const char *id)
{
	while(env != NULL) {
		if(strcmp(env->id, id) == 0)
			return env->value;

		env = env->up;
	}

	return NULL;
}
