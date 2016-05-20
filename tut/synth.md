Synthesizer Tutorial
====================

This tutorial will walk you through building a basic synthesizer using AMP.
For a complete and documented synthesizer, see the [Synthesizer
Example](../ex/synth.ml).

## Minimal Synthesizer

To start, let's see an entire program for a very basic synthesizer.

    let amp.instr =
      let osc = Sine'(Trig()) in
      let asr = ASR1(0.01,0.1) in
        Splice(Synth(1,8,Mul(asr,osc)))
