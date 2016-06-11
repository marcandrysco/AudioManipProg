(**
 * Create panning instrument on a single channel.
 *   @n (int):     The channel number.
 *   @v (float):   The volume.
 *   @d (float):   The delay in second.
 *   &ret (Instr): The instrument
 *)
let Pan(n,v,d) = Single(n,Chain[Gain v,Delay d])

(**
 * Create panning instrument on the left channel.
 *   @v (float):   The volume.
 *   @d (float):   The delay in second.
 *   &ret (Instr): The instrument
 *)
let PanLeft(v,d) = Pan(0,v,d)

(**
 * Create panning instrument on the right channel.
 *   @v (float):   The volume.
 *   @d (float):   The delay in second.
 *   &ret (Instr): The instrument
 *)
let PanRight(v,d) = Pan(1,v,d)


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


(****)
(* Module Components *)
(****)


(**** Samples ****)

(**
 * Load a sample with fast decay.
 *   @p (List):     The file path list of lists.
 *   &ret (Module): The sample module.
 *)
let Sample'(p) = Sample(3,0.01,p)

(**
 * Load a single sample.
 *   @n (int):      The number of simultaneous executions.
 *   @l (float):    The sustain length.
 *   @p (string):   The file path.
 *   &ret (Module): The sample module.
 *)
let Sample1(n,l,p) = Sample(n,l,[[p]])

(**
 * Load a single sample.
 *   @p (string):   The file path.
 *   &ret (Module): The sample module.
 *)
let Sample1'(p) = Sample1(3,0.01,p)

(**
 * Load a single-shot sample.
 *   @n (int):      The number of simultaneous executions.
 *   @l (float):    The sustain length.
 *   @p (List):     The file path list of lists.
 *   @k (Int,Int):  The key filter.
 *   &ret (Module): The sample module.
 *)
let SampleShot1(n,l,p,k) = Shot(Sample(n,l,p),k)

(**
 * Load a single-shot sample with fast decay.
 *   @n (int):      The number of simultaneous executions.
 *   @k (Int,Int):  The key filter.
 *   &ret (Module): The sample module.
 *)
let SampleShot'(p,k) = Shot(Sample(3,0.01,p),k)

(**
 * Load a single-shot single sample.
 *   @n (int):      The number of simultaneous executions.
 *   @l (float):    The sustain length.
 *   @p (string):   The file path.
 *   @k (Int,Int):  The key filter.
 *   &ret (Module): The sample module.
 *)
let SampleShot1(n,l,p,k) = Shot(Sample(n,l,[[p]]),k)

(**
 * Load a single-shot single sample with fast decay.
 *   @p (string):   The file path.
 *   @k (Int,Int):  The key filter.
 *   &ret (Module): The sample module.
 *)
let SampleShot1'(p,k) = Shot(Sample1(3,0.01,p),k)


(**** Synthesizer Components ****)

(**
 * Create an oscillator with a frequency.
 *   @f (Module): The frequency.
 *   &ret (Module): The oscillator module.
 *)
let Sine'(f) = Sine(Ramp(f))
let SineW(f,w) = Sine(Warp(Ramp(f),w))
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
let SynthGen (d,n,m) = Gen(Synth(d,n,m))
let SynthGen' (d,n,m) = Gen'(Synth(d,n,m))

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
