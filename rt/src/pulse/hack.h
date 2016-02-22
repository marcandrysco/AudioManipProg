#ifndef PULSE_INTEROP_H
#define PULSE_INTEROP_H

/*
 * hack declarations
 */

struct pulse_hack_t;

struct pulse_hack_t *pulse_hack_new(unsigned int rate);
void pulse_hack_delete(struct pulse_hack_t *hack);

void pulse_hack_reset(struct pulse_hack_t *hack);
void pulse_hack_proc(struct pulse_hack_t *hack, double **buf, unsigned int len);

#endif
