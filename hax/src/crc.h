#ifndef CRC_H
#define CRC_H

/*
 * base64 declarations
 */
void b64_enc(char *out, const void *in, size_t nbytes);
size_t b64_enclen(size_t nbytes);

/*
 * crc32 declarations
 */
extern uint32_t crc32_table[];
uint32_t crc32_calc(uint32_t crc, const void *buf, size_t nbytes);

#endif
