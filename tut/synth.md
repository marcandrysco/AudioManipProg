Synthesizer Tutorial
====================

This tutorial will walk you through building a basic synthesizer using AMP.
For a complete and documented synthesizer, see the [Synthesizer
Example](../ex/synth.ml).

## Minimal Synthesizer

To start, let's see an entire program for a very basic synthesizer. The
following program creates a synthesizer that outputs a sine wave that is
activated on the MIDI device number 1. To set up a MIDI device, see the
section [MIDI Setup](#midi-setup).

    let amp.instr =
      let osc = SineF(Trig()) in
      let asr = ASR1(0.01,0.1) in
        Splice(SynthGen'(1,8,Mul(asr,osc)))

Using the above program and a configured MIDI input, you should be able to
produce (boring) sine waves using the MIDI controller. If not, double check
that the device is configured correctly. Before moving on, we'll try to tackle
what exactly is happening in the above program.

#### Splice

Working from the bottom up, we start with the `Splice` instrument. From other
tutorials, you may be familiar how `Splice` enables us to insert a
single-channel processor onto a multi-channel instrument. In this case, our
synthesizer (`SynthGen'`) generates a single channel of output and the
`Splice` copies the data to all output channels.

#### SynthGen'

Moving onto the `SynthGen'` constructor. First, why not just `Synth`? Splice
expects a single-channel processor (aka, an `Effect`), but synthesizers have
no inputs (they are `Module`s). The `Gen` effect bridges the gap: it allows us
to insert a `Module` as an `Effect`. Second, what's with the apostrophe after
`Gen`? Normally, the `Gen` effect adds the module output to the stream, but
the `Gen'` effect first zeroes the input before writing. without zeroing, the
input (e.g. the microphone) would pass through the generator. Combining all
these attributes results in the synthesizer constructor `SynthGen'` -- we want
a synthesizer inserted as an effect that discards the input stream.

#### SineF

As you would expect, the `Sine` constructor creates a sine wave generating
oscillator. Unexpectedly, the input to `Sine` is not the frequency; instead,
the takes time as an input and computes `sin(2*π*x)` where `x` is the input.
To generate a signal with the expected period, we must use `Sine(Ramp(f))` for
a frequency `f`. To make this easier, constructor `SineF(f)` combines these
two constructors so that we provide a frequency as input. More sophisticated
uses of the raw `Sine` constructor is found later.

#### Trig

Lastly, this brings us to the final constructor `Trig`. Simply put, this
returns the frequency of the incoming note. Only a single value is returned by
`Trig`; for polyphonic (multiple note) sounds, the synthesizer sum a series of
modules, each using their own frequency given to `Trig`.

#### Expanded Version

Although we covered the convience constructors above, we could instead write
the program verbosely by using only explicit constructors. The below code is
exactly equivalent to the first snippet but its added length makes it a bit
unwiedly.

    let amp.instr =
      let osc = Sine(Ramp(Trig())) in
      let asr = ASR1(0.01,0.1) in
        Splice(Chain[Gain 0,Gen((1,8,Mul(asr,osc))))

## MIDI Setup

MIDI devices are specified on the command line using the `midi` option. For
example, the string `--midi="a1 28:0"` will open the ALSA device `28:0` and
assign it the device number 1. The device number is assigned to events
processed by the AMP program.
