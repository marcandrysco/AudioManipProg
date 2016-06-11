let amp.instr =
  let osc = Mul(SineW(Trig(),0.8),0.1) in
  let asr = ASR1(0.01,0.1) in
    Splice(SynthGen'(1,8,Mul(asr,osc)))
