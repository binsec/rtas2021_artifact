let unoption msg = function
| Some x -> x
| None -> raise (Failure msg)

let get_sym name =
  Loader_utils.address_of_symbol ~name (Kernel_functions.get_img ())
  |> unoption ("Cannot find symbol " ^ name)

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
    | Hook of (Domain.Context.t -> Record_cfg.t -> State.t ->
        Record_cfg.t * (Virtual_address.t * State.t) list)
        (** Manually change the transfer function of an instruction. *)
    | ChangeState of (Domain.Context.t -> State.t -> State.t)
        (** Modifies the state before executing the corresponding instruction. *)
    | Unroll of int
        (** Unroll every loop having this address as its head. Argument: number
            of iterations. *)
    | EndPath
        (** End this trace *)

  let exploration_only = ref false

  (* Mock address used to trigger some hooks in the analyzer. *)
  let kernel_exit_point = Virtual_address.create 0xcafecaf0

  let handlers = ["asm_syscall_handler"]
    |> List.map (fun sym ->
      let addr = Loader_utils.address_of_symbol ~name:sym (Kernel_functions.get_img ())
        |> unoption ("Symbol "^sym^" not found") in
      Virtual_address.create addr
    )

  let bunknown ~size ctx =
    Domain.binary_unknown ~size ~level:(Domain.Context.level ctx) ctx

  let push32 ctx state value =
    let open Domain.Binary_Forward in
    let esp = State.get state "esp" in
    let esp = bisub ~size:32 ctx ~nsw:false ~nuw:false esp @@
      biconst ~size:32 (Z.of_int 4) ctx in
    let state = State.set ctx state "esp" esp in
    let effective_addr = biadd ~size:32 ctx ~nsw:false ~nuw:false (State.get state "ss_base") esp in
    {state with State.memory = Domain.Memory_Forward.store ~size:32 ctx state.State.memory
      effective_addr value}

  let load_seg_reg ctx state seg value =
    let open Domain.Binary_Forward in
    let gdt = State.get state "gdt" in
    let state = State.set ctx state seg value in
    if seg <> "cs" then
      assert (EvalPred.check_invariant ~size:16 ctx (Pred.neq (Const Z.zero)) value);
    let index = band ~size:16 ctx value @@ biconst ~size:16 (Z.of_int 0xfffc) ctx in
    let desc_addr = biadd ~size:32 ctx gdt ~nsw:false ~nuw:false @@ buext ~size:32 ~oldsize:16 ctx index in
    let desc = Domain.Memory_Forward.load ~size:64 ctx state.State.memory desc_addr in
    let low = bextract ctx ~size:24 ~index:16 ~oldsize:64 desc in
    let high = bextract ctx ~size:8 ~index:56 ~oldsize:64 desc in
    let new_base = bconcat ctx ~size1:8 ~size2:24 high low in
    State.set ctx state (seg^"_base") new_base

  let mk_flags ctx state =
    let open Domain.Binary_Forward in
    let bx () = bunknown ~size:1 ctx in
    let push_head (cursize, acc) (bsize, b) =
      cursize+bsize, bconcat ~size1:bsize ~size2:cursize ctx b acc in
    List.fold_left push_head (1, State.get state "CF") @@
    [ 1, bx ();                 (* 1 *)
      1, State.get state "PF";  (* 2 *)
      1, bx ();                 (* 3 *)
      1, State.get state "AF";  (* 4 *)
      1, bx ();                 (* 5 *)
      1, State.get state "ZF";  (* 6 *)
      1, State.get state "SF";  (* 7 *)
      1, State.get state "TF";  (* 8 *)
      1, State.get state "IF";  (* 9 *)
      1, State.get state "DF";  (* 10 *)
      1, State.get state "OF";  (* 11 *)
      2, State.get state "IOPL";(* 12, 13 *)
      1, State.get state "NT";  (* 14 *)
      1, bx ();                 (* 15 *)
      1, State.get state "RF";  (* 16 *)
      1, bx ();                 (* 17 *)
      1, State.get state "AC";  (* 18 *)
      1, State.get state "VIF"; (* 19 *)
      1, State.get state "VIP"; (* 20 *)
      1, State.get state "ID";  (* 21 *)
      10, bunknown ~size:10 ctx;(* 22-31 *)
    ]
    |> snd

  let scramble_regs ctx state =
    Reg.registers |> List.fold_left (fun state (name,size) ->
      match name with
      (* Privileged registers, cannot be changed by tasks as long as APE holds *)
      | "IF" | "IOPL" | "VIF" | "VIP" | "VM" | "cpl" | "tr" | "gdt" | "ds_base"
      | "cs_base" | "ss_base" | "es_base" | "fs_base" | "gs_base" | "tr_base"
      | "ds_desc" | "cs_desc" | "ss_desc" | "es_desc" | "fs_desc" | "gs_desc"
      | "tr_desc" | "idt" -> state
      (* In the setting of this system, segments cannot be changed by
       * unprivileged code as long as APE holds (not true for all systems). *)
      | "cs" | "ss" | "ds" | "es" | "fs" | "gs" -> state
      | _ ->
        State.set ctx state name @@ bunknown ~size ctx
    ) state

  let _end_of_kernel = Loader_utils.address_of_symbol ~name:"_end_of_kernel"
    (Kernel_functions.get_img ()) |> unoption "no symbol _end_of_kernel"

  let abstract_task ctx state =
    Logger.result "entering abstract_task";
    let cs_sel = State.get state "cs" in
    Logger.debug ~level:2 "Checking cs...";
    Logger.check "wrong_task_segment";
    let task_cs_sel_pred = Ctypes.Pred.(Cmp (Equal, Unop (Extract (0,2), Self),
      Val (Const (Z.of_int 3)))) in
    if not (EvalPred.check_invariant ~size:16 ctx task_cs_sel_pred cs_sel) then begin
      Logger.alarm "wrong_task_segment";
      Logger.error "code selector %a is not compliant" (Domain.binary_pretty ~size:16 ctx) cs_sel;
    end;
    (* actually not really necessary to check cs_desc as long as you trust the
     * architecture model *)
    let base e = Ctypes.Pred.(Binop (Concat (8,24), Unop (Extract (56, 8), e), Unop (Extract (16,24), e))) in
    (*let task_ds_desc_pred = Ctypes.Pred.(Cmp (UGeq, base Self, Val (Sym "end_of_kernel"))) in*)
    let task_ds_desc_pred = Ctypes.Pred.(Cmp (UGeq, Self, Val (Sym "end_of_kernel"))) in
    let s_idx e = Ctypes.Pred.(Unop (Extract (3, 13), e)) in
    let task_ds_sel_pred = Ctypes.Pred.(Cmp (Equal, s_idx Self, Val (Const (Z.of_int 4)))) in
    Logger.check "wrong_task_segment";
    let ds = State.get state "ds" in
    Logger.debug ~level:2 "Checking ds...";
    if not (EvalPred.check_invariant ~size:16 ctx task_ds_sel_pred ds) then begin
      Logger.alarm "wrong_task_segment";
      Logger.error "data segment selector %a is not compliant" (Domain.binary_pretty
        ~size:64 ctx) ds;
    end;
    let ds_desc = State.get state "ds_desc" in
    Logger.debug ~level:2 "Checking ds_desc...";
    let base = let open Domain.Binary_Forward in
      bconcat ~size1:8 ~size2:24 ctx
      (bextract ~size:8 ~index:56 ~oldsize:64 ctx ds_desc)
      (bextract ~size:24 ~index:16 ~oldsize:64 ctx ds_desc)
    in
    Logger.debug ~level:2 "extracted base: %a" (Domain.binary_pretty ~size:32 ctx) base;
    if not (EvalPred.check_invariant ~size:64 ctx task_ds_desc_pred base) then begin
      Logger.alarm "wrong_task_segment";
      Logger.error "data segment descriptor %a is not compliant" (Domain.binary_pretty
        ~size:64 ctx) ds_desc;
    end;
    scramble_regs ctx state

  let abstract_interrupt ctx state =
    Logger.result "entering abstract_interrupt";
    let open Domain.Binary_Forward in
    (* go fetch the TSS descriptor *)
    let old_esp = State.get state "esp" in
    let old_ss = State.get state "ss" in
    let old_cs = State.get state "cs" in
    let const i = biconst ~size:32 (Z.of_int i) ctx in
    let bit_zero = biconst ~size:1 Z.zero ctx in
    let tss_base = State.get state "tr_base" in
    Logger.debug ~level:2 "loading new_esp from the TSS (ofs 4)...";
    let new_esp = Domain.(Memory_Forward.load ~size:32 ctx state.State.memory @@
      biadd ~size:32 ~nuw:false ~nsw:false ctx tss_base (const 4)) in
    Logger.debug ~level:2 "loading new_ss from the TSS (ofs 8)...";
    let new_ss = Domain.(Memory_Forward.load ~size:16 ctx state.State.memory @@
      biadd ~size:32 ~nuw:false ~nsw:false ctx tss_base (const 8)) in
    Logger.debug ~level:2 "setting new esp...";
    let state = State.set ctx state "esp" new_esp in
    Logger.debug ~level:2 "loading new ss...";
    let state = load_seg_reg ctx state "ss" new_ss in
    (* FIXME: ??? *)
    let new_cs = biconst ~size:16 (Z.of_int64 (Int64.of_string "0b0001_0_00")) ctx in
    Logger.debug ~level:2 "loading new cs...";
    let state = load_seg_reg ctx state "cs" new_cs in
    Logger.debug ~level:2 "Pushing old_ss = %a" (Domain.binary_pretty ~size:16 ctx) old_ss;
    let state = push32 ctx state @@ buext ~size:32 ~oldsize:16 ctx old_ss in
    Logger.debug ~level:2 "Pushing old_esp = %a" (Domain.binary_pretty ~size:16 ctx) old_esp;
    let state = push32 ctx state old_esp in
    let flags = mk_flags ctx state in
    Logger.debug ~level:2 "Pushing cur. flags = %a" (Domain.binary_pretty ~size:16 ctx) flags;
    let state = push32 ctx state flags in
    Logger.debug ~level:2 "Pushing old_cs = %a" (Domain.binary_pretty ~size:16 ctx) old_cs;
    let state = push32 ctx state @@ buext ~size:32 ~oldsize:16 ctx old_cs in
    (* FIXME old eip: let's not care about its value... *)
    let old_eip =
      EvalPred.use_invariant ~size:32 ctx (Pred.neq (Const Z.zero)) @@
      bunknown ~size:32 ctx in
    Logger.debug ~level:2 "Pushing old_eip = %a" (Domain.binary_pretty ~size:16 ctx) old_eip;
    let state = push32 ctx state old_eip in
    Logger.debug ~level:2 "Changing CPL to 0...";
    let state = State.set ctx state "cpl" @@ biconst ~size:2 Z.zero ctx in
    (* FIXME should do CS(RPL) := CPL *)
    Logger.debug ~level:2 "Zeroing out flags IF, TF, VM, RF and NT...";
    let state = State.set ctx state "IF" bit_zero in
    let state = State.set ctx state "TF" bit_zero in
    let state = State.set ctx state "VM" bit_zero in
    let state = State.set ctx state "RF" bit_zero in
    let state = State.set ctx state "NT" bit_zero in
    Logger.debug ~level:2 "Jumping to handlers...";
    [(Virtual_address.create 0xcafecaf2, state)]

  let skip_table =
    let t = Addr_tbl.create 100 in
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

    stop_at "error_infinite_loop" ~msg:"Infinite error loop";
    stop 0xcafecaf0 ~msg:"Return from interrupt";
    stop_at "fatal" ~msg:"fatal";
    (*stop_at "syscall_putchar" ~msg:"cannot analyze putchar";*)

    hook 0xcafecafe ~msg:"second task-and-interrupt sequence" (fun ctx trace state ->
      trace, (abstract_task ctx state |> abstract_interrupt ctx));


    hook 0xcafecaf2 ~msg:"id" (fun ctx trace state ->
      trace, List.map (fun addr -> addr, state) handlers
    );
    hook 0xcafecaf1 ~msg:"first task-and-interrupt sequence" (fun ctx trace state ->
      trace, (abstract_task ctx state |> abstract_interrupt ctx)
    );

    t
end
