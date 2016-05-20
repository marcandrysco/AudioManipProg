#include "../common.h"


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

	if(type != ml_value_nil_v) {
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
