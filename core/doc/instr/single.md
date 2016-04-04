Single Instrument
=================

The `single` instrument processes an effect on only one of the input channels,
and the other channels are unchanged.

## Summary

MuseLang constructor

    Single (chan:Int, effect:Effect)
    Left (effect:Effect)
    Right (effect:Effect)

The value of `chan` selects the input channel and is processed with the
`effect`. The constructors `Left` and `Right` process channel 0 and channel 1,
respectively.
