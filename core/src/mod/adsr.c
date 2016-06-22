#include "../common.h"


/**
 * ADSR structure.
 *   @on: The note on flag.
 *   @min, ax: The minimum and maximum.
 *   @v, atk, decay, sus, rel, target: The paramaters.
 */
struct amp_adsr_t {
	bool on;
	double min, max;
	double v, atk, decay, sus, rel, target[2];
};


/*
 * global variables
 */
const struct amp_module_i amp_adsr_iface = {
	(amp_info_f)amp_adsr_info,
	(amp_module_f)amp_adsr_proc,
	(amp_copy_f)amp_adsr_copy,
	(amp_delete_f)amp_adsr_delete
};


/**
 * Create an ADSR.
 *   @min: The minimum.
 *   @max: The maximum.
 *   @atk: The attack.
 *   @decay: The decay length.
 *   @sus: The sustain level.
 *   @rel: The release length.
 *   @rate: The sample rate.
 *   &returns: The ADSR.
 */
struct amp_adsr_t *amp_adsr_new(double min, double max, double atk, double decay, double sus, double rel, unsigned int rate)
{
	struct amp_adsr_t *adsr;

	sus = fmax(sus, 0.01);

	adsr = malloc(sizeof(struct amp_adsr_t));
	adsr->min = min;
	adsr->max = max;
	adsr->on = false;
	adsr->v = 0.01;
	adsr->atk = pow(100.0, 1.0 / (atk * rate));
	adsr->decay = pow(sus, 1.0 / (decay * rate));
	adsr->rel = pow((sus == 0.01) ? 0.01 : (0.01 / sus), 1.0 / (rel * rate));
	adsr->sus = sus;
	adsr->target[0] = 0.01;
	adsr->target[1] = 0.01;

	return adsr;
}

/**
 * Copy an ADSR.
 *   @adsr: The original ADSR.
 *   &returns: The copied ADSR.
 */
struct amp_adsr_t *amp_adsr_copy(struct amp_adsr_t *adsr)
{
	struct amp_adsr_t *copy;

	copy = malloc(sizeof(struct amp_adsr_t));
	*copy = *adsr;

	return copy;
}

/**
 * Delete an ADSR.
 *   @adsr: The ADSR.
 */
void amp_adsr_delete(struct amp_adsr_t *adsr)
{
	free(adsr);
}


/**
 * Create an ADSR from a value.
 *   @value: The value.
 *   @env: The environment.
 *   @err: The error.
 *   &returns: The value or null.
 */
char *amp_adsr_make(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env)
{
#define onexit
	double min, max, atk, decay, sus, rel;

	chkfail(amp_match_unpack(value, "((f,f),(f,f,f,f))", &min, &max, &atk, &decay, &sus, &rel));

	*ret = amp_pack_module((struct amp_module_t){ amp_adsr_new(min, max, atk, decay, sus, rel, amp_core_rate(env)), &amp_adsr_iface });
	return NULL;
#undef onexit
}


/**
 * Handle information on an ADSR.
 *   @adsr: The ADSR.
 *   @info: The information.
 */
void amp_adsr_info(struct amp_adsr_t *adsr, struct amp_info_t info)
{
	if(info.type == amp_info_note_e) {
		double vel;
		
		if(info.data.note->vel > 0.0) {
			vel = adsr->min + (adsr->max - adsr->min) * info.data.note->vel;
			adsr->on = false;
		}
		else {
			vel = 0.0;
			adsr->on = true;
		}

		adsr->target[0] = fmax(vel, 0.01);
		adsr->target[1] = fmax(vel * adsr->sus, 0.01);
	}
}

/**
 * Process an ADSR.
 *   @adsr: The ADSR.
 *   @buf: The buffer.
 *   @time: The time.
 *   @len: The length.
 *   @queue: The action queue.
 *   &returns: The continuation flag.
 */
bool amp_adsr_proc(struct amp_adsr_t *adsr, double *buf, struct amp_time_t *time, unsigned int len, struct amp_queue_t *queue)
{
	double v;
	unsigned int i;

	v = adsr->v;

	for(i = 0; i < len; i++) {
		if(v < adsr->target[0]) {
			v *= adsr->atk;
			if(v >= adsr->target[0]) {
				v = adsr->target[0];
				adsr->target[0] = adsr->target[1];
			}
		}
		else if(v > adsr->target[0]) {
			v *= adsr->on ? adsr->rel : adsr->decay;
			if(v <= adsr->target[0]) {
				v = adsr->target[0];
				adsr->target[0] = adsr->target[1];
			}
		}

		buf[i] = v - 0.01;
	}

	adsr->v = v;

	return (v > 0.01);
}
