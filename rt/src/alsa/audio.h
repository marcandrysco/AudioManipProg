#ifndef ALSA_AUDIO_H
#define ALSA_AUDIO_H

/*
 * alsa declarations
 */

extern const struct amp_audio_i alsa_audio_iface;

struct alsa_audio_t *alsa_audio_open(const char *conf);
void alsa_audio_close(struct alsa_audio_t *audio);

void alsa_audio_exec(struct alsa_audio_t *audio, amp_audio_f func, void *arg);
void alsa_audio_halt(struct alsa_audio_t *audio);

#endif
