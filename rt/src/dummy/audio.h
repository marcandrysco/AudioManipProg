#ifndef DUMMY_AUDIO_H
#define DUMMY_AUDIO_H

/*
 * dummy declarations
 */
extern const struct amp_audio_i dummy_audio_iface;

struct dummy_audio_t *dummy_audio_open(const char *conf);
void dummy_audio_close(struct dummy_audio_t *audio);

void dummy_audio_exec(struct dummy_audio_t *audio, amp_audio_f func, void *arg);
void dummy_audio_halt(struct dummy_audio_t *audio);

struct amp_audio_info_t dummy_audio_info(struct dummy_audio_t *audio);

#endif
