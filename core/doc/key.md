Keys and Notes
==============


## Key Representations

Keys have a set of three equivalent representations:

 * String names following [Scientific Notation] such "C4".
 * Integer values ranging from 0 to 255 where a value of 0 corresponds to
   "C0".
 * Frequency values of the respective key (values may vary depending on
   concert pitch).

Most AmpCore components expect integer values for a key; the notable exception
are synthesizer components which use raw frequencies. The string
representation is primarily a convenience for users.

### Conversion

AmpCore provides the following set of functions for converting between the key
representations. Note that frequency values depends on the concert pitch; by
default, AmpCore assumes a concert pitch of 440Hz for A4.

    keyS ival  (* to string -- from int *)
    keyF ival  (* to freq   -- from int *)
    keyN str   (* to number -- from string *)

The `keyS` function takes an integer value and converts it to the string repesentation.
The conversion chooses an arbitrary name for notes that are [enharmonic
equivalents].

The `keyF` function takes an integer and produces the frequency corresponding
to the key using a default concert pitch of 440Hz for A4 (and middle C of
261.63 Hz).

The `keyN` function converts a string note name to its corresponding integer
value. The note name consists of a single letter, followed by at most two
sharps '#' or flats 'b', and ending with a single number giving the octave.
For example, the key value 49 corresponds to note name 'C#4'.


## Modes

    modeI mode ival
    modeS mode root ival

    majI ival
    majS root ival
    minI ival
    minS root ival

[Scientific Notation]: https://en.wikipedia.org/wiki/Scientific_pitch_notation
[Enharmonic Equivalents]: https://en.wikipedia.org/wiki/Enharmonic
