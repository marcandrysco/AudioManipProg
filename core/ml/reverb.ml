(*
 * Reverb helpers.
 *)


(**** Delay ****)

(**
 * Delay constructor.
 *   @l (float): Delay length.
 *   @g (float): The gain.
 *   &ret (Effect): The delay effect.
 *)
let Delay (l,g) = DelayV (l,l,g)

(**
 * Pure delay constructor.
 *   @l (float): Delay length.
 *   &ret (Effect): The delay effect.
 *)
let Delay1 l = Delay (l,1)


(**** Comb ****)

(**
 * Comb feedack filter.
 *   @l (float): The delay length.
 *   @g (float): The gain.
 *   &ret (Effect): The comb effect.
 *)
let Comb (l,g) = CombV (l,l,g)


(**** Allpass ****)

(**
 * Allpass feedack filter.
 *   @l (float): The delay length.
 *   @g (float): The gain.
 *   &ret (Effect): The comb effect.
 *)
let Allpass (l,g) = AllpassV (l,l,g)


(**** Resonance Comb Filter ****)

(**
 * Allpass feedack filter.
 *   @l (float): The delay length.
 *   @g (float): The gain.
 *   @f (float): The frequency.
 *   @q (float): Quality.
 *   &ret (Effect): The comb effect.
 *)
let Rescf (l,g,f,q) = RescfV (l,l,g,f,q)

let RescfF (f,g,q) = Rescf (1.0/f,g,f,q)
