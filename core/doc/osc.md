Oscillators
===========

The following sections describe a number of oscillators available in AmpCore.
Before using an oscillator, it is useful to understand a few concepts
regarding the difference between time and frequency signals.

Oscillators do not operate on raw frequencies; instead, they expect a time
signal. Time signals are a stream of values that specify the current time or
phase of an oscillator, usually taking on values between zero and one.
Periodic time signals of a given frequency may be generated using the
`Ramp` constructor and passed to an oscillator such as `Sine`. Putting it all
together, a sine wave at 440Hz would be `Sine (Ramp 440)`.

## Ramp

    Ramp freq

The `Ramp` module is takes a frequency as input and generates a periodic time
signal. The output begins at zero and ramps to one, cycling `freq` times per
second. While the `Ramp` signal may be used directly as an oscillator, it is
typically better to use one of the oscillators below -- the `Ramp` signal only
takes on positive values producing a significant DC offset.

![Ramp](fig/osc_ramp.svg)\ 

## Warping

    Warp (time, dist)

The `Warp` module is used to distort a time signal by compressing or expanding
the first and second half of the signal.

## Sine Wave

    Sine  tm
    SineF freq               (* = Sine (Ramp freq) *)
    SineW (freq, dist)       (* = Sine (Warp (Ramp freq), dist) *)
    SineR (freq, low, high)  (* = UH? *)
    SineP (freq, high)       (* = UH? *)
    Sine1 (freq)             (* = UH? *)

## Noise

    Noise  ()
    Noise1 ()
