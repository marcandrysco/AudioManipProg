#ifndef CTRL_H
#define CTRL_H

/*
 * controller declarations
 */
struct web_ctrl_t;
struct web_ctrl_inst_t;

struct web_ctrl_t *web_ctrl_new(struct amp_rt_t *rt, const char *id);
char *web_ctrl_make(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env);
void web_ctrl_delete(struct web_ctrl_t *ctrl);

void web_ctrl_add(struct web_ctrl_t *ctrl, struct amp_loc_t loc, struct web_ctrl_inst_t *inst, uint16_t val);

void web_ctrl_info(struct web_ctrl_t *ctrl, struct amp_info_t info);
bool web_ctrl_proc(struct web_ctrl_t *ctrl, struct amp_time_t *time, unsigned int len, struct amp_queue_t *queue);

void web_ctrl_print(struct web_ctrl_t *ctrl, struct io_file_t file);
bool web_ctrl_req(struct web_ctrl_t *ctrl, struct http_args_t *args, struct json_t *json);

char *web_ctrl_load(struct web_ctrl_t *ctrl);
char *web_ctrl_save(struct web_ctrl_t *ctrl);

#endif
