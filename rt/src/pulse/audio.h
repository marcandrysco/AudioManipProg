#ifndef PULSE_AUDIO_H
#define PULSE_AUDIO_H

/*
 * pulse audio declarations
 */

extern const struct amp_audio_i pulse_audio_iface;

struct pulse_audio_t *pulse_audio_open(const char *conf);
void pulse_audio_close(struct pulse_audio_t *audio);

void pulse_audio_exec(struct pulse_audio_t *audio, amp_audio_f func, void *arg);
void pulse_audio_halt(struct pulse_audio_t *audio);

#endif
