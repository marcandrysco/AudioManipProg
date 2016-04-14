#ifndef SEQ_SNAP_H
#define SEQ_SNAP_H

/*
 * snap declarations
 */
struct amp_snap_t;

extern const struct amp_seq_i amp_snap_iface;

struct amp_snap_t *amp_snap_new(struct amp_time_t pt, struct amp_time_t mod);
struct amp_snap_t *amp_snap_copy(struct amp_snap_t *snap);
void amp_snap_delete(struct amp_snap_t *snap);

char *amp_snap_make(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env);

void amp_snap_add(struct amp_snap_t *snap, struct amp_id_t id);
void amp_snap_info(struct amp_snap_t *snap, struct amp_info_t info);
void amp_snap_proc(struct amp_snap_t *snap, struct amp_time_t *time, unsigned int len, struct amp_queue_t *queue);

#endif
