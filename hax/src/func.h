#ifndef FUNC_H
#define FUNC_H

/*
 * comparison declarations
 */
int compare_int(const void *left, const void *right);
int compare_uint16(const void *left, const void *right);
int compare_str(const void *left, const void *right);
int compare_char(const void *left, const void *right);
int compare_ptr(const void *left, const void *right);

void delete_noop(void *ptr);

#endif
