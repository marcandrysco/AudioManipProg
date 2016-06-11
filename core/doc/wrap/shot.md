Shot Wrapper
============

The `Shot` wrapper filters incoming events for a single-shot -- only a single
MIDI event device-key pair is passed through to the inner object. 

## Summary

MuseLang constructor

    Shot (obj:Object, (dev:Int, key:Int))

The `obj` is executed normally with a single exception: only events whose
device and key match the `dev`-`key` pair are passed to the internal object.
The `Shot` wrapper is frequency combined with the [`Sample`](../mod/sample.md)
module so tha the sample only fires on a specific MIDI event.
