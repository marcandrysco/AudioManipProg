#include "common.h"


struct z_double_t dsp_lpf_tf(double cutoff, struct z_double_t in)
{
	struct z_double_t g = z_re_d(cutoff);

	return z_div_d(z_add_d(g, z_mul_d(g, in)), z_add_d(z_sub_d(g, z_one_d), z_mul_d(z_add_d(g, z_one_d), in)));
}

struct z_double_t dsp_lpf_tf_real(double freq, double cutoff, double rate)
{
	return dsp_lpf_tf(tan(cutoff * M_PI / rate), z_expi_d(2.0 * M_PI * freq / rate));
}

double dsp_lpf_tf_mag(double freq, double cutoff, double rate)
{
	return z_mag_d(dsp_lpf_tf_real(freq, cutoff, rate));
}
