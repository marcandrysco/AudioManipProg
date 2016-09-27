AmpCore Language Extensions
===========================

[Controls](core/ctrl)

[Filters](core/filt)

[Keys and Mode](core/key)

[Oscillators](core/osc)


## MuseLang Language Extensions

### Effects

Effect are components that process data on a single channel. Effects are the
most basic component for altering an audio stream, including common processors
such as filters, reverb, and phasers. Due to their nature, effects are
typically chained together end-to-end using a `Chain` in order to produce more
elaborate effects. Effects can be applied to modules by using the `Patch`
module.

* [Gain](core/efx/gain) -- Gain effects provide control of the amplitude (or gain)
  of the processed signal.
* [Generator](core/efx/gen) -- Generators enable modules to inject their signal
  onto the signal.
* [Reverb](core/efx/reverb) -- Reberators create a series of delaying echoes
  of the oringal signal.

### Modules

Modules are signal generators that produce a single output signal in the
absense of an input signal. Typical modules, such as an oscillator, operate
over a set of input paremeters, each of which may be a constant, a user
controlled value, or another module. As a consequence, modules are typically
composed together to create more complex effects, such as ring-moduled
oscillators.

* [Patch](core/mod/patch) -- Patch modules process the output of a module
  using an effect, such as filtering the output of an oscillator.
* [Piano](core/mod/piano) -- The piano module combines audio samples and MIDI
  events to simulate a piano.


## Internal API


