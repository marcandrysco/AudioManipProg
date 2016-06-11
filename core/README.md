AMP Core Library (AmpCore)
==========================

The AMP Core Library provides the bulk of the signal processing code, written
using low-level C and exposed through a high-level functional language.
The [Language Reference](doc/index.md) documents the components exposed by
AmpCore through MuseLang. Further reading includes the
[Tutorials](../tut/README.md) and [Examples](../ex/README.md).

## Licensing

The AmpCore library is licensed under the GNU LGPLv3, the full text which is
found at [LICENSE](LICENSE). Source code compiled by MuseLang, plugins run
using AmpCore, and audio created by AMP are _not_ restricted in any way;
authors are free to license their source code, plugins, and audio however they
wish.

## Building

Run the `configure` script in order to generate a `Makefile`. The library is
built and installed using `make` and `make install`. If you are installing to
a system directory, instead install with `sudo make install`. The entire
process is outlined below.

    $ ./configure
    $ make
    $ sudo make install
