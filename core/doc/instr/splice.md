Splice Instrument
=================

The splice instrument processes a two-channel signal with a single channel
effect. The effect is run on a mix of the two inputs, and the output is copied
to both output channels.

## Summary

MuseLang constructor

    Splice ((left:Param, right:Param), effect:Effect)

The input to the effect is given by a mix of the input channels scaling the
left input by the `left` parameter and the right input by the `right`
parameter -- `in = left * chan[0] + right * chan[1]`.
