#ifndef SEQ_PLAYER_H
#define SEQ_PLAYER_H

/*
 * player declarations
 */

struct amp_player_t;

extern const struct amp_seq_i amp_player_iface;

struct amp_player_t *amp_player_new(void);
struct amp_player_t *amp_player_copy(struct amp_player_t *player);
void amp_player_delete(struct amp_player_t *player);

char *amp_player_make(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env);
struct ml_value_t *amp_player_make0(struct ml_value_t *value, struct ml_env_t *env, char **err);

void amp_player_add(struct amp_player_t *player, struct amp_time_t time, double len, struct amp_event_t event);

void amp_player_info(struct amp_player_t *player, struct amp_info_t info);
void amp_player_proc(struct amp_player_t *player, struct amp_time_t *time, unsigned int len, struct amp_queue_t *queue);

struct amp_player_inst_t *amp_player_first(struct amp_player_t *player);
struct amp_player_inst_t *amp_player_next(struct amp_player_inst_t *inst);
struct amp_player_inst_t *amp_player_atleast(struct amp_player_t *player, struct amp_time_t time);

#endif
