#ifndef CLK_BASIC_H
#define CLK_BASIC_H

/*
 * basic clock declarations
 */
struct amp_basic_t;

extern const struct amp_clock_i amp_basic_iface;

struct amp_basic_t *amp_basic_new(double bpm, double nbeats, unsigned int rate);
struct amp_basic_t *amp_basic_copy(struct amp_basic_t *basic);
void amp_basic_delete(struct amp_basic_t *basic);

char *amp_basic_make(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env);

void amp_basic_seek(struct amp_basic_t *basic, double bar);

void amp_basic_info(struct amp_basic_t *basic, struct amp_info_t info);
void amp_basic_proc(struct amp_basic_t *basic, struct amp_time_t *time, unsigned int len);


/**
 * Create a clock instance from a basic clock.
 *   @basic: The basic clock.
 *   &returns: The clock instance.
 */
static inline struct amp_clock_t amp_basic_clock(struct amp_basic_t *basic)
{
	return (struct amp_clock_t){ basic, &amp_basic_iface };
}

#endif
