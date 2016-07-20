#ifndef POLY_INJECT_H
#define POLY_INJECT_H

/*
 * inject declarations
 */
struct amp_poly_t;
struct amp_polyinfo_t;

extern struct amp_poly_i amp_inject_iface;

struct amp_inject_t *amp_inject_new(struct amp_seq_t seq);
struct amp_inject_t *amp_inject_copy(struct amp_inject_t *inject);
void amp_inject_delete(struct amp_inject_t *inject);

char *amp_inject_make(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env);

void amp_inject_info(struct amp_inject_t *inject, struct amp_info_t info);
bool amp_inject_proc(struct amp_inject_t *inject, struct amp_poly_t *poly, struct amp_polyinfo_t *info);

#endif
