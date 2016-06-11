Quick Start Guide
=================

This tutorial explains the bare minimum in order to begin working with AMP. By
the end of the tutorial, you will be able to create a very basic AMP programs.

## Building AMP

For the most basic build of AMP, simply run `./configure` and `make install`
from the root directory. This will install the libraries in the `lib`
directory and the binary `amprt` into the bin directory. The remainder of the
tutorial will use the `amprt` binary.

The default build will include the following automatically:

  * Linux: ALSA and PulseAudio
  * No other systems are supported at this time. :(

## Running AmpRT

The `amprt` binary reads a source program and executes it in real time. The
following subsections will cover usage of each device interface using the
example source file `dist.ml`.

*Important!* Most AmpRT programs read audio from the input (e.g. the
microphone) and write it back to the output (e.g. the speaker). If the speaker
volume is too high or the microphone is too sensitive, this will cause a
positive feedback loop that is extremely loud. I strongly suggest using
headphones or direction microphones.

### ALSA

The ALSA interface provides very direct bindings to audio devices in Linux,
thereby providing very low latencies. Note that AmpRT opens the device for
exclusive access, and therefore no other application can use the audio device
at the same time.

	  amprt --alsa "hw:0" "dist.ml"

The parameter passed to the ALSA audio module is `hw:0`. This specifies that
we wish to use the device `hw:0`. Additional parameters may specify specific
hardware options, for example the following invocation requests a sample rate
of `96000`.

	  amprt --alsa "hw:0 r96000" "dist.ml"

Without specifying any hardware options, the ALSA interface selects a
relatively useful set of default values.

### PulseAudio

PulseAudio provides a very simple but high latency interface for AMP. It is
not suitable for real time processing but is useful for purely synthesis-based
AMP programs.

	  amprt --pulse "AMP" "dist.ml"

In the above invocation, PulseAudio takes a single parameter which specifies
the name of the application to show up in the PulseAudio mixer. Additional
parameters may be specified similar to the ALSA interface, but this is rarely
necessary.

## Hello World

Now that we know how to run AmpRT, we will write our first, "hello world"
MuseLang file. Our first program (show below) processes all incoming data
through a low-pass filter with a cutoff frequency of 800Hz.

    let amp.instr = Splice(Lpf(800))

Save the code to a file called `hello.ml`. Execute the MuseLang file using
AmpRT by following the commands in the previous section, replacing `dist.ml`
with `hello.ml`. For example, use `amprt --alsa "hw:0" "hello.ml"` to process
a low-pass filter on the ALSA device `hw:0`.

Hopefully it's obvious that the piece `Lpf(800)` performs the low-pass
filter, but what is with the `Splice` and `amp.instr`? Starting from the left,
the variable `amp.instr` specifies the root instrument processor, which in
this case, is a `Splice` component. You can think of this as your `public
static void main` -- its the entry point for processing data.

Next, we move onto the mysterious `Splice` component. Low-pass filters are
designed to operate on a single stream (or channel) of data, but the root
instrument assumes a multi-channel input. In order to apply the filter, we
use the `Splice` component which merges all input channels into a single
stream before processing it with the filter. Once processed, the output is
copied to all output channels. If you only want to process the left channel,
you can replace `Splice` with `Left`, and likewise with the right channel and
`Right`.
