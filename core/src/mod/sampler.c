#include "../common.h"


/**
 * Sampler structure.
 *   @head, tail: The head and tail instances.
 */

struct amp_sampler_t {
	struct inst_t *head, *tail;
};

/**
 * Play structure.
 *   @vol: The volume.
 *   @len: The length.
 */

struct play_t {
	double vol;
	unsigned int len;
};

/**
 * Instance structure.
 *   @buf: The sample buffer.
 *   @dev, key: The device and key.
 *   @prev, next: The previous and next instances.
 *   @n: The number of players.
 *   @play: The player array.
 */

struct inst_t {
	struct dsp_buf_t *buf;
	uint16_t dev, key;

	struct inst_t *prev, *next;

	unsigned int n;
	struct play_t play[];
};


/**
 * Create a sampler.
 *   &returns: The sampler.
 */

struct amp_sampler_t *amp_sampler_new(void)
{
	struct amp_sampler_t *sampler;

	sampler = malloc(sizeof(struct amp_sampler_t));
	sampler->head = sampler->tail = NULL;

	return sampler;
}

/**
 * Copy a sampler.
 *   @sampler: The original sampler.
 *   &returns: The copied sampler.
 */

struct amp_sampler_t *amp_sampler_copy(struct amp_sampler_t *sampler)
{
	return amp_sampler_new();
}

/**
 * Delete a sampler.
 *   @sampler: The sampler.
 */

void amp_sampler_delete(struct amp_sampler_t *sampler)
{
	free(sampler);
}


/**
 * Create a sampler from a value.
 *   @value: The value.
 *   @env: The environment.
 *   @err: The error.
 *   &returns: The value or null.
 */

struct ml_value_t *amp_sampler_make(struct ml_value_t *value, struct ml_env_t *env, char **err)
{
	fprintf(stderr, "stub"), abort();
}


/**
 * Handle information on a sampler.
 *   @sampler: The sampler.
 *   @info: The information.
 */

void amp_sampler_info(struct amp_sampler_t *sampler, struct amp_info_t info)
{
}

/**
 * Process a sampler.
 *   @sampler: The sampler.
 *   @buf: The buffer.
 *   @time: The time.
 *   @len: The length.
 *   &returns: The continuation flag.
 */

bool amp_sampler_proc(struct amp_sampler_t *sampler, double *buf, struct amp_time_t *time, unsigned int len)
{
	return false;
}
