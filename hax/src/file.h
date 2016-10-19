#ifndef FILE_H
#define FILE_H

/**
 * Read function.
 *   @ref: The reference.
 *   @buf: The buffer.
 *   @nbytes: The number of bytes to read.
 *   &returns: The number of bytes read.
 */
typedef size_t (*io_read_f)(void *ref, void *buf, size_t nbytes);

/**
 * Write function.
 *   @ref: The reference.
 *   @buf: The buffer.
 *   @nbytes: The number of bytes to write.
 *   &returns: The number of bytes written.
 */
typedef size_t (*io_write_f)(void *ref, const void *buf, size_t nbytes);

/**
 * Close function.
 *   @ref: The reference.
 */
typedef void (*io_close_f)(void *ref);

/**
 * File interface.
 *   @read: Read data.
 *   @write: Write data.
 *   @close: Close file.
 */
struct io_file_i {
	io_read_f read;
	io_write_f write;
	io_close_f close;
};

/**
 * File structure.
 *   @ref: The reference.
 *   @iface: The file interface.
 */
struct io_file_t {
	void *ref;
	const struct io_file_i *iface;
};

/**
 * Read to a file.
 *   @file: The file.
 *   @buf: The buffer.
 *   @nbytes: The number of bytes.
 *   &returns: The number of bytes read.
 */
static inline size_t io_file_read(struct io_file_t file, void *buf, size_t nbytes)
{
	return file.iface->read(file.ref, buf, nbytes);
}

/**
 * Write to a file.
 *   @file: The file.
 *   @buf: The buffer.
 *   @nbytes: The number of bytes.
 *   &returns: The number of bytes written.
 */
static inline size_t io_file_write(struct io_file_t file, const void *buf, size_t nbytes)
{
	return file.iface->write(file.ref, buf, nbytes);
}

/**
 * Close a file.
 *   @file: The file.
 */
static inline void io_file_close(struct io_file_t file)
{
	file.iface->close(file.ref);
}


/*
 * file declarations
 */
struct io_file_t io_file_len(size_t *len);
struct io_file_t io_file_accum(char **str);
struct io_file_t io_file_wrap(FILE *file);
struct io_file_t io_file_fd(sys_fd_t fd);
struct io_file_t io_file_str(const char *str);
struct io_file_t io_file_buf(void **ptr, size_t *nbytes);

#endif
