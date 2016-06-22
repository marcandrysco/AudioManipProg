Reverb Effect
=============

The reverb effects are individual components used to constructor a complete
reverberator. Note that all reverberator effects do not let pass through the
original signal -- complete reverberators should manually mix the clean input
with the reverb output.

## All-Pass Filter

Allpass filter reverberator section. The allpass reverberator generates a a
"colorless" sequence of decaying echoes. Allpass sections are ideal for very
short and dense echoes with a delay length of under 5ms.

    Allpass (len:float, gain:Param)

The `len` value specifies the delay between echoes in seconds. For allpass
sections, this parameter is usually around 1ms. The `gain` parameter specifies
the amount of gain to be applied to each successive echo. Higher value create
a long chain of echoes but large values will lead to ringing in the output.
Typical values range from 0.4 to 0.7.

## Delay

The delay effect generates a delayed version of the input signal. Delay
elements are the basis of finite impulse response (FIR) filters.

    Delay (len:float, gain:Param)
    Delay1 (len:float)

The `len` value specifies the amount of time in seconds that the signal is
delayed. The `gain` specifies the amount of gain applied to the output. The
special constructor `Delay1` creates a pure delay without apply a gain to the
output.

## Comb Filter

MuseLang constructor

    Comb (len:float, gain:Param)

## Lpcf

MuseLang constructor

    Lpcf (len:float, gain:Param, freq:Param)
