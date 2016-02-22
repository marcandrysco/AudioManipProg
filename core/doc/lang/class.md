AmpCore Classes
===============

The classes provided by AmpCore are listed below:

* [Value](#value)
* [Handler](#handler)
* [Effect](#effect)
* [Module](#module)
* [Instrument](#instr)

The set of classes can be extended through plugins. Additional classes should
be careful to not clash with existing classes.

## Value

The `Value` class is used for providing either a constant `number` or a
user-controllable `Handler`.

    Values = number
           | Handler

When given a `number`, the consumer component will use the value as a constant
that will not vary over the course of a program. When given a `Handler`, the
value may change through a user interface or MIDI device. Unlike a `Signal`,
`Values` do not provide a continuous change and are therefore unsuitable for
components such as synthesizers.

## Handler

The `Handler` class provides a mechanism for processing MIDI events. The main
instance consists of the `Ctrl` handler for MIDI events, although a number of
other handlers can preprocess the event.

    Handler = Ctrl

## Effect

The `Effect` class performs simple processing of a mono signal. All parameters
to an effect are fixed values that may be changed using MIDI events. This
contrasts the `Module` class where parameters may be continuously changing
values.

    Effect = Chain [Effect]
           | Clip (Value, Value, Value, Value)
           | Comp (Value, Value, Value, Value)
           | Gain (Value)

## Module


## Instrument

The `Instr` or Instrument class

    Instr = Mixer [Instr]
          | Pan ((Value, Value), (Value, Value))
          | Splice (Effect, (Num, Num), (Num, Num))
