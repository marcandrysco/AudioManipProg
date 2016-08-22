#ifndef MATH_H
#define MATH_H

/**
 * Swap two doubles.
 *   @left: The left double.
 *   @right: The right double.
 */
static inline void m_swap_d(double *left, double *right)
{
	double tmp = *left;

	*left = *right;
	*right = tmp;
}

#endif
