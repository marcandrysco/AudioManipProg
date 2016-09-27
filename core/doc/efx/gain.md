Gain Effect
===========

The gain effects provide a mechanism for controlling the volume of a signal.
All versions of gain effects can accept time-varying inputs, including MIDI
controls (for faders) or oscillators (for VCAs).


## Summary

MuseLang constructor

    Gain scale:Param
    Boost val:Param
    Cut val:Param
    Thru
    Mute

The `Gain` constructor is applied linearly linearly so that `0` corresponds to
mute and `1` corresponds to unity gain. Negative values will invert the
signal.

The `Boost` increases the input signal by `val` decibels. Negative values will
cut the signal.

The `Cut` decreases the input signal by `val` decibels. Negatives values will
boost the signal.

The `Thru` and `Mute` objects are gain effects that pass through or completely
mute the signal. Note that these are objects and not constructors, they cannot
be passed any parameters.

### Detailed Operation

The output is computed exactly as `y[n] = s[n] * x[n]` where `s[n]` is the
scale, `x[n]` is the input, and `y[n]` is the output. For a `Boost` effect,
the scale is `s[n] = pow(10, v[n] / 20)`. For a `Cut` effect, the scale is
`s[n] = pow(0.1, v[n] / 20)`.
