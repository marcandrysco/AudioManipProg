#include "common.h"


/**
 * Initialize the string buffer.
 *   @buf: The memory where the string buffer is stored.
 *   @initlen: The initial buffer length.
 *   &returns: The string buffer.
 */
struct strbuf_t strbuf_init(size_t initlen)
{
	struct strbuf_t buf;

	buf.idx = 0;
	buf.len = initlen;
	buf.arr = malloc(initlen);

	return buf;
}

/**
 * Destroy a string buffer.
 *   @buf: The string buffer.
 */
void strbuf_destroy(struct strbuf_t *buf)
{
	free(buf->arr);
}


/**
 * Retrieve the last character from the string buffer.
 *   @buf: The string buffer.
 *   &returns: The last character or null.
 */
char strbuf_last(struct strbuf_t *buf)
{
	return (buf->idx > 0) ? buf->arr[buf->idx - 1] : '\0';
}

/**
 * Add a character in the string buffer.
 *   @buf: The string buffer.
 *   @ch: The character.
 */
void strbuf_addch(struct strbuf_t *buf, char ch)
{
	if(buf->idx >= buf->len)
		buf->arr = realloc(buf->arr, buf->len *= 2);

	buf->arr[buf->idx++] = ch;
}

/**
 * Add a string onto the string buffer.
 *   @buf: The string buffer.
 *   @str: The string.
 */
void strbuf_addstr(struct strbuf_t *buf, const char *restrict str)
{
	strbuf_addmem(buf, str, strlen(str));
}

/**
 * Write data onto the string buffer.
 *   @buf: The string buffer.
 *   @data: The data.
 *   @nbytes: The number of bytes.
 */
void strbuf_addmem(struct strbuf_t *buf, const char *restrict data, size_t nbytes)
{
	unsigned int idx, len;

	idx = buf->idx + nbytes;
	if(idx >= buf->len) {
		len = buf->len;

		do
			len *= 2;
		while(idx >= len);

		buf->arr = realloc(buf->arr, len);
		buf->len = len;
	}

	memcpy(buf->arr + buf->idx, data, nbytes);
	buf->idx = idx;
}


/**
 * Reset the string buffer.
 *   @buf: The buffer.
 */
void strbuf_reset(struct strbuf_t *buf)
{
	buf->idx = 0;
	buf->arr[0] = '\0';
}


/**
 * Write a null byte to the end of the array, returning a pointer to the
 * allocated string. This is only compatible with 'strbuf_init'.
 *   @buf: The string buffer.
 *   &returns: The buffered string.
 */
char *strbuf_done(struct strbuf_t *buf)
{
	buf->arr = realloc(buf->arr, buf->idx + 1);
	buf->arr[buf->idx] = '\0';

	return buf->arr;
}

/**
 * Write a null byte to the end of the array, return the buffered string, and
 * reset the internal index. The returned string will remain valid until the
 * next call to 'strbuf_addch' or if the string buffer is destroyed.
 *   @buf: The string buffer.
 *   &returns: The buffered string.
 */
const char *strbuf_finish(struct strbuf_t *buf)
{
	strbuf_addch(buf, '\0');
	buf->idx = 0;

	return buf->arr;
}
