Digital Signal Processing Library
=================================

The digital signal processing libray (libDSP) provides very low-level
primitives for performing computation over signals. In order to reduce
overhead as much as possible, most components are exported directly through
the header `libdsp.h` as static inline functions, eliminating function call
overhead and enabling better optimization by the compiler.
