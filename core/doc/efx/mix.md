Mix effect
==========

Mixes the dry input with the wet effect.

## Summary

MuseLang constructor

    Mix (ratio:Param, effect:Effect)

Processes the `effect` on the input, and mixes the wet effect with the dry
input with the given `ratio`. A value of `0` mixes only dry; a value of
`1` mixes only wet`; and a value of `0.5` mixes dry and wet evenly.
