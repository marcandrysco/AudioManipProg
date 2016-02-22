#ifndef PULSE_CONN_H
#define PULSE_CONN_H

/**
 * Pulse configuration.
 *   @in, out, rate: The number of inputs/outputs and sampling rate.
 *   @lat: The latency in milliseconds.
 */

struct pulse_conf_t {
	unsigned int in, out, rate;
	float lat;
};

/*
 * pulse connection declarations
 */

struct pulse_conn_t;

struct pulse_conn_t *pulse_conn_open(const char *name, amp_audio_f func, void *arg, const struct pulse_conf_t *conf);
void pulse_conn_close(struct pulse_conn_t *conn);


#endif
