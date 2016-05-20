#ifndef EFX_CLIP_H
#define EFX_CLIP_H

/*
 * clip declarations
 */
struct amp_clip_t *amp_clip_new(struct amp_param_t *maxlo, struct amp_param_t *satlo, struct amp_param_t *sathi, struct amp_param_t *maxhi);
struct amp_clip_t *amp_clip_copy(struct amp_clip_t *clip);
void amp_clip_delete(struct amp_clip_t *clip);

char *amp_clip_make(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env);

void amp_clip_info(struct amp_clip_t *clip, struct amp_info_t info);
bool amp_clip_proc(struct amp_clip_t *clip, double *buf, struct amp_time_t *time, unsigned int len, struct amp_queue_t *queue);

#endif
