#include "common.h"


/**
 * Clock structure.
 *   @id: The identifier.
 */
struct web_clock_t {
	const char *id;
};

/**
 * Clock instance structure.
 *   @bpm: The number of beats per minute.
 *   @nbars: The number of bars.
 *   @nbeats: The number of beats per measure.
 *   @next: The next instance.
 */
struct web_clock_inst_t {
	double bpm;
	unsigned int nbars;
	double nbeats;

	struct web_clock_inst_t *next;
};


/**
 * Create a new clock.
 *   @id: The identifier.
 *   &returns: The clock.
 */
struct web_clock_t *web_clock_new(const char *id)
{
	struct web_clock_t *clock;

	clock = malloc(sizeof(struct web_clock_t));
	clock->id = id;

	return clock;
}

/**
 * Delete a clock.
 *   @clock: The clock.
 */
void web_clock_delete(struct web_clock_t *clock)
{
	free(clock);
}
