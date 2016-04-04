#include "common.h"

/*****************************************************************************
 * The I/O manager enables components to asynchronously read and write data to
 * the real time thread. The I/O manager is composed to two pieces: 1) the
 * real time reader/writer 'amp_read_proc'/'amp_write_proc' and 2) the
 * asynchronous thread 'callback'. The readers and writers use an internal
 * buffer that is read/written without requiring a lock. The internal buffers
 * are split into two halves, and once one half is filled, the asynchronous
 * thread is woken up to process the data at a relatively slow pace; the
 * thread must finish processing data before the next buffer half is filled.
 ****************************************************************************/


/**
 * I/O manager structure.
 *   @terminate flag.
 *   @lock: The lock.
 *   @cond: The condition variable.
 *   @thread: Processing thread.
 *   @idx, sel: The index and buffer selector.
 *   @read, write: The read and write lists.
 */
struct amp_io_t {
	bool term;
	sys_mutex_t lock;
	sys_cond_t cond;
	sys_thread_t thread;

	unsigned int idx, sel;

	struct list_root_t read, write;
};

/**
 * Read structure.
 *   @io: The I/O manager.
 *   @idx: The index.
 *   @buf: The buffer.
 *   @func: Read function.
 *   @arg: Function argument.
 *   @node: The node.
 */
struct amp_read_t {
	struct amp_io_t *io;

	unsigned int idx;
	double buf[AMP_IO_LEN];

	amp_read_f func;
	void *arg;

	struct list_node_t node;
};

/**
 * Write structure.
 *   @io: The I/O manager.
 *   @idx: The index.
 *   @buf: The buffer.
 *   @func: Write function.
 *   @arg: Function argument.
 *   @node: The node.
 */
struct amp_write_t {
	struct amp_io_t *io;

	unsigned int idx;
	double buf[AMP_IO_LEN];

	amp_write_f func;
	void *arg;

	struct list_node_t node;
};


/*
 * local declarations
 */
static void *callback(void *arg);


/**
 * Create a new I/O manager.
 *   &returns: The I/O manager.
 */
struct amp_io_t *amp_io_new(void)
{
	struct amp_io_t *io;

	io = malloc(sizeof(struct amp_io_t));
	io->lock = sys_mutex_init(0);
	io->cond = sys_cond_init(0);
	io->thread = sys_thread_create(0, callback, io);
	io->read = io->write = list_root_init();

	return io;
}

/**
 * Delete an I/O manager.
 *   @io: The I/O manager.
 */
void amp_io_delete(struct amp_io_t *io)
{
	io->term = true;
	sys_mutex_lock(&io->lock);
	sys_cond_signal(&io->cond);
	sys_mutex_unlock(&io->lock);

	sys_thread_join(&io->thread);
	list_root_destroy(&io->read, offsetof(struct amp_read_t, node), free);
	list_root_destroy(&io->write, offsetof(struct amp_write_t, node), free);
	sys_mutex_destroy(&io->lock);
	sys_cond_destroy(&io->cond);
	free(io);
}


/**
 * Process an I/O manager.
 *   @io: The I/O manager.
 *   @len: The length.
 */
void amp_io_proc(struct amp_io_t *io, unsigned int len)
{
	uint8_t sel;
	struct amp_read_t *read;
	struct amp_write_t *write;

	io->idx = (io->idx + len) % AMP_IO_LEN;
	sel = io->idx / (AMP_IO_LEN / 2);

	if(io->sel == sel)
		return;

	io->sel = sel;

	//if(_test || _debug) {
		for(read = amp_read_first(io); read != NULL; read = amp_read_next(read)) {
			if(io->idx != read->idx)
				fatal("Player index mismatch. %u vs %u.", io->idx, read->idx);
		}

		for(write = amp_write_first(io); write != NULL; write = amp_write_next(write)) {
			if(io->idx != write->idx)
				fatal("Recorder index mismatch. %u vs %u.", io->idx, write->idx);
		}
	//}

	sys_mutex_lock(&io->lock);
	sys_cond_signal(&io->cond);
	sys_mutex_unlock(&io->lock);
}

/**
 * Prepare an I/O manager.
 *   @io: The I/O manager.
 */
void amp_io_prep(struct amp_io_t *io)
{
	struct amp_read_t *read;

	io->idx = 0;
	io->sel = 0;

	for(read = amp_read_first(io); read != NULL; read = amp_read_next(read))
		read->func(read->buf, AMP_IO_LEN, read->arg);
}

/**
 * Flush an I/O manager.
 *   @io: The I/O manager.
 */
