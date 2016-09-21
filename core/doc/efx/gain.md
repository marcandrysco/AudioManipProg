Gain Effect
===========

The gain effects provide a mechanism for controlling the volume of a signal.


## Summary

MuseLang constructor

    Gain scale:Param
    Boost val:Param
    Cut val:Param

The `Gain` constructor is applied linearly linearly so that `0` corresponds to
mute and `1` corresponds to unity gain. Negative values will invert the
signal.

The `Boost` increases the input signal by `val` decibels. Negative values will
cut the signal.

he `Cut` decreases the input signal by `val` decibels. Negatives values will
boost the signal.

### Detailed Operation

The output is computed exactly as `y[n] = s[n] * x[n]` where `s[n]` is the
scale, `x[n]` is the input, and `y[n]` is the output. For a `Boost` effect,
the scale is `s[n] = pow(10, v[n] / 20)`. For a `Cut` effect, the scale is
`s[n] = pow(0.1, v[n] / 20)`.
