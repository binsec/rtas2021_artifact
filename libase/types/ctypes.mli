type value =
  | Const of Z.t
  | Sym of string

val pp_value : Format.formatter -> value -> unit

module Pred : sig
  type unop =
  | Extract of int * int (** start index, length *)

  type binop =
  | Add | Sub | Mul | And
  | Concat of int * int (** Size 1, size 2. First argument is the most significant *)

  type expr =
    | Val of value
    | Self
    | Unop of unop * expr
    | Binop of binop * expr * expr

  type cmpop = Equal | NotEqual | ULt | SLt | ULeq | SLeq | UGeq | SGeq | UGt | SGt

  (** Predicate on a structure field or array element, which is supposed to be
     true at all times. *)
  type t =
    | True
    | Cmp of cmpop * expr * expr
    | And of t * t

  val compare : t -> t -> int
  val equal : t -> t -> bool
  val pp : Format.formatter -> t -> unit

  (** Helpers. *)
  val eq : value -> t
  val neq : value -> t
  val slt : value -> t
  val sleq : value -> t
  val sgeq : value -> t
  val sgt : value -> t
  val ult : value -> t
  val uleq : value -> t
  val ugeq : value -> t
  val ugt : value -> t

  exception Undefined_symbol of string

  (** Evaluate a predicate. May throw {! Undefined_symbol}. *)
  val eval : symbols:(Z.t Datatype_sig.StringMap.t) -> self:Z.t -> t -> bool
end

type basic = int * string
type enum = {
  en_name : string option;
  en_byte_size : int;
  en_values : (string * int) list;
}

type index =
  | ValidUnknown
  | Zero
  | PastEnd

val pp_index : Format.formatter -> index -> unit
val join_indices : index -> index -> index

type structure = {
  st_name : string option;
  st_byte_size : int option;
  st_members : (int * string * typ) list;
}

and descr =
  | Void
  | Base of basic
  | Structure of structure
  | Ptr of { pointed : typ; index : index }
  | Enum of enum
  | Array of typ * value option
      (** Arguments: element type, and the number of elements, if statically
         known. *)
  | Function of typ * typ list

and typ =
  { descr : descr;
    const : bool;
    pred : Pred.t;
  }

val pp_descr : Format.formatter -> descr -> unit
val pp : Format.formatter -> typ -> unit

val compare_descr : only_descr:bool -> descr -> descr -> int
val compare : only_descr:bool -> typ -> typ -> int

val equal_descr : only_descr:bool -> descr -> descr -> bool
val equal : only_descr:bool -> typ -> typ -> bool

(*
val pred_of : typ -> pred
val remove_pred : typ -> typ
*)

val pointer : typ -> bool

module type Type_settings = sig
  (** Root type of the parameters, and symbol to find it in the ELF file. *)
  val root : string * typ

  (** List of data addresses that should be considered to be an extension of
     parameter memory. *)
  val special_data_addrs : (int * (typ * int)) list

  (** List of every type [t] such that there exists only one array of [t]s in
      the parameters. *)
  val unique_array_types : typ list

  (** An attempt to store to a pointer to this type should raise an alarm. *)
  val non_modifiable_types : typ list

  (** List of global symbolic constants, with their sizes in bits and
     predicates that hold on them. *)
  val global_symbols : (string * int * Pred.t) list
end
