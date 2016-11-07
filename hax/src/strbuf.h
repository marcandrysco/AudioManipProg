#ifndef STRBUF_H
#define STRBUF_H

/**
 * String buffer structure.
 *   @idx, len: The index and length.
 *   @arr: The character array.
 */

struct strbuf_t {
	size_t idx, len;
	char *arr;
};


/*
 * string buffer declarations
 */

struct strbuf_t strbuf_init(size_t initlen);
void strbuf_destroy(struct strbuf_t *buf);

char strbuf_last(struct strbuf_t *buf);
void strbuf_addch(struct strbuf_t *buf, char ch);
void strbuf_addstr(struct strbuf_t *buf, const char *restrict str);
void strbuf_addmem(struct strbuf_t *buf, const char *restrict data, size_t nbytes);

void strbuf_reset(struct strbuf_t *buf);

char *strbuf_done(struct strbuf_t *buf);
const char *strbuf_finish(struct strbuf_t *buf);

#endif
