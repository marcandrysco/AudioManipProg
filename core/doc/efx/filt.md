Filter Effects
==============


## MuseLang

Filters are available in MuseLang using constructors specific for each type of
filter. All filters accept either constant or time varying inputs (i.e.
modules or controls).

### Low-Pass Filter

    Lpf (freq)
    Lpf2 (freq)
    Lpf3 (freq)
    Lpf4 (freq)

The low-pass filters are implemented using a butterworth design giving a cut
of `8*n` decibels per octave.

### High-Pass Filter

    Hpf (freq)
    Hpf2 (freq)
    Hpf3 (freq)
    Hpf4 (freq)

The high-pass filters are implemented using a butterworth design giving a cut
of `8*n` decibels per octave.

### Band-Pass Filter

    Bpf (freqlo, freqhi)

The band-pass filter cascades a low-pass and high-pass filter creating a
band-pass with `8*n` decibels per octave cut below `freqlo` and above
`freqhi`.

### Resonance Filter

    Res (freq, qual)

Resonance filters are a form of band-pass filters that remove everything
except data around the center frequency `freq`. The `qual` parameter specifies
the width of the band, with values above `1` creating a tall and
narrow peak. Especially high quality values will creating a ringing at the
center frequency.

### Peaking Filter

    Peak  (freq, gain, qual)
    Notch (freq, qual)        (* = Peak (freq, 0, qual) *)

Peaking filters modify only a region around the `freq` by the specified
`gain`. The `qual` parameter determines how narrow (large `qual`) or wide
(small `qual`) the band of effected frequencies.

### Ringing Filter

   Ringf (freq, gain, tau)
