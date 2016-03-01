(* compute power using exponential and log *)
let pow a b = exp (b * (log a))

(* compute square root using powers *)
let sqrt v = pow v 0.5

(* compute decibels from an amplitude *)
let amp2db v = 20 * log v / log 10

(* compute amplitude from decibels *)
let db2amp v = pow 10 (v / 20)


(* create a sequence from low to high *)
let seq l h =
  if l <= h
    then l::(seq (l+1) h)
    else []

(* sequence using length *)
let seq' start len = seq start (start+len-1)
