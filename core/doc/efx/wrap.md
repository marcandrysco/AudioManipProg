Wrap Effect
===========

Perform wrapping when an input exceeds a limit. The wrap effect prevents
discontinuities in the output.

## Summary

MuseLang constructor

    Wrap (limit:Param)

The wrap effect takes guarantees that the output falls within the range
'[-limit,limit]' by wrapping values that exceed the limits. Unlike integer
wrapping that causes discontinuities, values that exceed a limit bend
backward into the output range. For an example wrap with a limit of '1.0', the
sequence of values '(0.8,1.1,1.3)' would become '(0.8,0.9,0.7)'. For inputs
that fall significantly outside the linear range, the wrap effect adds a very
strong set of additional harmonics.
