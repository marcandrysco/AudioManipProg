#ifndef EFX_SECT_H
#define EFX_SECT_H

/*
 * section declarations
 */

struct amp_sect_t *amp_sect_new(void);
struct amp_sect_t *amp_sect_copy(struct amp_sect_t *sect);
void amp_sect_delete(struct amp_sect_t *sect);

void amp_sect_prepend(struct amp_sect_t *sect, double mix, struct amp_effect_t effect);
void amp_sect_append(struct amp_sect_t *sect, double mix, struct amp_effect_t effect);

struct ml_value_t *amp_sect_make(struct ml_value_t *value, struct ml_env_t *env, char **err);

void amp_sect_info(struct amp_sect_t *sect, struct amp_info_t info);
void amp_sect_proc(struct amp_sect_t *sect, double *buf, struct amp_time_t *time, unsigned int len);

#endif
