AmpCore Language Extensions
===========================

The AmpCore library extends [MuseLang](../../lang/README.md) by providing a
large number of new primitive types. An instance of each type corresponds to a
block of low latency C code that executes in real time.

## Core Classes

AmpCore provides a small set of core classes, each of which is implemented by
a large number of instance classes. Complete information is found in the
[Class List](class.md). The core classes are summarized below:

  * [Effect](index.md#effect) - Effects perform a continuous transformation
    from a single channel input to a single channel output. Examples include
    [distortion](efx/dist.md) and [chorus](efx/chorus.md).

  * [Module](index.md#module) - Modules generate a single channel of audio,
    discarding any input. Examples include [oscillators](mod/osc.md) and
    [attack-decay-sustain-release](mod/adsr.md).

  * [Instrument](index.md#instrument) - Instruments are the N-channel
    equivalent of an effect; they transform a multi-channel input to a
    multi-channel output.

  * [Sequencer](index.md#sequencer) - Sequencers process and generate events
    that are consumed by signal processing components (effects, modules, or
    instruments).

  * [Clock](index.md#clock) - Clocks compute the time (bar and beat) for every
    time step. The time information is may used by processing components. For
    example, an oscillator can be set to cycle once per beat.
