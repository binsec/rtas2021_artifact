module type S = sig
  module BR : Memory_sig.Operable_Value_Whole

  type t =
    | Top (** Could be anything: pointer or integer. *)
    | ParamT of Ctypes.typ * BR.binary * BR.binary * int
        (** Pointer to parameter memory. Constructor arguments:
            - type. {b Invariant}: the outermost constructor should always be
              {!const:Ctypes.Ptr}.
            - index in array (0 if irrelevant).
            - index from end of array (0 if irrelevant).
            - offset in structure.
        *)
    | ZeroT
    | Bottom

  (** Size in bits of the array index argument to {ParamT}. *)
  val array_index_size : int

  val index_zero : BR.Scalar.Context.t -> BR.binary
  val index_minus_one : BR.Scalar.Context.t -> BR.binary

  (** Size in bits of the struct offset argument to {ParamT}. *)
  val offset_size : int

  val eq : only_descr:bool -> BR.Scalar.Context.t -> t -> t -> bool

  (** A more imprecise equality between {!t}s that does not require a
     context. The difference is in the comparison between the array indices
     (of type {!BR.binary}). The only guarantee on this function is that if
     [t] and [u] are not equal, then [imprecise_eq t u] is [false]. *)
  val imprecise_eq : t -> t -> bool

  (** Size of a type in bytes *)
  val type_size : Ctypes.descr -> int

  exception Global_symbol_not_found

  (** Retrieve the abstract numeric value corresponding to a global symbolic
     constant (along with its size in bits). May throw {!Global_symbol_not_found}. *)
  val symbol : BR.Scalar.Context.t -> string -> int * BR.binary

  (** Utility functions to manipulate predicates over abstract values of the
     numeric domain. These functions are not really about types but it was
     convenient to put them here. *)

  val cond_of_pred_subdomain : size:int -> BR.Scalar.Context.t -> Ctypes.Pred.t
    -> BR.binary -> BR.Scalar.boolean
  val check_invariant_subdomain : size:int -> BR.Scalar.Context.t -> Ctypes.Pred.t
    -> BR.binary -> bool
  val use_invariant_subdomain : size:int -> BR.Scalar.Context.t -> Ctypes.Pred.t
    -> BR.binary -> BR.binary

  (** Returns the same type with the condition assumed true on components of
     the index. Note: assuming a boolean true has an effect on a "binary" only
     if the condition concerns said binary. *)
  val assume : BR.Scalar.Context.t -> BR.Scalar.boolean -> t -> t

  (** Required by the domain signature, but don't expect this order to make
     any sense. WARNING: this function is NOT the lattice comparison
     function! *)
  val compare : t -> t -> int
  val pp : BR.Scalar.Context.t -> Format.formatter -> t -> unit

  exception Type_error of string

  val serialize : BR.Scalar.Context.t -> t -> t -> 'a BR.Scalar.in_acc -> (t, 'a) BR.Scalar.result
  val join : BR.Scalar.Context.t -> t -> t -> t

  val pointer : t -> bool

  (** Returns the type obtained by dereferencing the argument. May throw a
      {Type_error}. *)
  val deref : BR.Scalar.Context.t -> t -> t * Ctypes.Pred.t

  (** Lattice comparison function. *)
  val subtype : BR.Scalar.Context.t -> t -> t -> bool

  (** Non-standard but needed operator. See notes. Returns true iff one of
      the concrete types in the concretization of the first operand is a
      subtype of one of the concrete types in the concretization of the first
      operand. *)
  val may_subtype : BR.Scalar.Context.t -> t -> t -> bool

  (** Takes a triple (type, index, offset) and an offset to add to an adress
      of that type, and returns a triple (type', index', offset') where
      (type',index',offset') is the resulting type triple. *)
  val add_offset : BR.Scalar.Context.t -> Ctypes.typ -> BR.binary
    -> BR.binary -> int -> BR.binary
    -> t
end

module Make : functor (BR : Memory_sig.Operable_Value_Whole)
    (TS : Ctypes.Type_settings)
    (Logger : Alarm_logger.S)
-> S with module BR = BR

module EvalPred : sig

  module type Arg = sig
    include Domain_sig.Base
    val symbol : Context.t -> string -> binary
  end

  module type Sig = sig
    module Domain : Domain_sig.Base
    val check_invariant : size:int -> Domain.Context.t -> Ctypes.Pred.t
      -> Domain.binary -> bool
    val use_invariant : size:int -> Domain.Context.t -> Ctypes.Pred.t
      -> Domain.binary -> Domain.binary
  end

  module Make : functor (A : Arg) ->
    Sig with module Domain := A

end
