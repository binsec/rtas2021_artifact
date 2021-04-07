type boolean =
  | Bottom
  | True
  | False
  | Top

include Datatype_sig.S with type t = boolean
val inter: t -> t -> t
val join: t -> t -> t
val bottom : t
val top : t  
val includes: t -> t -> bool
val includes_or_widen : previous:boolean -> boolean -> bool * boolean
val is_bottom: t -> bool
val widen : previous:boolean -> boolean -> boolean  
val singleton: bool -> boolean


val boolean_bottom: boolean

(* The result of [of_bools may_be_false may_be_false]:

     - May be false (i.e. "False" of "Top") if [may_be_false] is true,
     and must not be otherwise (i.e. "True" or "Bottom")

     - May be true (i.e. "True" of "Top") if [may_be_true] is true,
     and must not be otherwise (i.e. "False" or "Bottom") *)
val of_bools: may_be_false:bool -> may_be_true:bool -> boolean
val to_bools: boolean -> (bool * bool) (* may_be_false, may_be_true *)
val truth_value: boolean -> boolean
val convert_to_quadrivalent: boolean -> boolean

