Mix effect
==========

The Mix effect works like the mix knob found on various pedals by
controlling the amount of "dry" input and "wet" effect.

## Summary

There are two versions of the Mix effect: one with an explicit ratio and
another with a constant 50/50 mix.

    Mix (ratio:Param, effect:Effect)
    Mix' (effect:Effect)

The input signal is called the "dry" signal, and the output signal processed
using the `effect` is called the "wet" signal. The `ratio` parameter specifies
how much wet versus dry should be added to the output. A value of `1.0`
indicates that only the wet signal should be passed through, and a value of
`0.0` passes only the dry signal. The `Mix'` constructor uses an implicit mix
of `0.5` so that equal amounts of dry and wet signal are output.
