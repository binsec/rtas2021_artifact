module VarMap : Map.S with type key = string

type jump_target =
  | Jump_Inner of Dba.id (** Some instruction in a block *)
  | Jump_Outer of Virtual_address.t (** Some instruction outside of the block. *)
  | Jump_Dynamic              (* TODO *)

(*
module Constraints : Codex.Constraints_constraints_sig.Constraints
  with module Condition = Codex.Constraints.Condition.ConditionCudd
module Propag_domain : Codex.Constraint_domains_sig.Domain_S
  with module Constraints = Constraints
*)

module type StateS = sig
  module Region_binary : Codex.Region_binary.S
    with module Virtual_address := Virtual_address
  module Domain : Codex.With_focusing.Base_with_types2
    with module Context = Region_binary.BR.Scalar.Context
    and module Type = Region_binary.Type
  module EvalPred : Codex.Type_domain.EvalPred.Sig
    with module Domain := Domain

  type t = {
    vars: Domain.binary VarMap.t;
    memory: Domain.memory;
    instruction_count: int;
    is_bottom: bool;
    never_went_to_user_code : bool;
  }


  val initial : (Loader_elf.Img.t, 'a, 'b) Loader.t_pack -> Domain.Context.t ->
                t
  val initial_concrete : (Loader_elf.Img.t, 'a, 'b) Loader.t_pack
    (*-> (Loader_elf.Img.t, 'a, 'b) Loader.t_pack*) -> Domain.Context.t -> t
  val reset : (Loader_elf.Img.t, 'a, 'b) Loader.t_pack -> Domain.Context.t ->
              t
  val get : t -> string -> Domain.binary
  val set : Domain.Context.t -> t -> string -> Domain.binary -> t
  val assume : Domain.Context.t -> Domain.boolean -> t -> t
  val assume_pred : Domain.Context.t -> size:int -> Codex.Ctypes.Pred.t
    -> Region_binary.BR.binary -> t -> t
  val bottom : Domain.Context.t -> t
  val dump_state : Domain.Context.t -> Format.formatter -> t -> unit
  val dump_state_diff : Domain.Context.t -> Format.formatter -> t -> t -> unit
  val join : Domain.Context.t -> t -> t -> t
  val is_included : Domain.Context.t -> t -> t -> bool

  (** Serialize a state's variables and memory into a tuple. [to_tuple ctx
      state] returns the result of serialization, along with an inversion
      function, to turn a tuple back into a state. This function takes the
      original state and a tuple as arguments, and update the state's [vars]
      and [memory] fields with the tuple's contents. *)
  val serialize: Domain.Context.t ->
    t -> t -> 'a Domain.in_acc -> (t, 'a) Domain.result
end

module type Sig = sig
  module Scalar : Codex.Domain_sig.Base

  module Region_numeric : module type of Codex.Region_numeric_offset.Make(Scalar)
  module Numeric : Codex.Memory_sig.Operable_Value_Whole
    with module Scalar = Scalar
    and type binary = Region_numeric.Operable_Value.binary

  module TSettingC : Codex.Ctypes.Type_settings
  module Type : Codex.Type_domain.S
    with module BR = Numeric
  module Region_binary_inst : Codex.Region_binary.S
    with module BR = Numeric
    and module Type = Type
    and type Virtual_address.t = Virtual_address.t
    and module Virtual_address := Virtual_address
  module Region_binary_Memory : Codex.Region_binary.RegionS
    with module Virtual_address := Virtual_address

  (*module Region_binary_Operable_Value = Region_binary_inst.Binary_Representation*)
  (*
  module Region_numeric_Memory : Codex.Memory_sig.Memory_Region
    with module Scalar = Scalar
    and module Value = Region_binary_inst.Binary_Representation
    and type address = Region_numeric.Operable_Value.binary
    (*
    module type of
    Region_numeric.Memory(Region_numeric.Operable_Value)(Region_binary_Operable_Value)
    *)

  module Region_binary_Memory : Codex.Memory_sig.Whole_Memory
    with module Scalar = Scalar
    and type Value.binary = Region_binary_inst.binary
    and type address = Region_binary_inst.BR.binary
    (*module type of Region_binary_inst.Region(Region_numeric_Memory)*)
  *)
  module Domain : Codex.With_focusing.Base_with_types2
    with module Context = Region_binary_inst.BR.Scalar.Context
    and module Type = Type

  module EvalPred : Codex.Type_domain.EvalPred.Sig
    with module Domain := Domain

  module Make (Reg : Arch_settings.Registers with module Domain = Domain) : sig

    module State : StateS
      with module Region_binary = Region_binary_inst
      and module Domain = Domain

    val expr : Domain.Context.t -> Dba.Expr.t -> State.t -> (Domain.binary * State.t) list
    val instr : Domain.Context.t -> State.t -> Dba.Instr.t -> (jump_target * State.t) list

  end
end

module Create : functor () -> Sig
