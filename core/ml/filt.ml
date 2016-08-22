(*
 * Filter helpers.
 *)


(**** Peaking Filter ****)

(**
 * Notch filter derived from a peaking filter.
 *   @f (Param): The frequency to remove.
 *   @q (Param): The quality of the notch.
 *   &ret (Effect): The notch filter as an effect
 *)
let Notch (f,q) = Peak (f,0,q)
