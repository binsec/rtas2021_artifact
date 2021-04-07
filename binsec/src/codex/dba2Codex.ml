module VarMap = Codex.Extstdlib.Map.Make(String)

type jump_target =
  | Jump_Inner of Dba.id (** Some instruction in a block *)
  | Jump_Outer of Virtual_address.t (** Some instruction outside of the block. *)
  | Jump_Dynamic              (* TODO *)

open Codex

module Logger = Codex_logger

module type StateS = sig
  module Region_binary : Region_binary.S
    with module Virtual_address := Virtual_address
  module Domain : With_focusing.Base_with_types2
    with module Context = Region_binary.BR.Scalar.Context
    and module Type = Region_binary.Type
  module EvalPred : Type_domain.EvalPred.Sig
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
  val assume_pred : Domain.Context.t -> size:int -> Ctypes.Pred.t
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

  module TSettingC : Ctypes.Type_settings
  module Type : Type_domain.S
    with module BR = Numeric
  module Region_binary_inst : Region_binary.S
    with module BR = Numeric
    and module Type = Type
    and module Virtual_address := Virtual_address
  module Region_binary_Memory : Region_binary.RegionS
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
  module Domain : With_focusing.Base_with_types2
    with module Context = Region_binary_inst.BR.Scalar.Context
    and module Type = Type
  module EvalPred : Type_domain.EvalPred.Sig
    with module Domain := Domain

  module Make (Reg : Arch_settings.Registers with module Domain = Domain) : sig

    module State : StateS
      with module Region_binary = Region_binary_inst
      and module Domain = Domain

    val expr : Domain.Context.t -> Dba.Expr.t -> State.t -> (Domain.binary * State.t) list
    val instr : Domain.Context.t -> State.t -> Dba.Instr.t -> (jump_target * State.t) list

  end
end

