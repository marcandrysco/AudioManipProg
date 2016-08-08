(*
 * Oscillators.
 *)


(**** Sine ****)

(**
 * Sine oscillator with frequency input.
 *   @f (Param): The frequency.
 *   &ret (Module): The module.
 *)
let SineF(f) = Sine (Ramp f)

(**
 * Sine oscillator with warping.
 *   @f (Param): The frequency.
 *   @d (Param): Warping distortion.
 *   &ret (Module): The module.
 *)
let SineW(f,w) = Sine(Warp(Ramp(f),w))
