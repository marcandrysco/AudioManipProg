Controls
========

Controls are used to convert events from MIDI devices an sequencers into
signals used by effects and modules. MIDI event consists of triple containing
the device, key, and value, each given by a 16-bit unsigned integer. The
output is a stream of values that may be used as parameters or inputs to
modules or effects.


## Basic Control

    Ctrl   ((dev, key), (low, high), def)

Controls are used to generate signals based off of a MIDI input. The `dev` and
`key` specify the listening device and key. The values `low` and `high`
specify the range of output values. Because the AmpCore control cannot know
the current position of the MIDI device, it begins using the default value
given by `def`.
    
    Ctrl1  ((dev, key), def)    (* = Ctrl  ((dev, key), (0, 1), def) *)
    Ctrl1L (dev, key)           (* = Ctrl1 ((dev, key), 0.0) *)
    Ctrl1M (dev, key)           (* = Ctrl1 ((dev, key), 0.5) *)
    Ctrl1H (dev, key)           (* = Ctrl1 ((dev, key), 1.0) *)

For convenience, AmpCore provides additional constructors with implicit
parameters. The `Ctrl1` constructor uses a default output range of 0 to 1 and
but still requires default. The remaining three contructors `Ctrl1L`,
`Ctrl1M`, and `Ctrl1H` uses a low (0.0), medium (0.5), and high (1.0)
default values, respectively.
