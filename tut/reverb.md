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

Our reverberator probably sounds far too bright and tinny due to the fact that
we are using a 100% wet signal. To temper the reverberator, the
[Mix](../core/doc/efx/mix.md) works like the mix knob on most effect pedals. A
value of 0.0 generates a completely dry signal, and a value of 1.0 gives a
completely wet signal. For reverberators, we generally keep the mix low --
usuall around 10% (0.1) wet. The full code including the mixing is below.

    let reverb = Allpass(0.001,0.6)
    let amp.instr = Splice(Mix(0.1,reverb))
