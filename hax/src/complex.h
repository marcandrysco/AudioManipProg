#ifndef COMPLEX_H
#define COMPLEX_H

/**
 * Double complex structure.
 *   @re, im: The re and imaginary components.
 */
struct z_double_t {
	double re, im;
};


/*
 * complex variables
 */
extern struct z_double_t z_zero_d;
extern struct z_double_t z_one_d;


/**
 * Complex double contructor.
 *   @re: The real component.
 *   @im: The imaginary component.
 *   &returns: The complex number.
 */
static inline struct z_double_t z_init_d(double re, double im)
{
	return (struct z_double_t){ re, im };
}

/**
 * Create a purely real complex number.
 *   @re: The real component.
 *   &returns: The complex number.
 */
static inline struct z_double_t z_re_d(double re)
{
	return z_init_d(re, 0.0);
}

/**
 * Create a purely imaginary complex number.
 *   @im: The imaginary component.
 *   &returns: The complex number.
 */
static inline struct z_double_t z_im_d(double im)
{
	return z_init_d(0.0, im);
}

/**
 * Check if a complex double is zero.
 *   @flt: The complex double.
 *   &returns: True if zero, false otherwise.
 */
static inline bool z_iszero_d(struct z_double_t flt)
{
	return ((flt.re == 0.0) || (flt.re == -0.0)) && ((flt.im == 0.0) || (flt.im == -0.0));
}

/**
 * Check if a complex double is one.
 *   @flt: The complex double.
 *   &returns: True if one, false otherwise.
 */
static inline bool z_isone_d(struct z_double_t flt)
{
	return (flt.re == 1.0) && ((flt.im == 0.0) || (flt.im == -0.0));
}

/**
 * Check if two complex doubles are equal.
 *   @left: The left value.
 *   @right: The right value.
 *   &returns: True if equal, false otherwise.
 */
static inline bool z_isequal_d(struct z_double_t left, struct z_double_t right)
{
	return (left.re == right.re) && (left.im == right.im);
}


/**
 * Add two complex doubles together.
 *   @a: The first value.
 *   @b: The second value.
 *   &returns: The sum.
 */
static inline struct z_double_t z_add_d(struct z_double_t a, struct z_double_t b)
{
	return z_init_d(a.re + b.re, a.im + b.im);
}

/**
 * Subtract two complex doubles together.
 *   @a: The first value.
 *   @b: The second value.
 *   &returns: The difference.
 */
static inline struct z_double_t z_sub_d(struct z_double_t a, struct z_double_t b)
{
	return z_init_d(a.re - b.re, a.im - b.im);
}

/**
 * Multiply two complex doubles together.
 *   @a: The first value.
 *   @b: The second value.
 *   &returns: The product.
 */
static inline struct z_double_t z_mul_d(struct z_double_t a, struct z_double_t b)
{
	return z_init_d(a.re * b.re - a.im * b.im, a.im * b.re + a.re * b.im);
}

/**
 * Divide two complex doubles together.
 *   @a: The first value.
 *   @b: The second value.
 *   &returns: The quotient.
 */
static inline struct z_double_t z_div_d(struct z_double_t a, struct z_double_t b)
{
	double t = b.re * b.re + b.im * b.im;

	return z_init_d((a.re * b.re + a.im * b.im) / t, (a.im * b.re - a.re * b.im) / t);
}

/**
 * Exponentiate the complex double.
 *   @a: The complex value.
 *   &returns: The exponentiated value.
 */
static inline struct z_double_t z_exp_d(struct z_double_t a)
{
	double c = exp(a.re);

	return z_init_d(c * cos(a.im), c * sin(a.im));
}

/**
 * Exponentiate to an imaginary number.
 *   @im: The imaginary coefficient.
 *   &returns: The exponentiated value.
 */
static inline struct z_double_t z_expi_d(double im)
{
	return z_init_d(cos(im), sin(im));
}


/**
 * Compute the magnitude of the complex double.
 *   @a: The complex double.
 *   &returns: The magnitude.
 */
static inline double z_mag_d(struct z_double_t a)
{
	return sqrt((a.re * a.re) + (a.im * a.im));
}

/**
 * Compute the magnitude of the complex double.
 *   @a: The complex double.
 *   &returns: The argument.
 */
static inline double z_arg_d(struct z_double_t a)
{
	return atan2(a.im, a.re);
}

#endif
