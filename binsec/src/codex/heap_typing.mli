module StrMap = Codex.Datatype_sig.StringMap

type cell_typ = Codex.Ctypes.typ * int

exception Incomparable_types of string
exception Falsified_predicate of string

module type MEMORY = sig
  type t
  val read_u8 : t -> Virtual_address.t -> Loader_types.u8
  val read_u32 : t -> Virtual_address.t -> Loader_types.u32
  val read_u64 : t -> Virtual_address.t -> Loader_types.u64
end

module Make (Memory : MEMORY) : sig
  val type_heap : symbols:(Z.t StrMap.t) -> Memory.t
    -> Virtual_address.t -> Codex.Ctypes.typ -> cell_typ Virtual_address.Htbl.t
end