module Create () : Sig = struct

  module Constraints = Constraints.Constraints.MakeConstraints
    (Constraints.Condition.ConditionCudd)
  module Propag_domain = Domains_constraints_constraint_propagation.Make
    (Constraints)(Ival_basis)
  module Scalar0 = Constraint_domain2.Make(Constraints)(Propag_domain)
  module Scalar = Bitwise_domain.Make (Scalar0)

  module Region_numeric = Region_numeric_offset.Make(Scalar)
  module Numeric : Codex.Memory_sig.Operable_Value_Whole
    with module Scalar = Scalar
    and type binary = Region_numeric.Operable_Value.binary
  = Region_numeric.Operable_Value

  module Stuff_for_RB = struct
    let unoption msg = function
    | Some x -> x
    | None -> raise (Failure msg)

    let get_sym name =
      Loader_utils.address_of_symbol ~name (Kernel_functions.get_img ())
      |> unoption ("Cannot find symbol "^name)

    let readable_interval =
      let open Framac_ival in
      (*
      Ival.inject_range (Some (Z.of_int 0x12000000))
        (Some (Z.of_int (0x1202e000 - (data_size / 8))))
      *)
      if Codex_options.AnalyzeKernel.get () then
        let l = get_sym "_begin_of_all" in
        (*let h = get_sym "_end_of_fixed_region" in*)
        let  h = get_sym "_end_of_kernel" in (* if parameters instantiated *)
        fun data_size -> Ival.inject_range (Some (Z.of_int l))
          (Some (Z.of_int (h - (data_size / 8))))
      else fun _ -> Ival.top

    let writable_interval =
      let open Framac_ival in
      (*
      Ival.inject_range (Some (Z.of_int 0x1200d000))
        (Some (Z.of_int (0x1202e000 - (data_size / 8))))
      *)
      if Codex_options.AnalyzeKernel.get () then
        let l = get_sym "_end_of_readonly" in
        (*let h = get_sym "_end_of_fixed_region" in*)
        let  h = get_sym "_end_of_kernel" in (* if parameters instantiated *)
        fun data_size -> Ival.inject_range (Some (Z.of_int l))
          (Some (Z.of_int (h - (data_size / 8))))
      else fun _ -> Ival.top

    let mmio_interval data_size =
      let open Framac_ival in
      (*
      Ival.inject_range (Some (Z.of_int 0x00a00100))
        (Some (Z.of_int (0x021bc434 - (data_size / 8))))
      *)
      if Codex_options.AnalyzeKernel.get () then
        (* For the VGA buffer. *)
        Ival.inject_range (Some (Z.of_int 0xb8000))
          (Some (Z.of_int (0xb8fa0 - data_size / 8)))
      else Ival.bottom

    let option_use_shape () =
      Codex_options.UseShape.get ()

    module Logger = Logger

    module Virtual_address = Virtual_address
  end

  module TSettingC = TSetting.Create ()
  module Type = Type_domain.Make(Numeric)(TSettingC)(Logger)
  module Region_binary_inst : Region_binary.S
    with module BR = Numeric
    and module Type = Type
    and module Virtual_address := Virtual_address
    = Region_binary.Make(Numeric)(Type)(TSettingC)(Stuff_for_RB)
  (*
  module Region_binary_Operable_Value : Codex.Memory_sig.Value
    with module Scalar = Scalar
    = Region_binary_inst.Binary_Representation
  *)
  module Region_numeric_Memory =
    Region_numeric.Memory(Region_numeric.Operable_Value)(Region_binary_inst.Binary_Representation)

  module Region_binary_Memory = struct
    include Region_binary_inst.Region(Region_numeric_Memory)
  end

  module Domain = struct
    include Memory_domain.Make(Scalar)(Region_binary_inst.Binary_Representation)(Region_binary_Memory)
    module Type = Type
    let typ = Region_binary_inst.typ
    let with_type = Region_binary_inst.with_type
    let global_symbol = Region_binary_inst.global_symbol
    exception Needs_focusing of binary * memory
  end

  (*
  module Domain =
    With_focusing.Make_with_types (Domain0) (Logger)
  *)

    (*
  module Domain =
    Bitwise_domain.Make_with_types(Domain1)
    *)

  module EvalPred = Type_domain.EvalPred.Make(struct
    include Domain
    let symbol ctx name =
      snd @@ Domain.global_symbol ctx name
  end)

  module Ival = Codex.Framac_ival.Ival

  module type Registers = sig
    val registers : (string * int) list
    val initial_value : Domain.Context.t -> string * int -> Domain.binary
  end

  let bunknown ~size ctx =
    Domain.binary_unknown ~size ~level:(Domain.Context.level ctx) ctx

  module Make (Reg : Registers) = struct

    module State = struct

      module Region_binary = Region_binary_inst
      module Domain = Domain
      module EvalPred = EvalPred

      type t = {
        vars: Domain.binary VarMap.t;
        memory: Domain.memory;
        instruction_count: int;
        is_bottom: bool;
        never_went_to_user_code : bool;
      }

      let read_u32 (mem : Loader_elf.Img.t) (offset : int) : Loader_types.u32 =
        let open Loader_elf in
        let b0 = read_offset mem offset in
        let b1 = read_offset mem (offset+1) in
        let b2 = read_offset mem (offset+2) in
        let b3 = read_offset mem (offset+3) in
        (b3 lsl 24) lor (b2 lsl 16) lor (b1 lsl 8) lor b0

      let load_image ptrsize img ctx memory =
        let program_headers = Loader_elf.program_headers img in
        let memory = Array.fold_left (fun mem phdr ->
            let open Loader_elf in
            let vaddr = phdr.Phdr.vaddr in
            let filesize = phdr.Phdr.filesz in
            let filesize_mul_4 = filesize - (filesize mod 4) in
            let memsize = phdr.Phdr.memsz in
            Logger.result "Region: address %x filesize %x filesize_mul_4 %x memsize %x" vaddr filesize filesize_mul_4 memsize;
            if memsize = 0 then mem else
              let initial = Domain.Binary_Forward.biconst ~size:(memsize * 8) Z.zero ctx in
              (* XXX: to test syscalls individually, and quickly. *)
              (*
              let initial =
                Region_binary_Operable_Value.param_unknown ~size:(memsize * 8) (Domain.get_subcontext ctx) in
              *)
              let address = Domain.Binary_Forward.biconst ~size:ptrsize (Z.of_int vaddr) ctx in
              let mem = Domain.Memory_Forward.store ~size:(memsize * 8) ctx mem address initial in
              (* Logger.result "Memory %a"  (Domain.memory_pretty ctx) mem; *)
              let rec loop i mem =
                Logger.result "loop %x" i;
                (* TODO: lots of time, the loaded value is zero. We just
                   accumulate the zeroes, and write them as a big chunk,
                   using an inner loop. *)
                let rec inner_loop i idx size mem =
                  Logger.result "inner_loop %x %x %x" i idx size;
                  let u32 =
                    try read_u32 img (phdr.Phdr.offset + i)
                    with Invalid_argument _ -> 0 in
                  if u32 = 0 && i < filesize_mul_4
                  then inner_loop (i + 4) idx (size + 32) mem
                  else
                  if size == 0 then (i,u32,mem)
                  else
                    let address = Domain.Binary_Forward.biconst ~size:ptrsize (Z.of_int @@ vaddr + idx) ctx in
                    let zero = Domain.Binary_Forward.biconst ~size Z.zero ctx in
                    let mem = Domain.Memory_Forward.store ~size ctx mem address zero in
                    (i,u32,mem)
                in
                let (i,u32,mem) = inner_loop i i 0 mem in
                if i = filesize_mul_4 then mem
                else
                  (* let char = Loader_elf.read_offset img (phdr.p_offset + i) in *)
                  (* Logger.result "Char is %d %x" i char; *)
                  ((* Logger.result "Writing %x at address %x" char (vaddr + i); *)
                  let value = Domain.Binary_Forward.biconst ~size:32 (Z.of_int u32) ctx in
                  let address =
                    Domain.Binary_Forward.biconst ~size:ptrsize
                      (Z.of_int @@ vaddr + i) ctx in
                  let mem = Domain.Memory_Forward.store ~size:32 ctx mem address value in
                  loop (i + 4) mem)
              in
              let mem = loop 0 mem in
              let rec loop_rem i mem =
                Logger.result "loop_rem %x" i;
                if i = filesize then mem
                else
                  let value = Domain.Binary_Forward.biconst ~size:8 (Z.of_int @@ Loader_elf.read_offset img i) ctx in
                  let address = Domain.Binary_Forward.biconst ~size:ptrsize (Z.of_int @@ vaddr + i) ctx in
                  let mem = Domain.Memory_Forward.store ~size:8 ctx mem address value in
                  loop_rem (i+1) mem
              in
              loop_rem filesize_mul_4 mem

           (* XXX: Ecrire aussi le contenu de la memoire initiale. *)
          ) memory program_headers
        in
        memory

      let add_param_bridge_section root_type_addr root_type ctx memory =
        Logger.result "Populating bridge section...";
        let ptrsize = Codex_config.ptr_size () in
        let runtime = match root_type with
          Ctypes.({descr=Structure s;_}) -> s | _ -> assert false in
        let bridge_base_addr =
          Domain.Binary_Forward.biconst (Z.of_int (Virtual_address.to_int root_type_addr)) ctx ~size:ptrsize in
        runtime.Ctypes.st_members
          |> List.fold_left (fun acc_memory (offset,_,ftyp) ->
              Logger.result "Storing %a" Ctypes.pp ftyp;
              let zero = Numeric.Binary_Forward.biconst ~size:Type.array_index_size
                Z.zero ctx in
              let minus_one = Z.sub Z.zero Z.one in
              let minus_one = Numeric.Binary_Forward.biconst ~size:Type.array_index_size
                minus_one ctx in
              let ftyp_val,_ =
                (* We use [Type.deref] in order to focus the index if necessary
                 * (e.g. needed for the "mk" field, which is an array). *)
                Type.(Ctypes.(
                  deref ctx (ParamT ({descr = Ptr{pointed=ftyp;index=Zero};
                    pred = Pred.True; const = false}, zero, minus_one, 0))))
              in
              let size = ptrsize in (* Is ok because runtime contains only
                                       pointers and an {uint32_t}. *)
              let addr =
                Domain.Binary_Forward.biadd ~size ~nsw:false ~nuw:false ctx
                bridge_base_addr (Domain.Binary_Forward.biconst ~size:32
                (Z.of_int offset) ctx) in
              let unknown = bunknown ~size ctx in
              let value = Domain.with_type ftyp_val ~size ctx unknown in
              let value = EvalPred.use_invariant ~size ctx ftyp.Ctypes.pred value in
              Domain.Memory_Forward.store ~size ctx acc_memory addr value
          ) memory

      let _add_concrete_bridge_section ctx img memory =
        Logger.result "Populating bridge section...";
        let ptrsize = Codex_config.ptr_size () in
        load_image ptrsize img ctx memory

      let mk_initial_memory load_img add_bridge_section ctx =
        let memory = Domain.Memory_Forward.unknown ~level:0 ctx in
        let ptrsize = Codex.Codex_config.ptr_size() in
        Region_binary_Memory.set_check_store_intvl false;
        let memory = load_img ctx memory in
        let memory = add_bridge_section ctx memory in

        let memory =
          if Codex_options.AnalyzeKernel.get () then
            (* Add sections of MMIOs. *)
            (* This one is used in board_interrupt_handler. *)
            Domain.Memory_Forward.store ~size:(0x1000 * 8) ctx memory
                (Domain.Binary_Forward.biconst (Z.of_int 0xa00100) ctx ~size:ptrsize )
                (bunknown ctx ~size:(0x1000 * 8))
          else memory
        in
        Logger.result "*** End of memory initialization. ***";
        Region_binary_Memory.set_check_store_intvl true;
        memory

      let initial_memory img ctx =
        let ptrsize = Codex_config.ptr_size () in
        let img = match img with
          | Loader.PE _ | Loader.Dump _ -> assert false
          | Loader.ELF img -> img in
        let load_img = load_image ptrsize img in
        let root_type_sym, root_type = TSettingC.root in
        let root_type_addr = match Loader_utils.address_of_symbol ~name:root_type_sym
          (Loader.ELF img) with None -> assert false | Some a -> Virtual_address.create a in
        let add_bridge_section =
          Logger.debug ~level:2 "use-shape = %B" @@ Codex_options.UseShape.get ();
          if Codex_options.UseShape.get () then
            add_param_bridge_section root_type_addr root_type
          else fun _ m -> m
        in
        mk_initial_memory load_img add_bridge_section ctx

      let concrete_initial_memory img_kernel (*img_app*) ctx =
        let ptrsize = Codex_config.ptr_size () in
        let img_kernel = match img_kernel with
          | Loader.PE _ | Loader.Dump _ -> assert false
          | Loader.ELF img -> img in
        (*
        let img_app = match img_app with
          | Loader.PE _ | Loader.Dump _ -> assert false
          | Loader.ELF img -> img in
        *)
        let load_img = load_image ptrsize img_kernel in
        let add_bridge_section _ctx mem =
          (*add_concrete_bridge_section ctx img_app mem in*)
          mem in
        mk_initial_memory load_img add_bridge_section ctx

      let initial_vars ctx =
        List.fold_left (fun acc (name,size) ->
            let value = Reg.initial_value ctx (name,size) in
            (* Logger.result "creating reg %s of size %d" name size; *)
            VarMap.add name value acc
          ) VarMap.empty Reg.registers
      
      let initial img ctx =
        (* initialize symbol table. *)
        { vars = initial_vars ctx;
          memory = initial_memory img ctx;
          instruction_count = 0;
          is_bottom = false;
          never_went_to_user_code = true;
        }

      let initial_concrete img_kernel (*img_app*) ctx =
        { vars = initial_vars ctx;
          memory = concrete_initial_memory img_kernel (*img_app*) ctx;
          instruction_count = 0;
          is_bottom = false;
          never_went_to_user_code = true;
        }

      let empty_vars ctx =
        List.fold_left (fun acc (name,size) ->
            VarMap.add name (Domain.binary_empty ~size ctx) acc)
          VarMap.empty Reg.registers

      let reset img ctx =
        { vars = initial_vars ctx;
          memory = initial_memory img ctx;
          instruction_count = 0;
          is_bottom = false;
          never_went_to_user_code = true;
        }

      let is_singleton ctx ~size value =
        value
        |> Domain.Query.binary ~size ctx
        |> Domain.Query.binary_to_ival ~signed:true ~size
        |> fun x -> Ival.is_singleton_int x

      
      let get state reg =
        try VarMap.find reg state.vars
        with Not_found ->  failwith ("Could not find " ^ reg)

      let set state reg value =
        if not (VarMap.mem reg state.vars) then failwith ("Setting " ^ reg);
        {state with vars = VarMap.add reg value state.vars}

      let set ctx state reg value =
        Logger.debug ~level:3 "attempt to set %s" reg;
        let size = List.assoc reg Reg.registers in
        Logger.debug ~level:2 "Setting %s to %a" reg (Domain.binary_pretty ~size ctx) value;
        if not state.is_bottom then
          if reg = "sp" then assert(is_singleton ~size:32 ctx value);
          set state reg value

      let dump_state ctx fmt state =
        if state.is_bottom then Format.fprintf fmt "Bottom" else
          Reg.registers |> List.iter (fun (reg,size) ->
              let value = VarMap.find reg state.vars in
              Format.fprintf fmt "%s -> %a@\n" reg (Domain.binary_pretty ~size ctx) value
            )

      let dump_state_diff ctx fmt oldstate state =
        Reg.registers |> List.iter (fun (reg,size) ->
            let value = VarMap.find reg state.vars in
            let oldvalue = VarMap.find reg oldstate.vars in
            if not (Domain.Binary.equal value oldvalue)
            then
              Format.fprintf fmt "%s -> %a@\n" reg (Domain.binary_pretty ~size ctx) value (* (Domain.binary_pretty ~size ctx) value *)
          )

      
      let assume ctx cond state =
        if state.is_bottom then state else
          let vars = VarMap.mapi (fun reg (v:Domain.binary) -> 
              let size = List.assoc reg Reg.registers in
              (*Logger.result "%s before:@ %a" reg (Domain.binary_pretty ~size ctx) v;*)
              let res = Domain.assume_binary ~size ctx cond v in
              (*Logger.result "%s after:@ %a" reg (Domain.binary_pretty ~size ctx) res;*)
              res) state.vars in
          let memory = Domain.assume_memory ctx cond state.memory in
          {state with vars; memory}

      let assume_pred ctx ~size pred v state =
        let cond = Type.cond_of_pred_subdomain ~size ctx pred v in
        assume ctx cond state

      let serialize ctx a b (included,acc) =
        if a.is_bottom then
          Domain.Result (b.is_bottom, acc, fun out -> b, out)
        else if b.is_bottom then
          Domain.Result (included, acc, fun out -> a, out)
        else
          let Domain.Result(included,acc,fvar) = VarMap.fold_on_diff2 a.vars b.vars
              (Domain.Result(included,acc,fun out -> a.vars,out))
              (fun var a b (Domain.Result(inc,acc,k)) ->
                 match a,b with
                 | Some a, Some b when a == b -> Domain.Result(inc,acc,k)
                 | Some a, Some b ->
                   let size = List.assoc var Reg.registers in
                   Logger.result "### register %s:@ a =@ %a,@ b =@ %a" var
                    (Domain.binary_pretty ~size ctx) a (Domain.binary_pretty ~size ctx) b;
                   let Domain.Result(inc,acc,d) = Domain.serialize_binary ~size ctx a b
                    (inc,acc) in
                   Domain.Result(inc, acc, fun out ->
                       let v,out = d out in                   
                       let map,out = k out in
                       (VarMap.add var v map, out))
                 | _,_ -> assert false (* Variables should be defined in both cases *)
              ) in
          (*
          Logger.debug ~level:3 "memory a before serialize:@ %a" (Domain.memory_pretty ctx) a.memory;
          Logger.debug ~level:3 "memory b before serialize:@ %a" (Domain.memory_pretty ctx) b.memory;
          *)
          let Domain.Result(included,acc,fmem) = Domain.serialize_memory ctx a.memory b.memory
            (included,acc) in
          Domain.Result(included, acc, fun out ->
              (* TODO: Remove ctx and instruction_count from state. *)
              let memory,out = fmem out in
              let vars,out = fvar out in
              (*Logger.debug ~level:3 "memory after serialize:@ %a" (Domain.memory_pretty ctx) memory;*)
              { memory;vars;
                instruction_count = max a.instruction_count b.instruction_count;
                is_bottom = false;
                never_went_to_user_code =
                  a.never_went_to_user_code && b.never_went_to_user_code;
              }, out)
        ;;

      let bottom ctx =
        assume ctx (Domain.Boolean_Forward.false_ ctx)
          { instruction_count = 0;
            vars = empty_vars ctx;
            memory = Domain.Memory_Forward.unknown ~level:0 ctx;
            is_bottom = true;
            never_went_to_user_code = true;
          }

      let join ctx a b =
        if a.is_bottom then b else if b.is_bottom then a else
          let Domain.Result(_,acc,f) = serialize ctx a b
            (true, Domain.empty_tuple) in
          let out = Domain.typed_nondet2 ctx acc in
          let r,_ = f out in
          r

      let is_included ctx a b =
        if a.is_bottom then true else if b.is_bottom then false else
          let Domain.Result(included,_,_) = serialize ctx b a (true, Domain.empty_tuple) in
          included
    end


    let z_of_bitvector x = Z.of_string @@ Bitvector.to_hexstring x
    let bitvector_of_z x =
      let string = Z.format "#x" x in
      Bitvector.of_hexstring string

    let bin_of_bool ctx cond =
      Domain.Binary_Forward.bofbool ~size:1 ctx cond

    let bool_of_bin ctx bin =
      let zero = Domain.Binary_Forward.biconst ~size:1 Z.zero ctx in
      let condelse = Domain.Binary_Forward.beq ~size:1 ctx bin zero in
      let condthen = Domain.Boolean_Forward.not ctx condelse in
      condthen


    module ListM : sig
      val (>>=) : 'a list -> ('a -> 'b list) -> 'b list
    end = struct
      let (>>=) x f =
        List.concat (List.map f x)
    end

    let binop ctx ~size bop v1 v2 state =
      let open Dba in
      let open Binary_op in
      let f = match bop with
        | Plus -> fun ~size:_ -> assert false
        | Minus -> fun ~size:_ -> assert false
        | Mult -> Domain.Binary_Forward.bimul ~nsw:false ~nuw:false
        | DivU -> assert false
        | DivS -> Domain.Binary_Forward.bisdiv
        | ModU -> assert false
        | ModS -> Domain.Binary_Forward.bismod
        | Or -> Domain.Binary_Forward.bor
        | And -> Domain.Binary_Forward.band
        | Xor -> Domain.Binary_Forward.bxor
        | Concat -> assert false
        | LShift -> Domain.Binary_Forward.bshl ~nsw:false ~nuw:false
        | RShiftU -> Domain.Binary_Forward.blshr
        | RShiftS -> Domain.Binary_Forward.bashr
        | LeftRotate -> assert false
        | RightRotate -> fun ~size ctx _ _ ->
          (* FIXME: right rotation is fully imprecise *)
          bunknown ~size ctx
        | Eq -> fun ~size ctx v1 v2 ->
          bin_of_bool ctx @@ Domain.Binary_Forward.beq ~size ctx v1 v2
        | Diff -> fun ~size ctx v1 v2 ->
          bin_of_bool ctx @@ Domain.Boolean_Forward.not ctx @@ Domain.Binary_Forward.beq ~size ctx v1 v2
        | LeqU ->
          fun ~size ctx v1 v2 ->
            bin_of_bool ctx @@ Domain.Binary_Forward.biule ~size ctx v1 v2
        | LtU ->
          fun ~size ctx v1 v2 ->
            let cond = Domain.Binary_Forward.biule ~size ctx v2 v1 in
            let cond = Domain.Boolean_Forward.not ctx cond in
            bin_of_bool ctx cond
        | GeqU ->
          fun ~size ctx v1 v2 ->
            let cond = Domain.Binary_Forward.biule ~size ctx v2 v1 in
            bin_of_bool ctx cond
        | GtU ->
          fun ~size ctx v1 v2 ->
            let cond = Domain.Binary_Forward.biule ~size ctx v1 v2 in
            let cond = Domain.Boolean_Forward.not ctx cond in
          bin_of_bool ctx cond
        | LeqS ->
          fun ~size ctx v1 v2 ->
            let cond = Domain.Binary_Forward.bisle ~size ctx v1 v2 in
            bin_of_bool ctx cond
        | LtS ->
          fun ~size ctx v1 v2 ->
            let cond = Domain.Binary_Forward.bisle ~size ctx v2 v1 in
            let cond = Domain.Boolean_Forward.not ctx cond in
            bin_of_bool ctx cond
        | GeqS -> assert false
        | GtS -> fun ~size ctx v1 v2 ->
            let cond = Domain.Binary_Forward.bisle ~size ctx v1 v2 in
            let cond = Domain.Boolean_Forward.not ctx cond in
            bin_of_bool ctx cond
      in
      match bop with
      | Plus ->
          let c = Domain.Binary_Forward.valid_ptr_arith ~size ctx v1 v2 in
          Logger.check "ptr_arith";
          begin match Domain.Query.boolean ctx c |> Domain.Query.convert_to_quadrivalent with
          | Quadrivalent_Lattice.(True | Bottom) ->
              Domain.Binary_Forward.biadd ~nsw:false ~nuw:false ~size ctx v1 v2, state
          | Quadrivalent_Lattice.(False | Top) ->
              Logger.alarm "ptr_arith";
              let res = Domain.Binary_Forward.biadd ~nsw:false ~nuw:false ~size ctx v1 v2 in
              let res = Domain.assume_binary ~size ctx c res in
              res, State.assume ctx c state
          end
      | Minus ->
          let minus_v2 = Domain.Binary_Forward.bisub ~size ~nsw:false ~nuw:false ctx
            (Domain.Binary_Forward.biconst ~size Z.zero ctx) v2 in
          let c = Domain.Binary_Forward.valid_ptr_arith ~size ctx v1 minus_v2 in
          Logger.check "ptr_arith";
          begin match Domain.Query.boolean ctx c |> Domain.Query.convert_to_quadrivalent with
          | Quadrivalent_Lattice.(True | Bottom) ->
              Domain.Binary_Forward.bisub ~nsw:false ~nuw:false ~size ctx v1 v2, state
          | Quadrivalent_Lattice.(False | Top) ->
              Logger.alarm "ptr_arith";
              let res = Domain.Binary_Forward.bisub ~nsw:false ~nuw:false ~size ctx v1 v2 in
              let res = Domain.assume_binary ~size ctx c res in
              res, State.assume ctx c state
          end
      | _ -> f ~size ctx v1 v2, state

    let rec expr' ctx e state =
      let open Dba.Expr in
      let open Dba.Binary_op in
      let open Basic_types in
      let open ListM in
      let size = Dba.Expr.size_of e in
      match e with
      | Var{Dba.name;_} -> [State.get state name, state]
      | Load(size,_,addr) ->
        expr ctx addr state >>= fun (loc,state) ->
          Logger.check "array_offset_access";
        let valid = Domain.Binary_Forward.valid ~size:(size * 8)
          Transfer_functions_sig.Read ctx loc in
        let state =
          begin match Domain.Query.(convert_to_quadrivalent (boolean ctx valid)) with
          | Quadrivalent_Lattice.(True | Bottom) -> state
          | _ ->
              let state = State.assume ctx valid state in
              (*
              (* VERY UGLY *)
              (* we also verify (and if not, assume) that the index is smaller than
               * the array size *)
              let open Ctypes in
              let state = match loc with
              | Region_binary_inst.T {typ = Type.ParamT ({descr = Ptr{pointed={descr = Array (_, ar_sz);_};_};_}, idx, _, _); _} ->
                  let isize = Type.array_index_size in
                  let module BR = Region_numeric.Operable_Value in
                  let binary_of_value ~size ctx = function
                  | Const x -> BR.Binary_Forward.biconst ~size x ctx
                  | Sym s -> Type.symbol ctx s
                  in
                  begin match ar_sz with
                  | Some sz ->
                      (* [i <u s] implies that [i <s s], provided that [s >=s 0].
                       * Here we assume [s >=s 0].
                       * [i <u s] also implies [i <=s s], which is what we
                       * need to verify. Incidentally, [i <u s] is the
                       * condition that will be the easiest to verify with our
                       * constraint domain and hypotheses on array sizes. *)
                      let sz_b = binary_of_value ~size:isize ctx sz in
                      let c3 = BR.Scalar.Boolean_Forward.not ctx @@
                        BR.Binary_Forward.biule ~size:isize ctx sz_b idx in
                      begin match Domain.Query.(convert_to_quadrivalent (boolean ctx c3)) with
                      | Quadrivalent_Lattice.True | Quadrivalent_Lattice.Bottom -> state
                      | _ ->
                          *)
                        Logger.error "-alarm- array_offset_access";
                        (*
                        State.assume ctx c3 state
                      end
                  | None -> state
                  end
              | _ -> assert false (* should not happen if condition was not verified *)
              in
              *)
              state
          end in
        let res, mem =
          try Domain.Memory_Forward.load ~size:(size * 8) ctx state.State.memory loc, state.State.memory
          with Domain.Needs_focusing (res, mem) -> res, mem
        in
        [res, {state with State.memory = mem}]
      | Cst(_,bv) ->
          let size = Bitvector.size_of bv in
          let value = z_of_bitvector bv in
          [Domain.Binary_Forward.biconst ~size value ctx, state]
      | Unary(Dba.Unary_op.Restrict{lo;hi},e1) ->
        let oldsize = Dba.Expr.size_of e1 in
        assert(size == 1 + hi - lo);
        expr ctx e1 state >>= fun (v,state) ->
        [Domain.Binary_Forward.bextract ~size ~index:lo ~oldsize ctx v, state]
      | Unary(Dba.Unary_op.Uext size, e1) ->
        let oldsize = Dba.Expr.size_of e1 in
        expr ctx e1 state >>= fun (v1,state) ->
        [Domain.Binary_Forward.buext ~size ~oldsize ctx v1,state]
      | Unary(Dba.Unary_op.Sext size, e1) ->
        let oldsize = Dba.Expr.size_of e1 in
        expr ctx e1 state >>= fun (v1,state) ->
        [Domain.Binary_Forward.bsext ~size ~oldsize ctx v1,state]
      | Unary(Dba.Unary_op.Not,e) ->
          expr ctx e state >>= fun (v,state) ->
          if size = 1 then
            let v = Domain.Boolean_Forward.not ctx (bool_of_bin ctx v) in
            [bin_of_bool ctx v,state]
          else
            let zero = Domain.Binary_Forward.biconst ~size Z.zero ctx in
            let v = Domain.Binary_Forward.bxor ~size ctx v zero in
            [v,state]
      | Unary(Dba.Unary_op.UMinus,e) ->
          expr ctx e state >>= fun (v,state) ->
          let v = Domain.Binary_Forward.bisub ~size ~nsw:false ~nuw:false ctx
              (Domain.Binary_Forward.biconst ~size Z.zero ctx) v in
          [v,state]
      | Binary((RightRotate|LeftRotate),e1,Dba.Expr.Cst(_,bv))
        when Bitvector.is_zeros bv -> expr ctx e1 state
      | Binary(Dba.Binary_op.Concat, e1, e2) ->
        expr ctx e1 state >>= fun (v1,state) ->
        expr ctx e2 state >>= fun (v2,state) ->
        let size1 = Dba.Expr.size_of e1 in
        let size2 = Dba.Expr.size_of e2 in
        [Domain.Binary_Forward.bconcat ~size1 ~size2 ctx v1 v2, state]
      | Binary(bop, (Cst (_,bv1) as e1), e2) ->
        expr ctx e1 state >>= fun (v1,state) ->
        expr ctx e2 state >>= fun (v2, state) ->
        let size1 = Dba.Expr.size_of e1 in
        let size2 = Dba.Expr.size_of e2 in
        let v1' = if size1 <> size2
          then
            let value1 = z_of_bitvector bv1 in
            Domain.Binary_Forward.biconst ~size:size2 value1 ctx
          else v1
        in
        [binop ctx ~size:size2 bop v1' v2 state]
      | Binary(bop, e1, (Cst (_,bv2) as e2)) ->
        expr ctx e1 state >>= fun (v1,state) ->
        expr ctx e2 state >>= fun (v2,state) ->
        let size1 = Dba.Expr.size_of e1 in
        let size2 = Dba.Expr.size_of e2 in
        let v2' = if size1 <> size2
          then
            let value2 = z_of_bitvector bv2 in
            Domain.Binary_Forward.biconst ~size:size1 value2 ctx
          else v2
        in
        [binop ctx ~size:size1 bop v1 v2' state]
      | Binary(bop,e1,e2) ->
        expr ctx e1 state >>= fun (v1,state) ->
        expr ctx e2 state >>= fun (v2,state) ->
        let size1 = Dba.Expr.size_of e1 in
        let size2 = Dba.Expr.size_of e2 in
        assert(size1 == size2);
        assert(size == 1 || size == size1); (* Predicate, or regular binary operator. *)
        [binop ctx ~size:size1 bop v1 v2 state]
      | Ite (cond,e_then,e_else) ->
        expr ctx cond state >>= fun (cond_abs, state) ->
        let zero = Domain.Binary_Forward.biconst ~size:1 Z.zero ctx in
        let condelse = Domain.Binary_Forward.beq ~size:1 ctx cond_abs zero in
        let condthen = Domain.Boolean_Forward.not ctx condelse in
        begin match Domain.Query.convert_to_quadrivalent @@
            Domain.Query.boolean ctx condelse with
        | Lattices.Quadrivalent.Bottom -> []
        | Lattices.Quadrivalent.True ->
          (* Careful: we tested the value of the "else" condition, so in this
           * case we must take the "else" branch *)
          expr ctx e_else state
        | Lattices.Quadrivalent.False -> expr ctx e_then state
        | Lattices.Quadrivalent.Top ->
          let state_then = State.assume ctx condthen state in
          let state_else = State.assume ctx condelse state in
          expr ctx e_then state_then @ expr ctx e_else state_else
        end

    and expr ctx e state =
      expr' ctx e state

    let static_target_to_address =
      let open Dba in function
      | JInner id -> Jump_Inner id
      | JOuter {base;id}->
        assert(id == 0); Jump_Outer base

    let _start =
      let cache = ref None in
      function img ->
      match !cache with
      | Some a -> a
      | None ->
        begin match Loader_utils.address_of_symbol ~name:"_start" (Lazy.force img) with
        | None -> assert false
        | Some a -> cache := Some a; a
        end

    let _end_of_kernel =
      let cache = ref None in
      function img ->
      match !cache with
      | Some a -> a
      | None ->
        begin match Loader_utils.address_of_symbol ~name:"_end_of_kernel" (Lazy.force img) with
        | None -> assert false
        | Some a -> cache := Some a; a
        end

    let _idle =
      let cache = ref None in
      function img ->
      match !cache with
      | Some a -> a
      | None ->
        begin match Loader_utils.address_of_symbol ~name:"idle" (Lazy.force img) with
        | None -> assert false
        | Some a -> cache := Some a; a
        end

    let instr ctx state =
      let open Dba in
      let open ListM in
      let open Dba.Instr in function
        | Assign(LValue.Var{name=v;_},e,id) ->
          (*let bin,state = expr ctx e state in*)
          expr ctx e state >>= fun (bin,state) ->
          [Jump_Inner id, State.set ctx state v bin]
        | Assign(LValue.Store(size,_,addr),e,id) ->
          expr ctx e state >>= fun (v,state) ->
          expr ctx addr state >>= fun (loc,state) ->
          let valid = Domain.Binary_Forward.valid ~size:(size * 8)
            Transfer_functions_sig.Write ctx loc in
          let state =
            begin match Domain.Query.(convert_to_quadrivalent (boolean ctx valid)) with
            | Quadrivalent_Lattice.(True | Bottom) -> state
            | _ ->
                let state = State.assume ctx valid state in
                (*
                (* VERY UGLY *)
                (* we also verify (and if not, assume) that the index is smaller than
                 * the array size *)
                let open Ctypes in
                let state = match loc with
                | Region_binary_inst.T {typ = Type.ParamT ({descr = Ptr{pointed={descr = Array (_, ar_sz);_};_};_}, idx, _, _); _} ->
                    let isize = Type.array_index_size in
                    let module BR = Region_numeric.Operable_Value in
                    let binary_of_value ~size ctx = function
                    | Const x -> BR.Binary_Forward.biconst ~size x ctx
                    | Sym s -> Type.symbol ctx s
                    in
                    begin match ar_sz with
                    | Some sz ->
                        (* [i <u s] implies that [i <s s], provided that [s >=s 0].
                         * Here we assume [s >=s 0].
                         * [i <u s] also implies [i <=s s], which is what we
                         * need to verify. Incidentally, [i <u s] is the
                         * condition that will be the easiest to verify with our
                         * constraint domain and hypotheses on array sizes. *)
                        let sz_b = binary_of_value ~size:isize ctx sz in
                        let c3 = BR.Scalar.Boolean_Forward.not ctx @@
                          BR.Binary_Forward.biule ~size:isize ctx sz_b idx in
                        begin match Domain.Query.(convert_to_quadrivalent (boolean ctx c3)) with
                        | Quadrivalent_Lattice.True | Quadrivalent_Lattice.Bottom -> state
                        | _ ->
                          Logger.error "-alarm- array_offset_access";
                          State.assume ctx c3 state
                        end
                    | None -> state
                    end
                | _ -> assert false (* should not happen if condition was not verified *)
                in
                *)
                state
            end in
          let memory =
            Domain.Memory_Forward.store ~size:(size * 8) ctx state.State.memory loc v
          in
          [Jump_Inner id,{state with State.memory}]
        | Assign(LValue.Restrict({name=v;_}, Interval.({lo;hi})), e_rval, id) ->
          let size_v = List.assoc v Reg.registers in
          expr ctx e_rval state >>= fun (rval,state) ->
          Logger.debug ~level:3 "%s{%d,%d} := %a" v lo hi (Domain.binary_pretty ~size:size_v ctx) rval;
          let initial = State.get state v in
          let written_size = 1 + hi - lo in
          let new_v =
            if lo = 0 then rval
            else
              let lsb = Domain.Binary_Forward.bextract ~size:lo ~index:0
                ~oldsize:size_v ctx initial in
              Domain.Binary_Forward.bconcat ~size1:written_size ~size2:lo ctx rval lsb
          in
          let new_v =
            if hi = size_v - 1 then new_v
            else
              let msb = Domain.Binary_Forward.bextract ~size:(size_v - hi - 1)
                ~index:(hi+1) ~oldsize:size_v ctx initial in
              Domain.Binary_Forward.bconcat ~size1:(size_v - hi - 1)
                ~size2:(hi+1) ctx msb new_v
          in
          [Jump_Inner id, State.set ctx state v new_v]
        | If(cond,target,id) ->
          expr ctx cond state >>= fun (bin,state) ->
          let zero = Domain.Binary_Forward.biconst ~size:1 Z.zero ctx in
          let condelse = Domain.Binary_Forward.beq ~size:1 ctx bin zero in
          let condthen = Domain.Boolean_Forward.not ctx condelse in
          (match Domain.Query.convert_to_quadrivalent @@
              Domain.Query.boolean ctx condelse with
          | Lattices.Quadrivalent.Bottom -> []
          | Lattices.Quadrivalent.True ->
              (* Careful: we tested the value of the "else" condition, so in this
               * case we must take the "else" branch *)
              [Jump_Inner id,state]
          | Lattices.Quadrivalent.False -> [static_target_to_address target,state]
          | _ ->
            let state_then = State.assume ctx condthen state in
            let state_else = State.assume ctx condelse state in
            [(static_target_to_address target,state_then);(Jump_Inner id,state_else)]
          )
        | Stop _ -> Logger.result "Stop called"; []
        | Assert(Dba.Expr.Cst(_,x),_) when Bitvector.is_zero x ->
            Logger.result "Warning: assert false"; []
        | Assert (cond, id) ->
          expr ctx cond state >>= fun (cond_v, state) ->
          let zero = Domain.Binary_Forward.biconst ~size:1 Z.zero ctx in
          let notc = Domain.Binary_Forward.beq ~size:1 ctx cond_v zero in
          let c = Domain.Boolean_Forward.not ctx notc in
          let c = Domain.Query.convert_to_quadrivalent (Domain.Query.boolean ctx c) in
          let open Quadrivalent_Lattice in
          begin match c with
          | False | Top ->
            Logger.fatal "assert %a may be false" (Domain.binary_pretty ~size:1 ctx) cond_v;
          | Bottom -> []
          | True -> [Jump_Inner id, state]
          end
        | Assume _ -> assert false
        | SJump(jt,_) ->
          let target_addr = static_target_to_address jt in
          (*
          begin match target_addr with
          | Jump_Outer target_addr ->
              let lazy_img = lazy (Kernel_functions.get_img ()) in
              if Virtual_address.to_int target_addr = idle lazy_img then
                (* Go to mock address where the abstract task simulator should be hooked *)
                [Jump_Outer (Virtual_address.create 0xcafecaf0), state]
              else [Jump_Outer target_addr, state]
          | Jump_Inner _ | Jump_Dynamic ->*) [target_addr, state]
          (*end*)
        | DJump(e,_) ->
          expr ctx e state >>= fun (v,state) ->
          let size = (Dba.Expr.size_of e) in
          let res =
            Domain.Query.binary_to_ival ~signed:false ~size @@ Domain.Query.binary ~size ctx v in
          Logger.check "unresolved_dynamic_jump";
          if not (Ival.cardinal_is_less_than res 8) then begin
            (*
            match v with
            | Region_binary_inst.T {typ=Type.ParamT(Ctypes.({descr=Ptr{pointed=pt;_};_}),_,_,0);_}
                when pt == X86_types.task_eip ->
              (* Go to mock address where the abstract task simulator should be hooked *)
              [(Jump_Outer (Virtual_address.create 0xcafecaf0), state)]
            | _ ->
            *)
            let cpl = State.get state "cpl" |> Domain.Query.binary ~size:2 ctx
            |> Domain.Query.binary_to_ival ~signed:false ~size:2 |> Ival.project_int
            in
            if Z.equal cpl Z.zero then begin
              Logger.alarm "unresolved_dynamic_jump";
              Logger.error "more than 8 targets in DJump: %a" Ival.pretty res;
              []
            end else if Z.equal cpl (Z.of_int 3) then begin
              Logger.result "@[<hov 2>unknow target %a with CPL = 3, approximating by user code transition@]" Ival.pretty res;
              [(Jump_Outer (Virtual_address.create 0xcafecaf0), state)]
            end else
              assert false
          end else begin
            let lazy_img = lazy (Kernel_functions.get_img ()) in
            snd @@ Ival.fold_int (fun i (already_outofkernel, acc) ->
                if Codex_options.AnalyzeKernel.get () && (Z.lt i (Z.of_int @@ _start lazy_img)
                || Z.gt i (Z.of_int @@ _end_of_kernel lazy_img))
                (*|| Z.equal i (Z.of_int @@ idle lazy_img)*) then
                  if not already_outofkernel then
                  (* Go to mock address where the abstract task simulator should be hooked *)
                  true, (Jump_Outer (Virtual_address.create 0xcafecaf0), state) :: acc
                  else already_outofkernel, acc
                else
                already_outofkernel, (Jump_Outer (Virtual_address.of_bitvector @@ bitvector_of_z i), state)
                :: acc
              ) res (false,[])
          end
        | Undef (Dba.LValue.Var{name=v;_}, id) ->
          let size = List.assoc v Reg.registers in
          let new_v = bunknown ~size ctx in
          [Jump_Inner id, State.set ctx state v new_v]
        | _ -> assert false

  end

end
