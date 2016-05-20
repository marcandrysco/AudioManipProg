#ifndef SEQ_RULE_H
#define SEQ_RULE_H

/*
 * rule declarations
 */
struct amp_rule_t;

extern const struct amp_seq_i amp_rule_iface;

struct amp_rule_t *amp_rule_new(void);
struct amp_rule_t *amp_rule_copy(struct amp_rule_t *rule);
void amp_delete(struct amp_rule_t *rule);

char *amp_rule_make(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env);

void amp_rule_info(struct amp_rule_t *rule, struct amp_info_t info);
void amp_rule_proc(struct amp_rule_t *rule, struct amp_time_t *time, unsigned int len, struct amp_queue_t *queue);

#endif
