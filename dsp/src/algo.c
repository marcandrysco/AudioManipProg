#include "common.h"


/*
 * local declarations
 */
static void fft_d(struct z_double_t *in, struct z_double_t *out, int n, int step);

/**
 * Perform a fast fourier transform on a buffer.
 *   @in: The input buffer.
 *   @mag: Optional. The magnitude.
 *   @phase: Optional. The phase.
 *   @len: The length.
 */
void dsp_fft_d(const double *in, double *mag, double *phase, unsigned int len)
{
	unsigned int i;
	struct z_double_t *a, *b;

	a = malloc(len * sizeof(struct z_double_t));
	b = malloc(len * sizeof(struct z_double_t));

	for(i = 0; i < len; i++)
		a[i] = b[i] = z_re_d(in[i]);

	fft_d(a, b, len, 1);

	for(i = 0; i < len; i++) {
		if(mag != NULL)
			mag[i] = z_mag_d(a[i]);

		if(phase != NULL)
			phase[i] = z_arg_d(a[i]);
	}

	free(a);
	free(b);
}

/**
 * Perform an inverse fast fourier transform on a buffer.
 *   @mag: The magnitude.
 *   @phase: The phase.
 *   @out: The input buffer.
 *   @len: The length.
 */
void dsp_ifft_d(const double *mag, const double *phase, double *out, unsigned int len)
{
	unsigned int i;
	struct z_double_t *a, *b;

	a = malloc(len * sizeof(struct z_double_t));
	b = malloc(len * sizeof(struct z_double_t));

	for(i = 0; i < len; i++)
		a[i] = b[i] = z_mul_d(z_re_d(mag[i]), z_expi_d(phase[i]));

	fft_d(a, b, len, 1);

	for(i = 0; i < len; i++)
		out[i] = a[i].re / len;

	free(a);
	free(b);
}

/**
 * Internal FFT computation
 *   @in: The input.
 *   @out: The output.
 *   @n: The number of elements in the FFT.
 *   @step: The step size.
 */
static void fft_d(struct z_double_t *in, struct z_double_t *out, int n, int step)
{
	if(step < n) {
		fft_d(out, in, n, step * 2);
		fft_d(out + step, in + step, n, step * 2);

		for(int i = 0; i < n; i += 2 * step) {
			struct z_double_t t = z_mul_d(z_expi_d(M_PI * (double)i / n), out[i + step]);
			in[i / 2]     = z_add_d(out[i], t);
			in[(i + n)/2] = z_sub_d(out[i], t);
		}
	}
}
