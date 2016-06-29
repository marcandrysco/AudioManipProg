#ifndef CRC_H
#define CRC_H

/*
 * crc32 declarations
 */
extern uint32_t crc32_table[];
uint32_t crc32_calc(uint32_t crc, const void *buf, size_t nbytes);

#endif
