module type Base_with_types = sig
  include Domain_sig.Base
  module Type : Type_domain.S
    with module BR.Scalar.Context = Context

  (** Returns [None] if the argument is bottom. *)
  val typ : size:int -> Context.t -> binary -> Type.t option

  val with_type : Type.t -> size:int -> Context.t -> binary -> binary

  (** Get a global symbol from its name in the definition of the interface
      types. *)
  val global_symbol : Context.t -> string -> int * binary
end

(** Same signature as above, but the subdomain also has types. *)
module type Base_with_types2 = sig
  include Base_with_types
end

module type S = sig
  include Domain_sig.Base

  (** Exception thrown by {!Memory_Forward.load} when the load fails because
      the pointed region needs to be focused first. Contains the load result, as
      well as the new memory with the focusing performed. *)
  exception Needs_focusing of binary * memory
end

module type S_with_types2 = sig
  include Base_with_types2
  exception Needs_focusing of binary * memory
end

module Make (D : Domain_sig.Base) (L : Alarm_logger.S) : S
  with module Context = D.Context
  and type boolean = D.boolean

module Make_with_types (D : Base_with_types) (L : Alarm_logger.S) : S_with_types2
  with module Context = D.Context
  and type boolean = D.boolean
  and module Type = D.Type
