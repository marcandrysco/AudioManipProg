#ifndef INSTR_SERIES_H
#define INSTR_MIXER_H

/**
 * Series structure.
 *   @head, tail: The head and tail instances.
 */
struct amp_series_t {
	struct amp_series_inst_t *head, *tail;
};

/**
 * Instance structure.
 *   @instr: The instrument.
 *   @prev, next: The previous and next instances.
 */
struct amp_series_inst_t {
	struct amp_instr_t instr;

	struct amp_series_inst_t *prev, *next;
};

/*
 * series declarations
 */
extern struct amp_instr_i amp_series_iface;

struct amp_series_t *amp_series_new(void);
struct amp_series_t *amp_series_copy(struct amp_series_t *series);
void amp_series_delete(struct amp_series_t *series);

char *amp_series_make(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env);

void amp_series_prepend(struct amp_series_t *series, struct amp_instr_t instr);
void amp_series_append(struct amp_series_t *series, struct amp_instr_t instr);

void amp_series_info(struct amp_series_t *series, struct amp_info_t info);
void amp_series_proc(struct amp_series_t *series, double **buf, struct amp_time_t *time, unsigned int len, struct amp_queue_t *queue);

#endif
