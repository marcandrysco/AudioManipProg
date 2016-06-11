#ifndef POLY_SHOT_H
#define POLY_SHOT_H

/*
 * one shot declarations
 */
extern const struct amp_instr_i amp_shot_iface_instr;
extern const struct amp_effect_i amp_shot_iface_effect;
extern const struct amp_module_i amp_shot_iface_module;

struct amp_shot_t *amp_shot_new(struct amp_id_t id, struct amp_box_t *box);
struct amp_shot_t *amp_shot_copy(struct amp_shot_t *shot);
void amp_shot_delete(struct amp_shot_t *shot);

char *amp_shot_make(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env);

void amp_shot_info(struct amp_shot_t *shot, struct amp_info_t info);
bool amp_shot_proc_instr(struct amp_shot_t *shot, double **buf, struct amp_time_t *time, unsigned int len, struct amp_queue_t *queue);
bool amp_shot_proc_effect(struct amp_shot_t *shot, double *buf, struct amp_time_t *time, unsigned int len, struct amp_queue_t *queue);
bool amp_shot_proc_module(struct amp_shot_t *shot, double *buf, struct amp_time_t *time, unsigned int len, struct amp_queue_t *queue);

#endif
