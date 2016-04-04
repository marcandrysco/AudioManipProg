Oscillator Module
=================

The oscilator modules generates a periodic signal. The output signal has a
period matching the `freq` input, starting and ending at `0` for every period.
The output covers the range of outputs from `-1` to `1`.

## Summary

MuseLang constructor

    Osc (type:String, freq:Param, warp:Param)

The oscillator generates a period given by `freq`. The `type` is a string of
either `sine` for a sine wave, `saw` for a saw or tirnagle wave, or `pulse`
for a pulse or square wave. The `warp` parameter varies from `-1` to `1` to
deform the output wave by stretching or compressing the each half of the wave.
Warping a saw wave changes the upward and downward slope of the wave so that a
unwarped saw is a triangle waves and a fully warped saw is a saw with
discontinuities. Warping a pulse wave varies the width of the pulse, and a
warp of zero is a square wave.

## Specialized Constructors

Special constructors create oscillators with a specific type. `Sine` creates a
sine oscillator, `Saw` creates a sawtooth or triangle oscillator, and `Pulse`
creates a pulse or square oscillator.

    Sine (freq:Param, warp:Param)
    Saw (freq:Param, warp:Param)
    Pulse (freq:Param, warp:Param)



    Osc1 (type:String, freq:Param, warp:Param)
    Sine1 (freq:Param, warp:Param)
    Saw1 (freq:Param, warp:Param)
    Pulse1 (freq:Param, warp:Param)
