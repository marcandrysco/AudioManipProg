Math
====

Functions ending with `f` are computing using `float`s (single-precision)
whereas functions ending with `d` are computing using `double`s
(double-precision).


## Constants

    LOGHALF /* = log(0.5) */


## Decay Constant

    double dsp_decay_d(double target, double len)
    float dsp_decay_d(float target, double len)
    double dsp_half_d(double len)
    float dsp_half_f(float len)

The above functions are used to compute decay constants useful in a number of
algorithms. More specificially, the decay constant provides a fast way of
computing an exponential curve towards zero.

The `dsp_decay_d` and `dsp_decay_f` functions compute a constant that decays
to `target` after `len` steps. If target is `0.5`, use the `dsp_half_d` or
`dsp_half_f` instead.

The `dsp_half_d` and `dsp_half_f` functions compute a decay constant with an
target of exactly `0.5`. Because the target is a constant, these functions are
much faster than the more general form.

### Detailed Operation

The general equation for compute a decay constant is `c=(1-t)^(1/l)` for a
target `t` and length `l`. If the target is `0.5` the alternate formula
`c=exp(log 0.5/l)` is used, where `log 0.5` is precomputed as a constant. The
use of `exp` and `log` slightly reduces precision but vastly improves speed.

The resulting constant satisfies the formula `c^l=t`, such that the value
`1.0` multiplied by the constant `c` exactly `l` times produces the target
value `t`. For digital signal processing, starting at a volume `v[0]` and
iteratively computing successive values `v[n] = v[n-1] * c`, the `l`th value
of `v` will be exactly the target `t`. This produces an efficient method of
computing an expontially decaying volume.
