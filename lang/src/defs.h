#ifndef DEFS_H
#define DEFS_H

/**
 * Path structure.
 *   @str: The string.
 *   @refcnt: The reference count.
 */
struct ml_path_t {
	char *str;
	unsigned int refcnt;
};

/**
 * Tag structure.
 *   @path: The path.
 *   @line, col: The line and column information.
 */
struct ml_tag_t {
	struct ml_path_t *path;
	unsigned int line, col;
};

#endif
