(*
 * Control helpers.
 *)


(**** Basic Control ****)

(**
 * Create a control that varies from zero to one.
 *   @i (int,int): The device ID pair.
 *   @d (float): Default value.
 *   &ret (Param): The parameter.
 *)
let Ctrl1 (i,d) = Ctrl(i,(0,1),d)

(**
 * Create a standard control with an initial low value.
 *   @i (int,int): The device ID pair.
 *   &ret (Param): The parameter.
 *)
let Ctrl1L i = Ctrl1(i,0)

(**
 * Create a standard control with an initial middle value.
 *   @i (int,int): The device ID pair.
 *   &ret (Param): The parameter.
 *)
let Ctrl1M i = Ctrl1(i,0.5)

(**
 * Create a standard control with an initial high value.
 *   @i (int,int): The device ID pair.
 *   &ret (Param): The parameter.
 *)
let Ctrl1H i = Ctrl1(i,1)
