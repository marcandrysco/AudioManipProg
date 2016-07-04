#include "../common.h"


/**
 * Continue structure.
 */
struct amp_cont_t {
};


/*
 * global variables
 */
const struct amp_effect_i amp_cont_iface = {
	(amp_info_f)amp_cont_info,
	(amp_effect_f)amp_cont_proc,
	(amp_copy_f)amp_cont_copy,
	(amp_delete_f)amp_cont_delete
};


/**
 * Create a continue.
 *   &returns: The continue.
 */
struct amp_cont_t *amp_cont_new(void)
{
	struct amp_cont_t *cont;

	cont = malloc(sizeof(struct amp_cont_t));

	return cont;
}

/**
 * Copy a continue.
 *   @cont: The original continue.
 *   &returns: The copied continue.
 */
struct amp_cont_t *amp_cont_copy(struct amp_cont_t *cont)
{
	return amp_cont_new();
}

/**
 * Delete a continue.
 *   @cont: The continue.
 */
void amp_cont_delete(struct amp_cont_t *cont)
{
	free(cont);
}


/**
 * Create a continue from a value.
 *   @ret: Ref. The return value.
 *   @value: The value.
 *   @env: The environment.
 *   &returns: Error
 */
char *amp_cont_make(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env)
{
	if(value->type != ml_value_nil_v)
		return mprintf("%C: Expected '()'.", ml_tag_chunk(&value->tag));

	*ret = amp_pack_effect((struct amp_effect_t){ amp_cont_new(), &amp_cont_iface });

	return NULL;
}


/**
 * Handle information on a continue.
 *   @cont: The continue.
 *   @info: The information.
 */
void amp_cont_info(struct amp_cont_t *cont, struct amp_info_t info)
{
}

/**
 * Process a continue.
 *   @cont: The continue.
 *   @buf: The buffer.
 *   @time: The time.
 *   @len: The length.
 *   @queue: The action queue.
 *   &returns: The continuation flag.
 */
bool amp_cont_proc(struct amp_cont_t *cont, double *buf, struct amp_time_t *time, unsigned int len, struct amp_queue_t *queue)
{
	unsigned int i;
	bool flag= false;

	for(i = 0; i < len; i++) {
		flag |= (fabs(buf[i]) > 1e-6);
	}

	return flag;
}
