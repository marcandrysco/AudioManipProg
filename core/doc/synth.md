Synthesizer
===========


## Synth Module

    Synth (dev:int, cnt:int, mod:Module)



## Trigger

    Trig mul:Int

Triggers generate a constant signal at the synthesizer frequency. For example,
if playing the note A4, the trigger will output a constant signal of 440.
Triggers are almost exclusively used to control other modules or effects,
primarily oscillators and filters. The `mul` parameter specifies which
multiple of the trigger frequency to output -- e.g. `Trig 2` will output twice
the frequency (880 Hz for the note A4).


## Attack Decay Sustain Release (ADSR)

    ADSR (atk:float, decay:float, sus:float, rel:float)
    AtkRel (atk:float, rel:float)  (* = ADSR(atk,0,0,rel) *)
