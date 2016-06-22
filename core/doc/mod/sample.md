Sample Module
=============

The sample module is used to load sample files and play them back when
triggered by a MIDI event. A single sample module can load a set of samples
that provide round-robin and velocity-sensitive playback.

## Summary

MuseLang constructor

    Sample (simul:Int, decay:Float, files:[[String]])
    Sample' (files:[[String]])
    Sample1 (simul:Int, decay:Float, files:String)
    Sample1' (file:String)
    SampleShot (simul:Int, decay:Float, files:[[String]], (dev:Int, key:Int))
    SampleShot' (files:[[String]], (dev:Int, key:Int))
    SampleShot1 (simul:Int, decay:Float, files:String, (dev:Int, key:Int))
    SampleShot1' (file:String, (dev:Int, key:Int))
