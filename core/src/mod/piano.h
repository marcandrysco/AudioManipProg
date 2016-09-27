#ifndef MOD_PIANO_H
#define MOD_PIANO_H

/*
 * piano declarations
 */
struct amp_piano_t *amp_piano_new(uint16_t dev, unsigned int simul);
struct amp_piano_t *amp_piano_copy(struct amp_piano_t *piano);
void amp_piano_delete(struct amp_piano_t *piano);

char *amp_piano_make(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env);

struct amp_piano_key_t *amp_piano_key(struct amp_piano_t *piano, uint16_t num);
struct amp_piano_vel_t *amp_piano_vel(struct amp_piano_key_t *key);
void amp_piano_rr(struct amp_piano_vel_t *vel, struct amp_file_t *file);

void amp_piano_info(struct amp_piano_t *piano, struct amp_info_t info);
bool amp_piano_proc(struct amp_piano_t *piano, double *buf, struct amp_time_t *time, unsigned int len, struct amp_queue_t *queue);

#endif
