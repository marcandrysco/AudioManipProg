#include "common.h"


/**
 * Initialize a peak array.
 *   @peak: The peake array.
 *   @n: The number of peaks.
 */
void dsp_tone_init(struct dsp_tone_t *peak, unsigned int n)
{
	unsigned int i;

	for(i = 0; i < n; i++)
		peak[i].freq = peak[i].mag = 0.0;
}

/**
 * Add a tone to the peak array.
 *   @peak: The peake array.
 *   @n: The number of peaks.
 *   @freq: The frequency.
 *   @mag: The magnitude.
 */
void dsp_tone_add(struct dsp_tone_t *peak, unsigned int n, double freq, double mag)
{
	unsigned int i;

	for(i = 0; i < n; i++) {
		if(mag < peak[i].mag)
			continue;

		m_swap_d(&peak[i].freq, &freq);
		m_swap_d(&peak[i].mag, &mag);
	}
}

/**
 * Sort the peaks by frequency.
 *   @peak: The peak.
 *   @n: The number of peaks.
 */
void dsp_tone_sort(struct dsp_tone_t *peak, unsigned int n)
{
	qsort(peak, n, sizeof(struct dsp_tone_t), dsp_tone_compare);
}


/**
 * Compare two tones.
 *   @left: The left tone.
 *   @right: The right tone.
 *   &returns: Their order.
 */
int dsp_tone_cmp(struct dsp_tone_t left, struct dsp_tone_t right)
{
	if(left.freq < right.freq)
		return -1;
	else if(left.freq > right.freq)
		return 1;
	else
		return 0;
}

/**
 * Compare two tones.
 *   @left: The left tone.
 *   @right: The right tone.
 *   &returns: Their order.
 */
int dsp_tone_compare(const void *left, const void *right)
{
	return dsp_tone_cmp(*(const struct dsp_tone_t *)left, *(const struct dsp_tone_t *)right);
}
