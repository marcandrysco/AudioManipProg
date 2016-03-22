(* dist.ml
 *   Guitar Distortion
 *
 * This sample provides a simple distortion effect that gets applied to the
 * input device. The inputs are mixed into a mono signal before processing.
 * The output is written to both left and right output channels.
 *
 * Licensed under the MIT License (see LICENSE for details).
 *)


(***** Options *****)

(*
 * The following variables control the different aspects of the distortion
 * effect. Other than 'pregain', all controls should be set to values in the
 * range zero to ten.
 *)

(* pregain: The amount of gain to be applied before any effects. This is
 *   specified in decibels. *)
let pregain = 0

(* dist: The amount of clipping based distortion. *)
let dist = 5

(* tone: The tone control between low and high frequencies. A value of zero
 *   accents low-end and a value of ten accents the high-end. *)
let tone = 5

(* crush: The amount of bitcrushing to be applied. *)
let crush = 5

(* drive: How much gain should be applied to the tube amp modeling. Setting
 *   this to zero will completely remove the tube amp model. *)
let drive = 5

(***** End Options *****)


let chain = Chain []

(* the root element is a two-channel instrument 'amp.instr', the splice
 * instrument used to combine channels and apply a one-channel effect chain *)
let amp.instr = Splice(chain)
