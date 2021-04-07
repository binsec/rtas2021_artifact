type skip_type = NotWhenInterpreting | Always

module Addr_tbl = Hashtbl.Make(struct
  type t = Virtual_address.t
  let equal x y = Virtual_address.compare x y = 0
  let hash = Virtual_address.to_int
end)

module type S = sig
  module State : Dba2Codex.StateS
  module Record_cfg : Record_cfg.S

  type skip =
    | SkipTo of skip_type * Virtual_address.t (** skip to address *)
    | Hook of (State.Domain.Context.t -> Record_cfg.t -> State.t ->
        Record_cfg.t * (Virtual_address.t * State.t) list)
        (** Manually change the transfer function of an instruction. *)
    | ChangeState of (State.Domain.Context.t -> State.t -> State.t)
        (** Modifies the state before executing the corresponding instruction. *)
    | Unroll of int
        (** Unroll every loop having this address as its head. Argument: number
            of iterations. *)
    | EndPath
        (** End this trace *)

  val skip_table : (skip * string) Addr_tbl.t

  (** Says whether the analysis should be "merge over all paths" only. Initially false. *)
  val exploration_only : bool ref

  val kernel_exit_point : Virtual_address.t
end

module type MAKE = functor (Reg : Arch_settings.Registers)
  (State : Dba2Codex.StateS)
  (Record_cfg : Record_cfg.S)
-> S with module State = State and module Record_cfg = Record_cfg
