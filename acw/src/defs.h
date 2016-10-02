#ifndef DEFS_H
#define DEFS_H

/**
 * ACW information structure.
 *   @magic: Magic constant.
 *   @nbytes, nblocks, length: Number of bytes, blocks, and total length
 *   @rate, bits: The sample rate and base bitwidth.
 */
struct acw_info_t {
	uint64_t magic;
	uint32_t nbytes, nblocks, length;
	uint16_t rate, bits;
};

/**
 * Buffer structure.
 *   @info: The buffer information.
 *   @raw: The raw data.
 */
struct acw_buf_t {
	struct acw_info_t info;
	void *raw;
};


/**
 * Magic constant values.
 *   @ACW_MAGIC_1_0: Version 1.0.
 */
#define ACW_MAGIC_1_0 0x8821eb30b0f0100


/*
 * format definitions
 */
#define ACW_0BIT  0x0
#define ACW_4BIT  0x1
#define ACW_6BIT  0x2
#define ACW_8BIT  0x3
#define ACW_12BIT 0x4
#define ACW_16BIT 0x5
#define ACW_20BIT 0x6
#define ACW_24BIT 0x7

#define ACW_ADD_SZ   12
#define ACW_ADD_MAX  ((1<<(ACW_ADD_SZ-1))-1)
#define ACW_ADD_MIN  (-(1<<(ACW_ADD_SZ-1)))


/**
 * Sign extend a value.
 *   @val: The value to be extended.
 *   @bits: The number of bits to extend.
 */
static inline int acw_ext(int val, int bits)
{
	return (val << (8*sizeof(int) - bits)) >> (8*sizeof(int) - bits);
}

/**
 * Get the number of bits from a format.
 *   @fmt: The format.
 *   &returns: The bit width, or negative if invalid format.
 */
static inline int acw_getbits(uint16_t fmt)
{
	switch(fmt) {
	case ACW_0BIT: return 0;
	case ACW_4BIT: return 4;
	case ACW_8BIT: return 8;
	case ACW_12BIT: return 12;
	case ACW_16BIT: return 16;
	case ACW_20BIT: return 20;
	case ACW_24BIT: return 24;
	default: return -1;
	}
}

#endif
