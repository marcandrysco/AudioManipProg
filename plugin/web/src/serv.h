#ifndef SERV_H
#define SERV_H

/**
 * Instance type enumerator.
 *   @inst_mach_v: Machine.
 *   @inst_player_v: Player piano.
 */
enum web_inst_e {
	web_mach_v,
	web_player_v
};


/*
 * server declarations
 */
struct web_serv_t *web_serv_new(void);
void web_serv_delete(struct web_serv_t *serv);

bool web_serv_req(struct web_serv_t *serv, const char *path, struct http_args_t *args);

struct web_inst_t *web_inst_first(struct web_serv_t *serv);
struct web_inst_t *web_inst_next(struct web_inst_t *inst);
struct web_inst_t *web_serv_lookup(struct web_serv_t *serv, const char *id);

struct web_inst_t *web_serv_mach(struct web_serv_t *serv, const char *id);
struct web_inst_t *web_serv_player(struct web_serv_t *serv, const char *id);

/*
 * instance declarations
 */
extern const struct amp_seq_i web_iface_seq;

struct web_inst_t *web_inst_ref(struct web_inst_t *inst);
void web_inst_unref(struct web_inst_t *inst);

void web_inst_info(struct web_inst_t *inst, struct amp_info_t info);
bool web_inst_seq(struct web_inst_t *inst, struct amp_time_t *time, unsigned int len, struct amp_queue_t *queue);

const char *web_inst_type(enum web_inst_e type);
struct io_chunk_t web_inst_chunk(struct web_inst_t *inst);

#endif
