module type Address_sig = sig
  type t
  val create : int -> t

  module Set : Stdlib.Set.S with type elt = t
  module Htbl : Stdlib.Hashtbl.S with type key = t
end

module type RegionS = sig
  module Virtual_address : Address_sig
  include Memory_sig.Whole_Memory
  val written_data_addrs : Virtual_address.Set.t ref
  val read_data_addrs : Virtual_address.Set.t ref
  val set_untyped_load : bool -> unit
  val set_check_store_intvl : bool -> unit
  val set_param_typing : (Ctypes.typ * int) Virtual_address.Htbl.t -> unit
end

module type S = sig
  module BR : Memory_sig.Operable_Value_Whole
  module Type : Type_domain.S
    with module BR = BR
  module TS : Ctypes.Type_settings
  module Virtual_address : Address_sig

  type 'a t = private
  | Bottom
  | T of { typ : Type.t; value : 'a; }
      (** invariant: none of the components is bottom. *)

  type binary = BR.binary t

  module type Binary_Representation_S = Memory_sig.Operable_Value_Whole
    with module Scalar = BR.Scalar
    and type binary = binary

  module Binary_Representation : Binary_Representation_S

  include Binary_Representation_S with type binary := binary

  (** Returns [None] if the argument is bottom. *)
  val typ : size:int -> BR.Scalar.Context.t -> binary -> Type.t option

  val with_type : Type.t -> size:int -> BR.Scalar.Context.t -> binary -> binary
  val give_type : Type.t -> BR.binary -> binary

  val global_symbol : BR.Scalar.Context.t -> string -> int * binary

  val use_invariant : size:int -> BR.Scalar.Context.t -> Ctypes.Pred.t -> binary -> binary

  val check_invariant : size:int -> BR.Scalar.Context.t -> Ctypes.Pred.t -> binary -> bool

  module Region (DataReg : Memory_sig.Memory_Region
       with module Scalar = BR.Scalar
        (* The below restriction is necessary so that load(unknown) = unknown.
           It is acceptable because this domain is special-purpose. *)
        and type Value.binary = binary
        and type address = BR.binary
      ) : RegionS
        with module Scalar = BR.Scalar
        and module Value = DataReg.Value
        and type address = binary
        and module Virtual_address := Virtual_address
end

module type Needed_stuff = sig
  module Virtual_address : Address_sig
  val readable_interval : int -> Framac_ival.Ival.t
  val writable_interval : int -> Framac_ival.Ival.t
  val mmio_interval : int -> Framac_ival.Ival.t
  module Logger : Alarm_logger.S
  val option_use_shape : unit -> bool
end

module Make
    (BR : Memory_sig.Operable_Value_Whole)
    (Type : Type_domain.S with module BR = BR)
    (TS : Ctypes.Type_settings)
    (Stuff : Needed_stuff)
    : S
      with module BR = BR
      and module Type = Type
      and module TS = TS
      and module Virtual_address := Stuff.Virtual_address