void amp_io_flush(struct amp_io_t *io)
{
	struct amp_write_t *write;
	unsigned int idx, len;

	idx = (AMP_IO_LEN / 2) * (io->idx / (AMP_IO_LEN / 2));
	len = io->idx - idx;

	if(len == 0)
		return;

	for(write = amp_write_first(io); write != NULL; write = amp_write_next(write))
		write->func(write->buf + idx, len, write->arg);
}

/**
 * Thread callback.
 *   @arg: The argument.
 *   &returns: Always null.
 */
static void *callback(void *arg)
{
	struct amp_read_t *read;
	struct amp_write_t *write;
	struct amp_io_t *io = arg;
	unsigned int sel = 0;

	sys_mutex_lock(&io->lock);

	while(true) {
		sys_cond_wait(&io->cond, &io->lock);

		if(io->term)
			break;

		for(read = amp_read_first(io); read != NULL; read = amp_read_next(read))
			read->func(read->buf + sel * AMP_IO_LEN, AMP_IO_LEN, read->arg);

		for(write = amp_write_first(io); write != NULL; write = amp_write_next(write))
			write->func(write->buf + sel * AMP_IO_LEN, AMP_IO_LEN, write->arg);

		sel = 1 - sel;
	}

	sys_mutex_unlock(&io->lock);

	return NULL;
}


/**
 * Create a new reader.
 *   @io: The I/O manager.
 *   @func: The read function.
 *   @arg: The function argument.
 *   &returns: The reader.
 */
struct amp_read_t *amp_read_new(struct amp_io_t *io, amp_read_f func, void *arg)
{
	struct amp_read_t *read;

	read = malloc(sizeof(struct amp_read_t));
	read->io = io;
	read->idx = 0;
	read->func = func;
	read->arg = arg;
	list_root_append(&io->read, &read->node);

	return read;
}

/**
 * Delete a reader.
 *   @read: The reader.
 */
void amp_read_delete(struct amp_read_t *read)
{
	list_root_remove(&read->io->read, &read->node);
	free(read);
}


/**
 * Process a reader.
 *   @read: The reader.
 *   @buf: The buffer.
 *   @len: The length.
 */
void amp_read_proc(struct amp_read_t *read, double *buf, unsigned int len)
{
	while(len-- > 0) {
		*(buf++) += read->buf[read->idx];
		read->idx = (read->idx + 1) % AMP_IO_LEN;
	}
}

/**
 * Retrieve the first reader from the I/O manager.
 *   @io: The I/O manager.
 *   &returns: The reader or null.
 */
struct amp_read_t *amp_read_first(struct amp_io_t *io)
{
	struct list_node_t *node;

	node = io->read.head;
	return node ? getparent(node, struct amp_read_t, node) : NULL;
}

/**
 * Retrieve the next reader.
 *   @read: The reader.
 *   &returns: The next reader or null.
 */
struct amp_read_t *amp_read_next(struct amp_read_t *read)
{
	struct list_node_t *node;

	node = read->node.next;
	return node ? getparent(node, struct amp_read_t, node) : NULL;
}


/**
 * Create a new writer.
 *   @io: The I/O manager.
 *   @func: The write function.
 *   @arg: The function argument.
 *   &returns: The writer.
 */
struct amp_write_t *amp_write_new(struct amp_io_t *io, amp_write_f func, void *arg)
{
	struct amp_write_t *write;

	write = malloc(sizeof(struct amp_write_t));
	write->io = io;
	write->idx = 0;
	write->func = func;
	write->arg = arg;
	list_root_append(&io->write, &write->node);

	return write;
}

/**
 * Delete a writer.
 *   @write: The writer.
 */
void amp_write_delete(struct amp_write_t *write)
{
	list_root_remove(&write->io->write, &write->node);
	free(write);
}


/**
 * Process a writer.
 *   @write: The writer.
 *   @buf: The buffer.
 *   @len: The length.
 */
void amp_write_proc(struct amp_write_t *write, const double *buf, unsigned int len)
{
	while(len-- > 0) {
		write->buf[write->idx] = *(buf++);
		write->idx = (write->idx + 1) % AMP_IO_LEN;
	}
}

/**
 * Retrieve the first writer from the I/O manager.
 *   @io: The I/O manager.
 *   &returns: The writer or null.
 */
struct amp_write_t *amp_write_first(struct amp_io_t *io)
{
	struct list_node_t *node;

	node = io->write.head;
	return node ? getparent(node, struct amp_write_t, node) : NULL;
}

/**
 * Retrieve the next writer.
 *   @write: The writer.
 *   &returns: The next writer or null.
 */
struct amp_write_t *amp_write_next(struct amp_write_t *write)
{
	struct list_node_t *node;

	node = write->node.next;
	return node ? getparent(node, struct amp_write_t, node) : NULL;
}
