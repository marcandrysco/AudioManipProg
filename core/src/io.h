#ifndef IO_H
#define IO_H

/*
 * i/o definitions
 */
#define AMP_IO_LEN (16*1024)

/**
 * Read callback.
 *   @buf: The buffer.
 *   @len: The buffer length.
 *   @arg: The argument.
 */
typedef void (*amp_read_f)(double *buf, unsigned int len, void *arg);

/**
 * Write callback.
 *   @buf: The buffer.
 *   @len: The buffer length.
 *   @arg: The argument.
 */
typedef void (*amp_write_f)(const double *buf, unsigned int len, void *arg);

/*
 * io declarations
 */
struct amp_io_t *amp_io_new(void);
void amp_io_delete(struct amp_io_t *io);

void amp_io_prep(struct amp_io_t *io);
void amp_io_flush(struct amp_io_t *io);

/*
 * reader declarations
 */
struct amp_read_t *amp_read_new(struct amp_io_t *io, amp_read_f func, void *arg);
void amp_read_delete(struct amp_read_t *read);

void amp_read_proc(struct amp_read_t *read, double *buf, unsigned int len);
struct amp_read_t *amp_read_first(struct amp_io_t *io);
struct amp_read_t *amp_read_next(struct amp_read_t *read);;

/*
 * writer declarations
 */
struct amp_write_t *amp_write_new(struct amp_io_t *io, amp_write_f func, void *arg);
void amp_write_delete(struct amp_write_t *write);

void amp_write_proc(struct amp_write_t *write, const double *buf, unsigned int len);
struct amp_write_t *amp_write_first(struct amp_io_t *io);
struct amp_write_t *amp_write_next(struct amp_write_t *write);

#endif
