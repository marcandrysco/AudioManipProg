#ifndef EFX_SECT_H
#define EFX_SECT_H

/**
 * Section structure.
 *   @head, tail: The head and tail.
 */
struct amp_sect_t {
	struct amp_sect_inst_t *head, *tail;
};

/**
 * Instance structure.
 *   @mix: The mix.
 *   @effect: The effect.
 *   @prev, next: The previous and next instances.
 */
struct amp_sect_inst_t {
	double mix;
	struct amp_effect_t effect;
	struct amp_sect_inst_t *prev, *next;
};

/*
 * section declarations
 */
extern const struct amp_effect_i amp_sect_iface;

struct amp_sect_t *amp_sect_new(void);
struct amp_sect_t *amp_sect_copy(struct amp_sect_t *sect);
void amp_sect_delete(struct amp_sect_t *sect);

void amp_sect_prepend(struct amp_sect_t *sect, struct amp_effect_t effect);
void amp_sect_append(struct amp_sect_t *sect, struct amp_effect_t effect);

struct ml_value_t *amp_sect_make(struct ml_value_t *value, struct ml_env_t *env, char **err);

void amp_sect_info(struct amp_sect_t *sect, struct amp_info_t info);
bool amp_sect_proc(struct amp_sect_t *sect, double *buf, struct amp_time_t *time, unsigned int len, struct amp_queue_t *queue);

#endif
