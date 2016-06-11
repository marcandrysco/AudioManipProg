Audio Manipulation Program
==========================

The Audio Manipulation Program (or AMP) is a set of tools for performing real
time audio synthesis, processing, mixing, and mastering. The project is split
into five components:

  * [cHax](hax/README.Md) - Cross-platform library for augmenting the C
    standard library.
  * [libDSP](dsp/README.md) - Low-level digital signal processing library.
    This library provides the building blocks used to create more
    sophisticated processing effects.
  * [MuseLang](lang/README.md) - Music oriented functional language based off
    of ML. By itself, the language provides a very basic set of features. The
    language must be combined with AmpCore to achieve useful functionality.
  * [AmpCore](core/README.md) - The core library for AMP. This library
    provides a number of high-level components for editing real time audio.
    Wen combined with MuseLang, users can compose complicated real time
    programs that are written in a high-level language.
  * [AmpRT](rt/README.md) - Real time processing application. This program
    reads in high-level programs written in MuseLang+AmpCore and executes the
    program in real time using an audio device as input and output.

The [Tutorial](tut/README.md) provides a way to started using the Audio
Manipulation Program software stack. Documented [Examples](ex/README.md) are
complete, high-level program that demonstrate features a variety of features.

## Licensing

The AMP libraries (libDSP, MuseLang, and AmpCore) are licensed under the GNU
LGPLv3. The applications (AmpRT) are licensed under the GNU GPLv3. The cHax
library is licensed under the MIT License. Source code compiled by MuseLang,
plugins run using AmpCore, and audio created by AMP are _not_ restricted in
any way; authors are free to license their source code, plugins, and audio
however they wish.

## Getting Started

This section covers building the AMP components and using AmpRT with the
provided examples.

### Default Build

The root directory contains a `configure` script for performing a "default"
build. Running `make` will recusively build all subdirectories. The libraries
are locally  installed into `lib` and `include`, and the AmpRT binary is
installed into `bin` and is executed with the command `bin/amprt`.

### Manual Build

Each subdirectory contains an individual `configure` script for generating a
`Makefile`. Once the `Makefile` is generated, `make` will build the binary and
`make install` will install the binary. The subdirectories have the following
dependencies:

  * Everything depends on cHax
  * AmpCore depends on libDSP and MuseLang
  * AmpRT depends on AmpCore

By default, AmpRT will attempt to build with against a default set of audio
APIs. To select different APIs, use the appropriate configure flags (e.g.
`--alsa`, `--pulse`). See each directory for more detailed building
instructions.

### Examples

The [Examples](ex/README.md) provide simple programs for performing real
time processing of audio. The code is heavily documented to both demonstrate
the capabilities of AMP and serve as a foundation for creating new programs.

## Architecture

Audio production tools try to satisfy two competing goals: efficiency (for low
latency and high throughput) and expressiveness (for creating complex audio
constructs). AMP attempts to achieve both objectives using a hybrid approach
that combines an efficient low-level library with a powerful high-level
functional language.

### Digital Signal Processing Library

To satisfy high efficiency, [libDSP](dsp/README.md) provides an extremely fast
set of low-level building blocks. The library is built using heavily optimized
C code to ensure both low latency and high throughput. The "hot" path that
processes audio is carefully written to eliminate sources of slowdowns such as
memory allocation or system calls. Much of the code is directly exported in
headers as static inline functions to eliminate function call overhead.

### MuseLang

[MuseLang](lang/README.md) is a high-level, ML-like functional programming. By
itself, it is very similar to other functional languages such as OCaml,
providing a small set of primitives for basic programs. 

### AmpCore

[AmpCore](core/README.md) forms the foundation of AMP by gluing together the
low-level DSP library with the high-level language. AmpCore provides a large
number of primitive types and classes to MuseLang, each corresponding to a
very efficient music or signal processing function. Bindings for AmpCore
include multi-track recording, distortion, reverberation, equalization, and
more.

Additional functionality may be loaded through the use of plugins that extend
the base set of AmpCore primitives. Plugins should expose their components
through MuseLang so that they may be used directly in high-level, functional
programs.

### AmpRT

The [AmpRT](rt/README.md) is a command-line program that executes an AmpCore
program in real time. Currently, AmpRT only runs on Linux (using ALSA or
PulseAudio).
