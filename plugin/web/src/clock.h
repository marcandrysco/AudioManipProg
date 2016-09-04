#ifndef CLOCK_H
#define CLOCK_H

/*
 * clock declarations
 */
struct web_clock_t;

struct web_clock_t *web_clock_new(const char *id);
void web_clock_delete(struct web_clock_t *clock);

#endif
