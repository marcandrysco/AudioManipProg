#include "common.h"


/**
 * MIDI communication structure.
 *   @event: The event list.
 *   @rd, wr: The read and write indices.
 *   @lock: Write lock.
 *   @inst: The instance list.
 *   @len: The instance list length.
 */
struct amp_comm_t {
	struct amp_event_t event[AMP_COMM_LEN];
	volatile unsigned int rd, wr;

	sys_mutex_t lock;

	struct inst_t *inst;
};

/**
 * Instance structure.
 *   @dev: The device number.
 *   @midi: The MIDI device.
 *   @comm: The communication structure.
 *   @next: The next instance.
 */
struct inst_t {
	uint16_t dev;
	struct amp_midi_t midi;
	struct amp_comm_t *comm;

	struct inst_t *next;
};


/*
 * local declarations
 */
static void callback(uint16_t key, uint16_t val, void *arg);


/**
 * Create a communication structure.
 *   &returns: The communication structure.
 */
struct amp_comm_t *amp_comm_new(void)
{
	struct amp_comm_t *comm;

	comm = malloc(sizeof(struct amp_comm_t));
	comm->rd = comm->wr = 0;
	comm->inst = NULL;
	comm->lock = sys_mutex_init(0);

	return comm;
}

/**
 * Delete a communication structure.
 *   @comm: The communication structure.
 */
void amp_comm_delete(struct amp_comm_t *comm)
{
	struct inst_t *inst;

	while((inst = comm->inst) != NULL) {
		comm->inst = inst->next;

		amp_midi_close(inst->midi);
		free(inst);
	}

	sys_mutex_destroy(&comm->lock);
	free(comm);
}


/**
 * Add a MIDI device to the communication structure.
 *   @comm: The communication structure.
 *   @dev: The device number.
 *   @conf: The configuration.
 *   @iface: The MIDI interface.
 *   @midi: The MIDI device.
 */
void amp_comm_add(struct amp_comm_t *comm, uint16_t dev, const char *conf, const struct amp_midi_i *iface)
{
	struct inst_t *inst;

	inst = malloc(sizeof(struct inst_t));
	inst->dev = dev;
	inst->midi = amp_midi_open(conf, callback, inst, iface);
	inst->comm = comm;
	inst->next = comm->inst;
	comm->inst = inst;
}

/**
 * Read an event from the communications
 *   @comm: The communication structure.
 *   @event: Out. The output event pointer.
 *   &returns: True if event available, false otherwise.
 */
bool amp_comm_read(struct amp_comm_t *comm, struct amp_event_t *event)
{
	if(comm->rd != comm->wr) {
		*event = comm->event[comm->rd];
		comm->rd = (comm->rd + 1) % AMP_COMM_LEN;

		return true;
	}
	else
		return false;

}


/**
 * MIDI callback.
 *   @key: The key.
 *   @val: The value.
 *   @arg: The argument.
 */
static void callback(uint16_t key, uint16_t val, void *arg)
{
	struct inst_t *inst = arg;
	struct amp_comm_t *comm = inst->comm;

	sys_mutex_lock(&comm->lock);

	comm->event[comm->wr] = (struct amp_event_t){ inst->dev, key, val };
	__sync_synchronize();
	comm->wr = (comm->wr + 1) % AMP_COMM_LEN;

	sys_mutex_unlock(&comm->lock);
}
