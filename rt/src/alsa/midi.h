#ifndef ALSA_MIDI_H
#define ALSA_MIDI_H

/*
 * midi declarations
 */

extern struct amp_midi_i alsa_midi_iface;

int alsa_midi_find(const char *id);

struct alsa_midi_t *alsa_midi_open(const char *conf, amp_midi_f func, void *arg);
void alsa_midi_close(struct alsa_midi_t *midi);

#endif
