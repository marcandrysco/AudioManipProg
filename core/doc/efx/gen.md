Generator Effect
================

Generators are used to inject the signal from a module into a stream as an
effect. By default, AmpRT processes a signal from input to output, whereas
modules such as a synthesizer do not use an input. Generates bridge the gap by
acting as an effect while writing the output from module into the stream.


## Summary

    Gen  mod:Module
    Gen0 mod:Module (* = Chain[Mute, Gen mod] *)

The `Gen` effect injects the output of the module `mod` into the input stream
of the generator. Because the signals are added, any input to `Gen` will be
passed to the output. For some modules (e.g. synthesizers), this will cause
the input (e.g. microphone) to unexpectedly leak to the output. To fix this,
use the `Gen0` constructor

The `Gen0` effect first zeroes the input before processing the module. This
prevents the input signal from leaking into the output.
