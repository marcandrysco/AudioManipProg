#include "common.h"


/**
 * Accumulator structure.
 *   @str: The string pointer.
 *   @buf: The buffer.
 */
struct accum_t {
	char **str;
	struct strbuf_t buf;
};


/**
 * Write to a length.
 *   @ref: The length reference.
 *   @buf: The buffer.
 *   @nbytes: The number of bytes to write.
 *   &returns: The number of bytes write.
 */
static size_t len_write(void *ref, const void *data, size_t nbytes)
{
	*(size_t *)ref += nbytes;

	return nbytes;
}

/**
 * Read from a length.
 *   @ref: The length reference.
 *   @buf: The buffer.
 *   @nbytes: The number of bytes to read.
 *   &returns: Always zero, cannot read from length.
 */
static size_t len_read(void *ref, void *data, size_t nbytes)
{
	return 0;
}

/**
 * Close a length.
 *   @ref: The length reference.
 */
static void len_close(void *ref)
{
}

/**
 * Create a length file.
 *   @str: Ref. The destination string pointer.
 *   &returns: The file.
 */
struct io_file_t io_file_len(size_t *len)
{
	static const struct io_file_i iface = { len_read, len_write, len_close };

	return (struct io_file_t){ len, &iface };
}


/**
 * Write to an accumulator.
 *   @ref: The accumulator reference.
 *   @buf: The buffer.
 *   @nbytes: The number of bytes to write.
 *   &returns: The number of bytes write.
 */
static size_t accum_write(void *ref, const void *data, size_t nbytes)
{
	struct accum_t *accum = ref;

	strbuf_addmem(&accum->buf, data, nbytes);

	return nbytes;
}

/**
 * Read from an accumulator.
 *   @ref: The accumulator reference.
 *   @buf: The buffer.
 *   @nbytes: The number of bytes to read.
 *   &returns: Always zero, cannot read from accumulator.
 */
static size_t accum_read(void *ref, void *data, size_t nbytes)
{
	return 0;
}

/**
 * Close an accumulator.
 *   @ref: The accumulator reference.
 */
static void accum_close(void *ref)
{
	struct accum_t *accum = ref;

	*accum->str = strbuf_done(&accum->buf);
	free(accum);
}

/**
 * Create a string accumulator file.
 *   @str: Ref. The destination string pointer.
 *   &returns: The file.
 */
struct io_file_t io_file_accum(char **str)
{
	struct accum_t *accum;
	static const struct io_file_i iface = { accum_read, accum_write, accum_close };

	accum = malloc(sizeof(struct accum_t));
	accum->str = str;
	accum->buf = strbuf_init(256);

	return (struct io_file_t){ accum, &iface };
}


/**
 * Write to file.
 *   @ref: The file reference.
 *   @buf: The buffer.
 *   @nbytes: The number of bytes to write.
 *   &returns: The number of bytes written.
 */
static size_t file_write(void *ref, const void *buf, size_t nbytes)
{
	return fwrite(buf, 1, nbytes, ref);
}

/**
 * Read from file.
 *   @ref: The file reference.
 *   @buf: The buffer.
 *   @nbytes: The number of bytes to read.
 *   &returns: The number of bytes read.
 */
static size_t file_read(void *ref, void *buf, size_t nbytes)
{
	return fread(buf, 1, nbytes, ref);
}

/**
 * Close file.
 *   @ref: The file reference.
 */
static void file_close(void *ref)
{
}
/**
 * Create a descriptor file.
 *   @fd: The file descriptor.
 *   &returns: The file.
 */
struct io_file_t io_file_wrap(FILE *file)
{
	static const struct io_file_i iface = { file_read, file_write, file_close };

	return (struct io_file_t){ file, &iface };
}


/**
 * Write to file descriptor.
 *   @ref: The file descriptor reference.
 *   @buf: The buffer.
 *   @nbytes: The number of bytes to write.
 *   &returns: The number of bytes written.
 */
static size_t fd_write(void *ref, const void *buf, size_t nbytes)
{
	return write((int)(intptr_t)ref, buf, nbytes);
}

/**
 * Read from file descriptor.
 *   @ref: The file descriptor reference.
 *   @buf: The buffer.
 *   @nbytes: The number of bytes to read.
 *   &returns: The number of bytes read.
 */
static size_t fd_read(void *ref, void *buf, size_t nbytes)
{
	return read((int)(intptr_t)ref, buf, nbytes);
}

/**
 * Close file descriptor.
 *   @ref: The file descriptor reference.
 */
static void fd_close(void *ref)
{
}
/**
 * Create a descriptor file.
 *   @fd: The file descriptor.
 *   &returns: The file.
 */
struct io_file_t io_file_fd(int fd)
{
	static const struct io_file_i iface = { fd_read, fd_write, fd_close };

	return (struct io_file_t){ (void *)(intptr_t)fd, &iface };
}


