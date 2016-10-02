#ifndef MATH_H
#define MATH_H

/**
 * Compute the minimum of two integers.
 *   @left: The left integer.
 *   @right: The right integer.
 *   &returns: The minimum.
 */
static inline int m_min_i(int left, int right)
{
	return (left < right) ? left : right;
}

/**
 * Compute the maximum of two integers.
 *   @left: The left integer.
 *   @right: The right integer.
 *   &returns: The maximum.
 */
static inline int m_max_i(int left, int right)
{
	return (left > right) ? left : right;
}


/**
 * Compute the minimum of two unsigned integers.
 *   @left: The left integer.
 *   @right: The right integer.
 *   &returns: The minimum.
 */
static inline unsigned int m_min_u(unsigned int left, unsigned int right)
{
	return (left < right) ? left : right;
}

/**
 * Compute the maximum of two unsigned integers.
 *   @left: The left integer.
 *   @right: The right integer.
 *   &returns: The maximum.
 */
static inline unsigned int m_max_u(unsigned int left, unsigned int right)
{
	return (left > right) ? left : right;
}

/**
 * Compute the minimum of a list of unsigned integers.
 *   @vec: The vector of integer.
 *   @n: The number of integers.
 *   &returns: The minimum.
 */
static inline unsigned int m_min_u_vec(unsigned int *vec, unsigned int n)
{
	unsigned int min = UINT_MAX;

	while(n-- > 0)
		min = m_min_u(min, vec[n]);

	return min;
}


/**
 * Divide with positive rounding.
 *   @val: Value to be divided.
 *   @div: Divisor value.
 */
static inline int m_div_p(int val, int div)
{
	return (val <= 0) ? (val / div) : ((val + div - 1) / div);
}

/**
 * Divide with negative rounding.
 *   @val: Value to be divided.
 *   @div: Divisor value.
 */
static inline int m_div_n(int val, int div)
{
	return (val >= 0) ? (val / div) : ((val - div + 1) / div);
}


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
