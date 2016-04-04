Clip Effect
===========

The clip effect performs a quadratic soft clipping of the input. 

## Summary

MuseLang constructor

    Clip (maxlo:Param, satlo:Param, sathi:Param, maxhi:Param)

The parameters denote the points at which the clipper begins to saturate and
when it is fully saturated. Between `satlo` and `sathi`, the clipper behaves
linearly with no distortion. Outside the linear region incurs saturation
where quadratic interpolation is used to compute the output. The output will
never fall below `maxlo` or above `maxhi`.

## Detailed Operation

There are five regions of operation: linear, (low/high) partial saturation,
and (low/high) complete saturation. The partial saturation regions are
computed using a parabola to guarantee linear operation near `satlo`/`sathi`
and a flat response near `maxlo`/`maxhi`.

Using `x` as the input and `y` as the output, the regions are described by:
`y = x` in the linear region; `y = c` for a constant `c` in full saturation;
and `y = a(x-h)^2 + c` for constants `a`, `c`, and `h` in partial saturation.
During full saturation, the output should be exactly `maxlo` or `maxhi`. For
partial saturation, the output `y` should continuously change behavior from
linear to full saturation, generating the constraints `dy/dx = 1` near
linearity and `dy/dx = 0` near full saturation, `y(sat) = sat` and `y(h) =
max` where `h` transitions from partial to full saturation.

Solving the quadratic for partial saturation gives `h = 2*max - sat`, `a = 1 /
(4 * (sat-max))`, and `c = max`. For full saturation, `c = max`. The value of
`h` corresponds to the input value `x` at which the clipper will be fully
saturated.
 
    h = 2*max - sat
    a = 1 / (4 * (sat-max))

    y = | x < sat -> x
        | x < h   -> a * (x-h)^2 + max;
        | else    -> max;
