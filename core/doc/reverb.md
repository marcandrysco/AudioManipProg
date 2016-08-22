Reverberators
=============

This page covers filters commonly found in reverberators -- these are filters
that store a long tail of previous values for creating distant echoes. More
conventional filters are covered [separately](filt). Each filter comes in two
types: fixed length filters that use a constant delay length, and variable
delay filters whose delay length may vary over time. Because memory is
allocated statically, variable delay filters must up front specify the maximum
delay length. Variable delay filters are denoted with a 'V' after the filter
name.


## Delay Filter

    Delay  (len, gain)
    Delay1 len               (* Delay1 len = Delay (len, 1) *)
    DelayV (max, len, gain)

The delay filter (or [feedforward comb filter]) is the simpliest form of
reverberator filter. The input signal is delayed by the number of samples
specified by the `length` and multiplied by the specified `gain`. The `Delay1`
constructor creates a delay with unity gain (`gain=1`).


## Comb Filter

    Comb  (len, gain)
    CombV (max, len, gain)

The comb filter (more accurately a [feedback comb filter]) generates an
infinite series of decaying copies of the original input. Because the filter
uses a feedback path from the output, it is important to never let the gain
reach of exceed one to prevent an unstable filter.


## Allpass Filter

    Allpass  (len, gain)
    AllpassV (max, len, gain)

The allpass filter is a long tail filter that composes a feedforward and
feedback comb filter (more details can be found on [DSP Related]). The
resulting reverberation is "colorless" in that the frequency response is
perfectly flat. However, high gain levels will still produce a distinct
ringing -- something that can be avoided by using a low-pass comb filter.


## Low-Pass Comb Filter

    Lpcf  (len, gain, freq)
    LpcfV (max, len, gain, freq)

The low-pass comb filter combines is a feedback comb filter with a one-pole
low-pass filter on the feedback path.


## Band-Pass Comb Filter

    Bpcf  (len, gain, freqlo, freqhi)
    BpcfV (max, len, gain, freqlo, freqhi)

The low-pass comb filter combines is a feedback comb filter with a two-pole
band-pass filter on the feedback path.


## Resonant Comb Filter

    Rescf  (len, gain, freq, qual)
    RescfF (freq, gain, qual)
    RecsfV (max, len, gain, freq, qual)

Resonant comb filters are implemented by adding a resonant filter (`Res`) in
the feedback loop of a `Comb` filter. The resulting filter creates a decaying
series of echoes that quickly converge to resonate at the given `frequency`.
The `RescfF` version of the filter uses a delay length of `1/freq` -- this is
its most common usage.


[feedforward comb filter]: https://en.wikipedia.org/wiki/Comb_filter#Feedforward_form
[feedback comb filter]: https://en.wikipedia.org/wiki/Comb_filter#Feedforward_form
[DSP Related]: https://www.dsprelated.com/freebooks/pasp/Allpass_Two_Combs.html
