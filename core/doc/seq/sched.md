Scheduler
=========


## Summary

MuseLang constructor

    Sched [(d,f),(d,d),d]

The scheduler takes a list of events as input that are played back at the
appropriate times. Each event consists of a triplet: a time `(bar,beat)`, a
key `(device,key)` and a value.
