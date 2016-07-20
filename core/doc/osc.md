Oscillators
===========

The following sections describe a number of oscillators available in AmpCore.
Before using an oscillator, it is useful to understand a few concepts
regarding the difference between time and frequency signals.

Oscillators do not operate on raw frequencies; instead, they expect a time
signal. Time signal are a stream of values that specify the current time or
phase of an oscillator, usually taking on values between zero and one.
Periodic time signals of a given frequency may be generated using the
`Ramp` constructor. Putting it all together, a sine wave at 440Hz would be
`Sine (Ramp 440)`.

## Ramp

    Ramp freq

The `Ramp` module is takes a frequency as input and generates a periodic time
signal. The output begins at zero and creates a ramp to one that cycles `freq`
times per second.

## Warping

    Warp (tm, dist)

## Sine Wave

    Sine  tm
    SineF freq          (* = Sine (Ramp freq) *)
    SineW (freq, dist)  (* = Sine (Warp (Ramp freq) dist) *)

