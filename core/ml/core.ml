let map f l = mapl (f,l)

(*
  match l with
    | h::t -> (f h) :: (map f t)
    | _    -> []
*)
let println s = print (s ++ "\n")

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
let db v = db2amp v
let gain v = db2amp v
let cut v = db2amp (-v)


(* create a sequence from low to high *)
let nat l h =
  if l <= h
    then l::(nat (l+1) h)
    else []

(* sequence using length *)
let nat' start len = seq start (start+len-1)


(*** Attack-Sustain-Decay-Release variants ***)

(* Attack-Release constructor
 *   @l,h (num,num): The low and high value pair.
 *   @a,r (num,num): The attack and release lengths.
 *   &ret (Module): The ADSR module.
 *)
let AtkRel ((l,h),(a,r)) = ADSR ((l,h),(a,r,0,r))
let AtkRel' (a,r) = AtkRel ((0,1),(a,r))
let AtkRel1 (a,r) = AtkRel ((1,1),(a,r))

(* ASR (attack:Num, release:Num)
 *   ASDR without any decay, sustained at 1 *)
let ASR ((l,h),(a,r)) = ADSR ((l,h),(a,0,1,r))


(*** Alternate constructors ***)

(**
 * Mute the input.
 *   &returns (Effect): Muted effect.
 *)
let Mute = Gain 0

(**
 * Pass through the input.
 *   &returns (Effect): Unchanged output.
 *)
let Thru = Gain 1

(**
 * Perform standard scaling of with an input with unity gain.
 *   @l (float): The low output.
 *   @h (float): THe high output.
 *   &returns (Effect): Standard scaling effect.
 *)
let Scale'(l,h) = Scale(-1,1,l,h)

(**
 * Scale a standard input from unity gain to (0,1).
 *   &returns (Effect): Scaling to (0,1) effect.
 *)
let Scale1 = Scale'(0,1)

let Sine(f,w) = Osc("sine",f,w)
let Sine1(f,w) = Patch(Sine(f,w),Scale1)

let Saw(f,w) = Osc("tri",f,w)
let Saw1(f,w) = Patch(Saw(f,w),Scale1)

let Pulse(f,w) = Osc("square",f,w)
let Pulse1(f,w) = Patch(Pulse(f,w),Scale1)

(**
 * Created a muted generator.
 *   @m (Module): The module.
 *   &returns (Effect): The generator with muted input.
 *)
let Gen' m  = Chain[Mute,Gen(m)]

(**
 * Process an effect on the left channel.
 *   @e (Effect): The effect.
 *   &returns (Instr): The instrument.
 *)
let Left e = Single(0,e)

(**
 * Process an effect on the right channel.
 *   @e (Effect): The effect.
 *   &returns (Instr): The instrument.
 *)
let Right e = Single(1,e)
