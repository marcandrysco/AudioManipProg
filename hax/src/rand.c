#include "common.h"


/*
 * global variables
 */
struct m_rand_t m_rand = { 123456789, 362436069, 521288629, 1049141 };


void m_rand_seed(uint32_t seed)
{
	m_rand = m_rand_init(seed);
}

/**
 * Initialize a random number generator.
 *   @seed: The seed.
 *   &returns; The random generator.
 */
struct m_rand_t m_rand_init(uint32_t seed)
{
	return (struct m_rand_t){ 123456789, 362436069, 521288629, seed * 1049141 };
}

/**
 * Retrieve the next number from the generator.
 *   @rand: The number generator.
 *   &returns: The next number.
 */
uint32_t m_rand_next(struct m_rand_t *rand)
{
	  uint32_t t;

	  if(rand == NULL)
		  rand = &m_rand;

	  t = rand->x ^ (rand->x << 11);
	  rand->x = rand->y;
	  rand->y = rand->z;
	  rand->z = rand->w;
	  rand->w = rand->w ^ (rand->w >> 19) ^ (t ^ (t >> 8));

	  return rand->w;
}

/**
 * Retrieve a random number between the values zero and one.
 *   @rand: The number generator.
 *   &returns: The next number.
 */
double m_rand_d(struct m_rand_t *rand)
{
	return m_rand_next(rand) / (double)UINT32_MAX;
}
