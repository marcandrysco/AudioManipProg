#ifndef ALGO_H
#define ALGO_H

/*
 * algorithm declarations
 */
void dsp_fft_d(const double *in, double *mag, double *phase, unsigned int len);
void dsp_ifft_d(const double *mag, const double *phase, double *out, unsigned int len);

#endif
