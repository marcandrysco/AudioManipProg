Pan Instrument
==============

Performs panning on a multi-channel signal. One channel is scaled and delayed
to provide a surround sound effect.

## Summary

MuseLang constructor

    Pan (chan:int, volume:float, delay:float)
    PanLeft (volume:float, delay:float)
    PanRight (volume:float, delay:float)

The target channel is selected by the `chan` parameter. The constructors
`PanLeft` and `PanRight` automatically select the first (left) and second
(right) channels, respectively.
