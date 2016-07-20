(*
 * Mode and note helpers.
 *)


(**** Scale ****)

(**
 * Major scale calculation.
 *   @r (int): The root node.
 *   @n (int): The offset number from the root.
 *   &ret (int): The 'nth' note in the major scale.
 *)
let majS (r,n) = modeS(1,r,n)

(**
 * Minor scale calculation.
 *   @r (int): The root node.
 *   @n (int): The offset number from the root.
 *   &ret (int): The 'nth' note in the minor scale.
 *)
let minS (r,n) = modeS(6,r,n)


(**** Interval ****)

(**
 * Major interval calculation.
 *   @n (int): Thee offset from the root.
 *   &ret (int): The 'nth' note away from the root.
 *)
let majI n = modeI(1,n)

(**
 * Minor interval calculation.
 *   @n (int): Thee offset from the root.
 *   &ret (int): The 'nth' note away from the root.
 *)
let minI n = modeI(6,n)
