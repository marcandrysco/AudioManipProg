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

(*** Helper functions ***)

(**
 * Compute a value from a velocity
 *   @v (float): The velocity.
 *   &ret (int): The value.
 *)
let vel v =
  if v >= 1.0
    then 65535
    else if v <= 0
      then 0
      else v * 65535


(*** Attack-Decay-Sustain-Release variants ***)

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
let ASR' (a,r) = ASR((0,1),(a,r))
let ASR1 (a,r) = ASR((1,1),(a,r))

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


(**** Synthesizer Components ****)

(**
 * Create an oscillator with a frequency.
 *   @f (Module): The frequency.
 *   &ret (Module): The oscillator module.
 *)
let Sine'(f) = Sine(Ramp(f))
let Tri'(f) = Tri(Ramp(f))
let Square'(f) = Square(Ramp(f))

(*
let Sine(f,w) = Osc("sine",f,w)
let Sine'(f,w,l,h)  = Patch(Sine(f,w),Scale'(l,h))
let Sine1(f,w) = Patch(Sine(f,w),Scale1)

let Saw(f,w) = Osc("tri",f,w)
let Saw1(f,w) = Patch(Saw(f,w),Scale1)

let Pulse(f,w) = Osc("square",f,w)
let Pulse1(f,w) = Patch(Pulse(f,w),Scale1)
*)

(**
 * Create a muted generator.
 *   @m (Module): The module.
 *   &ret (Effect): The generator with muted input.
 *)
let Gen' m  = Chain[Mute,Gen(m)]

(**
 * Create synthesizer generator.
 *   @d (int): The device.
 *   @n (int): The number of simultaneous outputs.
 *   @m (Module): The module.
 *   &ret (Effect): The generator.
 *)
let SynthGen (d,n,m) = Gen(Synth((d,0),n,m))
let SynthGen' (d,n,m) = Gen'(Synth((d,0),n,m))

(**
 * Process an effect on the left channel.
 *   @e (Effect): The effect.
 *   &ret (Instr): The instrument.
 *)
let Left e = Single(0,e)

(**
 * Process an effect on the right channel.
 *   @e (Effect): The effect.
 *   &ret (Instr): The instrument.
 *)
let Right e = Single(1,e)

(**
 * Process an effect on both channels.
 *   @e (Effect): The effect.
 *   &ret (Instr: The instrument.
 *)
let Both e = Series[Left e,Right e]

(**
 * Boost a signal in decibels.
 *   @v (float): The number of decibels to boost.
 *   &ret (Effect): The gain effect.
 *)
let Boost v = Gain(db2amp v)

(**
 * Cut a signal in decibels.
 *   @v (float): The number of decibels to cut.
 *   &ret (Effect): The gain effect.
 *)
let Cut v = Gain(db2amp (-v))


(**** Filters ****)

(**
 * Band-pass filter.
 *   @l (float): The low frequency.
 *   @h (float): The high frequency.
 *   &ret (Effect): The filter effect.
 *)
let Bpf (l,h) = Chain[Lpf h,Hpf l]

let Res2 (f,q) = Chain[Res(f,q),Res(f,q)]
let Res2' (g,f,q) = Chain[Boost(g),Res(f,q),Res(f,q)]
let Res3 (f,q) = Chain[Res(f,q),Res(f,q),Res(f,q)]
let Res3' (g,f,q) = Chain[Boost(g),Res(f,q),Res(f,q),Res(f,q)]
