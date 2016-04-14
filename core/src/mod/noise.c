#include "../common.h"


/**
 * Random storage structure.
 *   @x, y, z, w: Storage data.
 */
struct m_rand_t {
	uint32_t x, y, z, w;
};

/**
 * Initialize a random number generator.
 *   @seed: The seed.
 *   &returns; The random generator.
 */
struct m_rand_t m_rand_init(uint32_t seed)
{
	return (struct m_rand_t){ 123456789, 362436069, 521288629, seed * 1049141 };
}

/**
 * Retrieve the next number from the generator.
 *   @rand: The number generator.
 *   &returns: The next number.
 */
uint32_t m_rand_next(struct m_rand_t *rand)
{
	  uint32_t t;

	  t = rand->x ^ (rand->x << 11);
	  rand->x = rand->y;
	  rand->y = rand->z;
	  rand->z = rand->w;
	  rand->w = rand->w ^ (rand->w >> 19) ^ (t ^ (t >> 8));

	  return rand->w;
}

/**
 * Retrieve a random number between the values zero and one.
 *   @rand: The number generator.
 *   &returns: The next number.
 */
double m_rand_d(struct m_rand_t *rand)
{
	return m_rand_next(rand) / (double)UINT32_MAX;
}


/**
 * Noise structure.
 *   @rand: The random number generator.
 */
struct amp_noise_t {
	struct m_rand_t rand;
};


/*
 * global variables
 */
const struct amp_module_i amp_noise_iface = {
	(amp_info_f)amp_noise_info,
	(amp_module_f)amp_noise_proc,
	(amp_copy_f)amp_noise_copy,
	(amp_delete_f)amp_noise_delete
};


/**
 * Create a noise.
 *   &returns: The noise.
 */
struct amp_noise_t *amp_noise_new(void)
{
	struct amp_noise_t *noise;

	noise = malloc(sizeof(struct amp_noise_t));
	noise->rand = m_rand_init(1);

	return noise;
}

/**
 * Copy a noise.
 *   @noise: The original noise.
 *   &returns: The copied noise.
 */
struct amp_noise_t *amp_noise_copy(struct amp_noise_t *noise)
{
	return amp_noise_new();
}

/**
 * Delete a noise.
 *   @noise: The noise.
 */
void amp_noise_delete(struct amp_noise_t *noise)
{
	free(noise);
}


/**
 * Create a noise from a value.
 *   @value: The value.
 *   @env: The environment.
 *   @err: The error.
 *   &returns: The value or null.
 */
struct ml_value_t *amp_noise_make(struct ml_value_t *value, struct ml_env_t *env, char **err)
{
	enum ml_value_e type = value->type;

	ml_value_delete(value);

	if(type != ml_value_nil_e) {
		*err = amp_printf("Type error. Expected '()'.");
		return NULL;
	}

	return amp_pack_module((struct amp_module_t){ amp_noise_new(), &amp_noise_iface });
}


/**
 * Handle information on a noise.
 *   @noise: The noise.
 *   @info: The information.
 */
void amp_noise_info(struct amp_noise_t *noise, struct amp_info_t info)
{
}

/**
 * Process a noise.
 *   @noise: The noise.
 *   @buf: The buffer.
 *   @time: The time.
 *   @len: The length.
 *   @queue: The action queue.
 *   &returns: The continuation flag.
 */
bool amp_noise_proc(struct amp_noise_t *noise, double *buf, struct amp_time_t *time, unsigned int len, struct amp_queue_t *queue)
{
	unsigned int i;
	
	for(i = 0; i < len; i++)
		buf[i] = 2.0 * m_rand_d(&noise->rand) - 1.0;

	return false;
}
