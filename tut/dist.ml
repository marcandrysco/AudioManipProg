let PolyClipA(a,b,c,d) = Chain[PolyClipP(c,d),PolyClipN(-b,-a)]

let amp.instr =
  let dist = Chain[Boost(34),LogClipS(0.1,1.0),Cut(18)] in
    Splice(dist)
