#ifndef RAND_H
#define RAND_H

/**
 * Random storage structure.
 *   @x, y, z, w: Storage data.
 */
struct m_rand_t {
	uint32_t x, y, z, w;
};

/*
 * random declarations
 */
extern struct m_rand_t m_rand;

void m_rand_seed(uint32_t seed);
struct m_rand_t m_rand_init(uint32_t seed);
double m_rand_d(struct m_rand_t *rand);

#endif
