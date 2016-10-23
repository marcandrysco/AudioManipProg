#include "../common.h"

/*
 * global variables
 */
sys_fd_t sys_badfd = { NULL, INVALID_SOCKET };

/*
 * local declarations
 */
static size_t fd_write(void *ref, const void *buf, size_t nbytes);
static size_t fd_read(void *ref, void *buf, size_t nbytes);
static void fd_close(void *ref);

static size_t sock_write(void *ref, const void *buf, size_t nbytes);
static size_t sock_read(void *ref, void *buf, size_t nbytes);
static void sock_close(void *ref);


/**
 * Check if file descriptor is valid.
 *   @fd: The file descriptor.
 *   &returns: True if valid.
 */
bool sys_isfd(sys_fd_t fd)
{
	return fd.handle != NULL;
}


/**
 * Create a descriptor file.
 *   @fd: The file descriptor.
 *   &returns: The file.
 */
struct io_file_t io_file_fd(sys_fd_t fd)
{
	static const struct io_file_i fdiface = { fd_read, fd_write, fd_close };
	static const struct io_file_i sockiface = { sock_read, sock_write, sock_close };

	if(fd.sock != INVALID_SOCKET)
		return (struct io_file_t){ (void *)(intptr_t)fd.sock, &sockiface };
	else
		return (struct io_file_t){ fd.handle, &fdiface };
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
 * Write to file descriptor.
 *   @ref: The file descriptor reference.
 *   @buf: The buffer.
 *   @nbytes: The number of bytes to write.
 *   &returns: The number of bytes written.
 */
static size_t sock_write(void *ref, const void *buf, size_t nbytes)
{
	return send((int)(intptr_t)ref, buf, nbytes, 0);
}

/**
 * Read from file descriptor.
 *   @ref: The file descriptor reference.
 *   @buf: The buffer.
 *   @nbytes: The number of bytes to read.
 *   &returns: The number of bytes read.
 */
static size_t sock_read(void *ref, void *buf, size_t nbytes)
{
	return recv((int)(intptr_t)ref, buf, nbytes, 0);
}

/**
 * Close file descriptor.
 *   @ref: The file descriptor reference.
 */
static void sock_close(void *ref)
{
}
