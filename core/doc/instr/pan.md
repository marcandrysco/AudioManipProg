Pan Instrument
==============

Perform volume and delay panning on a stereo instrument.

## Summary

MuseLang constructor

    Pan ((left-volume:Parameter, right-volume:Parameter), (left-delay:Num, right-delay:Num))

The volume of each channel is a parameter representating the amount of scaling
to apply. To adjust the volume by a decibel level, specify the volume using
`amp2db`. The delay of each channel is a constant number representing the
amount of delay in seconds.
