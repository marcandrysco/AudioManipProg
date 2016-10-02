#ifndef ENCODE_H
#define ENCODE_H

/*
 * encoder declarations
 */
struct acw_encode_t *acw_encode_new(void);
void acw_encode_proc(struct acw_encode_t *enc, int *arr, unsigned int len);
struct acw_buf_t acw_encode_done(struct acw_encode_t *enc);

#endif
