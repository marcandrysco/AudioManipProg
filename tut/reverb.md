Reverberator Tutorial
=====================

This tutorial walks through building a reverberator using by composing effect
components. For a complete and documented reverberator, see the [Reverberator
Example](../ex/reverb.ml).

## Allpass Reverberator

Before creating any reverberator at all, we will first copy a minimalistic
program that simply passing any input (e.g. microphone) to the output (e.g.
speakers).

    let reverb = Thru
    let amp.instr = Splice(reverb)

Our reverberator is bound to the variable appropriately named `reverb`. The
above program constructs a `Thru` or passthrough effect, performing no
computation on the signal at all.

To begin, our reverberator will consist of a single all-pass reverberator.
The [Allpass](../core/doc/efx/reverb.md#allpass) constructor takes two
parameters: the echo delay in seconds and the echo gain. Generally, the delay
should be set below 20ms so that our ears do not notice a distinct echo, and
the gain is usually around 0.5. That gives us the following program with 1ms
delay and 0.6 gain.

    let reverb = Allpass(0.001,0.6)
    let amp.instr = Splice(reverb)
