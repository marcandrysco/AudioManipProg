#include "common.h"


/**
 * Audit structure.
 *   @serv: The server.
 *   @id: The identifier.
 */
struct web_audit_t {
	struct web_serv_t *serv;
	const char *id;

	struct amp_tuner_t *tuner;
};


/*
 * local declarations
 */
static void audit_proc(struct io_file_t file, void *arg);


/**
 * Create a new audit.
 *   @serv: The server.
 *   @id: The indetifier.
 *   &returns: The audit.
 */
struct web_audit_t *web_audit_new(struct web_serv_t *serv, const char *id)
{
	struct web_audit_t *audit;

	audit = malloc(sizeof(struct web_audit_t));
	audit->serv = serv;
	audit->id = id;

	audit->tuner = amp_tuner_new(50, 3, 96000);

	return audit;
}

/**
 * Create an audit from a value.
 *   @ret: Ref. The returned value.
 *   @value: The value.
 *   @env: The environment.
 *   &returns: Error.
 */
char *web_audit_make(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env)
{
	if(value->type != ml_value_str_v)
		return mprintf("WebAudit takes a string as input.");

	*ret = amp_pack_effect(amp_effect(web_serv_audit(web_serv, value->data.str), &web_iface_effect));
	return NULL;
}

/**
 * Delete an audit.
 *   @audit: The audit.
 */
void web_audit_delete(struct web_audit_t *audit)
{
	amp_tuner_delete(audit->tuner);
	free(audit);
}


/**
 * Process information on an audit.
 *   @audit: The audit.
 *   @info: The information.
 */
void web_audit_info(struct web_audit_t *audit, struct amp_info_t info)
{
}

/**
 * Process data on an audit.
 *   @audit: The audit.
 *   @buf: The buffer.
 *   @time: The time.
 *   @len: The length.
 *   @queue: The action queue.
 *   &returns: The continuation flag.
 */
bool web_audit_proc(struct web_audit_t *audit, double *buf, struct amp_time_t *time, unsigned int len, struct amp_queue_t *queue)
{
	//static int a = 0;

	for(int i = 0; i < len; i++) {
		amp_tuner_proc(audit->tuner, buf[i]);
		//amp_tuner_proc(audit->tuner, sinf(2*M_PI*200*a++/96000));
	}

	//printf("%10.4f\n", audit->tuner->tone[0].f);

	return false;
}


/**
 * Retrieve the information from an audit.
 *   @audit: The audit.
 *   @file: The file.
 */
void web_audit_print(struct web_audit_t *audit, struct io_file_t file)
{
	static const float arr[] = { 1.2, 3.4, -0.9, 1e7 };
	char tmp[b64_enclen(sizeof(arr)) + 1];

	b64_enc(tmp, arr, sizeof(arr));
	hprintf(file, "{\"data\":\"%s\"}", tmp);
}

/**
 * Create a chunk for the audit.
 *   @audit: The audit.
 *   &returns: The chunk.
 */
struct io_chunk_t web_audit_chunk(struct web_audit_t *audit)
{
	return (struct io_chunk_t){ audit_proc, audit };
}
static void audit_proc(struct io_file_t file, void *arg)
{
	web_audit_print(arg, file);
}


/**
 * Load an audit from disk.
 *   @audit: The audit.
 *   &returns: Error.
 */
char *web_audit_load(struct web_audit_t *audit)
{
	return NULL;
}

/**
 * Save an audit to disk.
 *   @audit: The audit.
 *   &returns: Error.
 */
char *web_audit_save(struct web_audit_t *audit)
{
	return NULL;
}


/**
 * Create a tuner.
 *   @lpf: The low-pass frequency.
 *   @n: The number of frequencies.
 *   @rate: The sample rate.
 *   &returns: The tuner.
 */
struct amp_tuner_t *amp_tuner_new(double lpf, unsigned int n, unsigned int rate)
{
	unsigned int i;
	struct amp_tuner_t *tuner;

	tuner = malloc(n * sizeof(struct amp_tone_t) + sizeof(struct amp_tuner_t));
	tuner->vol = dsp_vol_init(lpf, rate);
	tuner->tm = 1;
	tuner->last = 0.0;
	tuner->len = n;
	tuner->rate = rate;

	for(i = 0; i < n; i++)
		tuner->tone[i] = (struct amp_tone_t){ 0.0, 0.0, 0.0, 0.0, { 0.0, 0.0 } };

	return tuner;
}

/**
 * Delete a tuner.
 *   @tuner: The tuner.
 */
void amp_tuner_delete(struct amp_tuner_t *tuner)
{
	free(tuner);
}

void amp_tuner_proc(struct amp_tuner_t *tuner, double v)
{
	unsigned int i;

	for(i = 0; i < tuner->len; i++) {
		double w, m, t, s;
		struct amp_tone_t *tone = &tuner->tone[i];

		if(tone->g < 0.0001)
			continue;

		s = dsp_res_proc(v, dsp_res_init(tone->f, 1.0, tuner->rate), tone->s);
		w = tone->g * dsp_osc_sine_f(tone->t);

		if(fabs(s - w) > (tone->g)) {
			*tone = (struct amp_tone_t){ 0.0, 0.0, 0.0, 0.0, { 0.0, 0.0 } };
			continue;
		}

		/* adjust gain */
		m = dsp_osc_tri(tone->t);
		t = s - w;
		t = 0.005 * t * m;
		tone->g += t;

		/* adjust freq */
		m = dsp_osc_tri(tone->t + 0.25);
		t = (s - w) * m;
		tone->f += t * 0.03;
		tone->t = dsp_osc_inc(tone->t, t * 0.03);

		tone->t = dsp_osc_inc(tone->t, dsp_osc_step(tone->f, tuner->rate));

		v -= w;

		if((tone->f < 20) || (tone->f > 10000))
			tone->g = 0.0;
	}

	if((tuner->last <= 0.0) && (v > 0.0)) {
		double freq = (double)tuner->rate / (double)tuner->tm;

		if(freq < 8000.0) {
			for(i = 0; i < tuner->len; i++) {
				if(tuner->tone[i].g < (tuner->vol.v * 0.25))
					break;
			}

			if(i < tuner->len) {
				tuner->tone[i] = (struct amp_tone_t){ 0.0, 1.0, freq, tuner->vol.v, { 0.0, 0.0 } };
			}
		}

		tuner->tm = 1;
	}
	else
		tuner->tm++;

	tuner->last = v;
	dsp_vol_proc(&tuner->vol, v);
}
