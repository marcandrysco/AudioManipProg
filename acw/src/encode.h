#ifndef ENCODE_H
#define ENCODE_H

/*
 * encoder declarations
 */
struct acw_encode_t *acw_encode_new(unsigned int n);
void acw_encode_proc(struct acw_encode_t *enc, int val);
void acw_encode_done(struct acw_encode_t *enc);

#endif
