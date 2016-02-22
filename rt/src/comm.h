#ifndef COMM_H
#define COMM_H

/*
 * communication declarations
 */

struct amp_comm_t *amp_comm_new(void);
void amp_comm_delete(struct amp_comm_t *comm);

bool amp_comm_read(struct amp_comm_t *comm, struct amp_event_t *event);
void amp_comm_add(struct amp_comm_t *comm, uint16_t dev, const char *conf, const struct amp_midi_i *iface);

#endif
