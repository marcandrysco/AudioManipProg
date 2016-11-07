#ifndef AUDIT_H
#define AUDIT_H

/*
 * audit declarations
 */
struct web_audit_t *web_audit_new(struct web_serv_t *serv, const char *id);
char *web_audit_make(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env);
void web_audit_delete(struct web_audit_t *audit);

void web_audit_info(struct web_audit_t *audit, struct amp_info_t info);
bool web_audit_proc(struct web_audit_t *audit, double *buf, struct amp_time_t *time, unsigned int len, struct amp_queue_t *queue);

void web_audit_print(struct web_audit_t *audit, struct io_file_t file);
struct io_chunk_t web_audit_chunk(struct web_audit_t *audit);

char *web_audit_load(struct web_audit_t *audit);
char *web_audit_save(struct web_audit_t *audit);



/**
 * Tone structure.
 *   @t, q, f, g, s: The time, quality, frequency, gain, and filter state.
 */
struct amp_tone_t {
	double t, q, f, g, s[2];
};

/**
 * Tuner structure.
 *   @vol: The volume.
 *   @last: The last value.
 *   @tm, rate: Time since previous crossing and smaple rate.
 *   @len: The tone array length.
 *   @tone: The tone array.
 */
struct amp_tuner_t {
	struct dsp_vol_t vol;
	double last;
	unsigned int tm, rate;

	unsigned int len;
	struct amp_tone_t tone[];
};

struct amp_tuner_t *amp_tuner_new(double lpf, unsigned int n, unsigned int rate);
void amp_tuner_delete(struct amp_tuner_t *tuner);
void amp_tuner_proc(struct amp_tuner_t *tuner, double v);

#endif
