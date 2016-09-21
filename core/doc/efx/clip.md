Clip Effect
===========

The clipping effects are used to non-linearly distort the input once it
exceeds a saturation point. The distortion parameter controls the strength of
the clipping effect. There are four forms of every clipper.

  * Asymmetric with independent saturation and distortion for positive and
    negative input.
  * Symmetric where both positive and negative inputs clipped equally.
  * Positive where only positive signals are clipped.
  * Negative where only negative signals are clipped.


## Hard Clipping

    HardClipA (satlo:Param, sathi:Param)
    HardClipS (sat:Param)
    HardClipP (sat:Param)
    HardClipN (sat:Param)

Hard clipping prevents the output from exceeding the saturation point -- any
input beyond saturation will produce an output exactly at saturation. This
form of clipping generates a very harsh result that typically benefits from
subsequent low-pass filter.

### Detailed Operation

The hard clipping effect is computed as `y[n] = min(x[n], s[n])` for positive
input `x[n]` and `y[n] = max(x[n], s[n])` for negative input. The saturation
is given by `s[n]`, the input by `x[n]`, and the output by `y[n]`.


## Linear Clipping

    LinClipA (satlo:Param, distlo:Param, sathi:Param, disthi:Param)
    LinClipS (sat:Param, dist:Param)
    LinClipP (sat:Param, dist:Param)
    LinClipN (sat:Param, dist:Param)

Linear clipping applies a linear scaling of the output beyond the saturation
point. The `dist` parameter controls the rate at which the output is scaled.
In terms of clipping, the linear clipper is fairly poor and is better used to
add small amounts of distortion.

### Detailed Operation

    y[n] = (x[n] - s[n]) / (1 + d[n]) + s[n]

The linear clipper allows the signal to pass through below the saturation
point. Above `sat` the output `y[n]` exactly follows the above equation,
producing a output to input ratio of `1/(1+d)`.


## Polynomial Clipping

    PolyClipA (satlo:Param, distlo:Param, sathi:Param, disthi:Param)
    PolyClipP (sat:Param, dist:Param)
    PolyClipN (sat:Param, dist:Param)
    PolyClipS (sat:Param, dist:Param)


## Root Clipping

    RootClipA (satlo:Param, distlo:Param, sathi:Param, disthi:Param)
    RootClipP (sat:Param, dist:Param)
    RootClipN (sat:Param, dist:Param)
    RootClipS (sat:Param, dist:Param)


## Logrithmic Clipping

    LogClipA (satlo:Param, distlo:Param, sathi:Param, disthi:Param)
    LogClipP (sat:Param, dist:Param)
    LogClipN (sat:Param, dist:Param)
    LogClipS (sat:Param, dist:Param)


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
