Algorithms
==========


## Fast Fourier Transform

    void dsp_fft_d(const double *in, double *mag, double *phase, unsigned int len);
    void dsp_ifft_d(const double *mag, const double *phase, double *out, unsigned int len);
    double dsp_fft_freq(unsigned int idx, unsigned int rate, unsigned int size);


## Blackman Window

    void dsp_blackman_d(double *buf, unsigned int len);
