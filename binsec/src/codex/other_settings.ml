open Analysis_settings
module Ctypes = Codex.Ctypes
open Ctypes

module Logger = Codex_logger

module Make = functor
    (Types : Ctypes.Type_settings)
    (Reg : Arch_settings.Registers)
    (State : Dba2Codex.StateS)
    (Record_cfg : Record_cfg.S)
-> struct

  module State = State
  module Domain = State.Domain
  module EvalPred = State.EvalPred
  module Record_cfg = Record_cfg

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

  let unoption msg = function
  | Some x -> x
  | None -> raise (Failure msg)

  let get_sym name =
    Loader_utils.address_of_symbol ~name (Kernel_functions.get_img ())
    |> unoption ("Cannot find symbol " ^ name)

  let skip_table =
    let t = Addr_tbl.create 1 in
    let stop i ~msg =
      Addr_tbl.add t (Virtual_address.create i) (EndPath, msg) in
    let hook i ~msg f =
      Addr_tbl.add t (Virtual_address.create i) (Hook f, msg) in
    let skip_always i ~dest ~msg =
      Addr_tbl.add t (Virtual_address.create i) (SkipTo (Always, Virtual_address.create dest), msg) in
    let _unroll i max_iter ~msg =
      Addr_tbl.add t (Virtual_address.create i) (Unroll max_iter, msg) in
    let lift_int_sym f sym = f (get_sym sym) in
    let stop_at = lift_int_sym stop in
    let hook_at = lift_int_sym hook in
    let _skip_always_at sym ~destsym =
      lift_int_sym (fun dest -> lift_int_sym skip_always sym ~dest) destsym in
    t

  let kernel_exit_point = Virtual_address.create 0x0
  let exploration_only = ref false

end
