Tone Tracking
=============


    struct dsp_tone_t {
      double freq, mag;
    };


    void dsp_tone_init(struct dsp_tone_t *peak, unsigned int n);
    void dsp_tone_add(struct dsp_tone_t *peak, unsigned int n, double freq, double mag);

    void dsp_tone_sort(struct dsp_tone_t *peak, unsigned int n);

    int dsp_tone_cmp(struct dsp_tone_t left, struct dsp_tone_t right);
    int dsp_tone_compare(const void *left, const void *right);
