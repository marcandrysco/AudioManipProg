#ifndef PLAYER_H
#define PLAYER_H

/*
 * player declarations
 */
struct web_player_t;
struct web_serv_t;

struct web_player_t *web_player_new(struct web_serv_t *serv, const char *id);
char *web_player_make(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env);
void web_player_delete(struct web_player_t *player);

struct web_player_inst_t *web_player_lookup(struct web_player_t *player, struct amp_loc_t begin, struct amp_loc_t end, uint16_t key);
void web_player_add(struct web_player_t *player, struct amp_loc_t begin, struct amp_loc_t end, uint16_t key, uint16_t vel);
void web_player_remove(struct web_player_t *player, struct amp_loc_t begin, struct amp_loc_t end, uint16_t key);

void web_player_info(struct web_player_t *player, struct amp_info_t info);
bool web_player_proc(struct web_player_t *player, struct amp_time_t *time, unsigned int len, struct amp_queue_t *queue);

void web_player_print(struct web_player_t *player, struct io_file_t file);
struct io_chunk_t web_player_chunk(struct web_player_t *player);

bool web_player_req(struct web_player_t *player, struct http_args_t *args, struct json_t *json);

char *web_player_load(struct web_player_t *player);
void web_player_save(struct web_player_t *player);

/*
 * player configuration declarations
 */
struct web_player_conf_t;

void web_player_conf_init(struct web_player_conf_t *conf);
void web_player_conf_destroy(struct web_player_conf_t *conf);

#endif
