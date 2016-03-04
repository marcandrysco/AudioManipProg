(**
 * Compute a mathematical power 'a^b'.
 *   @a (num): The base.
 *   @b (num): The exponent.
 *   &returns (num): The result 'a^b'.
 *)
let pow a b = exp (b * (log a))

(**
 * Compute a square root.
 *   @v (num): The value.
 *   &returns (num): The result 'sqrt(v)'.
 *)
let sqrt v = pow v 0.5

(* compute decibels from an amplitude *)
let amp2db v = 20 * log v / log 10

(* compute amplitude from decibels *)
let db2amp v = pow 10 (v / 20)


(* create a sequence from low to high *)
let nat l h =
  if l <= h
    then l::(nat (l+1) h)
    else []

(* sequence using length *)
let nat' start len = seq start (start+len-1)


(*** Attack-Sustain-Decay-Release variants ***)

(* AtkRel (attack:Num, release:Num)
 *   Only an attack and release without sustain *)
let AtkRel ((l,h),(a,r)) = ADSR ((l,h),(a,r,0,1))

(* ASR (attack:Num, release:Num)
 *   ASDR without any decay, sustained at 1 *)
let ASR ((l,h),(a,r)) = ADSR ((l,h),(a,0,1,r))
