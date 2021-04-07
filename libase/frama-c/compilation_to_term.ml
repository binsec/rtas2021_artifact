(**************************************************************************)
(*                                                                        *)
(*  This file is part of Frama-C.                                         *)
(*                                                                        *)
(*  Copyright (C) 2007-2014                                               *)
(*    CEA (Commissariat à l'énergie atomique et aux énergies              *)
(*         alternatives)                                                  *)
(*                                                                        *)
(*  you can redistribute it and/or modify it under the terms of the GNU   *)
(*  Lesser General Public License as published by the Free Software       *)
(*  Foundation, version 2.1.                                              *)
(*                                                                        *)
(*  It is distributed in the hope that it will be useful,                 *)
(*  but WITHOUT ANY WARRANTY; without even the implied warranty of        *)
(*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *)
(*  GNU Lesser General Public License for more details.                   *)
(*                                                                        *)
(*  See the GNU Lesser General Public License version 2.1                 *)
(*  for more details (enclosed in the file licenses/LGPLv2.1).            *)
(*                                                                        *)
(**************************************************************************)

open Codex

module StringMap = Map.Make(String)
module VarMap = Cil_datatype.Varinfo.Map

(* An initial syntactic pass on the code; notably to know which
   globals are necessary. *)
module PreAnalysis(Main:sig val main: Kernel_function.t end) = struct

  module VISet = Cil_datatype.Varinfo.Set;;
  module StringSet = Datatype.String.Set;;

  type acc = {
    functions_used: VISet.t;
    functions_visited:VISet.t;
    globals_used: VISet.t;
    globals_visited: VISet.t;
    strings_used: StringSet.t
  }

  open Cil_types;;

  let visit_constant acc = function
    | CStr str -> {acc with strings_used = StringSet.add str acc.strings_used }
    | CWStr _ -> Codex_options.fatal "Wide strings not yet handled"
    | CInt64 _ | CChr _ | CReal _ | CEnum _ -> acc
  
  let rec visit_expr acc exp = match exp.enode with
    | Const c -> visit_constant acc c
    | Lval lv -> visit_lval acc lv
    | SizeOf _ | SizeOfE _ | SizeOfStr _
    | AlignOf _ | AlignOfE _ -> acc
    | UnOp(_,e,_) | CastE(_,e) | Info(e,_) -> visit_expr acc e
    | BinOp(_,e1,e2,_) ->
      let acc = visit_expr acc e1 in
      visit_expr acc e2
    | AddrOf lv | StartOf lv -> visit_lval acc lv

  and visit_lval acc (lhost,offset) =
    let acc = visit_lhost acc lhost in
    visit_offset acc offset

  and visit_lhost acc = function
    | Var vi ->
      if vi.vglob
      then match vi.vtype with
        | (TFun _) -> {acc with functions_used = VISet.add vi acc.functions_used}
        | _ -> {acc with globals_used = VISet.add vi acc.globals_used }
      else acc
    | Mem e -> visit_expr acc e

  and visit_offset acc = function
    | NoOffset -> acc
    | Field(_,o) -> visit_offset acc o
    | Index(e,o) ->
      let acc = visit_expr acc e in
      visit_offset acc o

  let rec visit_init acc = function
    | SingleInit e -> visit_expr acc e
    | CompoundInit(_,l) -> List.fold_left (fun acc (o,i) ->
        let acc = visit_offset acc o in
        visit_init acc i
      ) acc l
  ;;
  
  let visit_local_init acc = function
    | AssignInit i -> visit_init acc i
    | ConsInit(f,args,_) ->
      let acc = visit_lhost acc (Var f) in
      List.fold_left visit_expr acc args  

  
  let visit_instr acc = function
    | Set(lv,e,_) ->
      let acc = visit_lval acc lv in
      visit_expr acc e
    | Call(lv,e,args,_) ->
      let acc = match lv with
        | None -> acc
        | Some lv -> visit_lval acc lv in
      let acc = visit_expr acc e in
      List.fold_left visit_expr acc args
    | Asm _ ->  acc
    | Skip _ -> acc
    | Code_annot _ -> acc
    | Local_init(vi,li,_)  ->
      let acc = visit_lhost acc (Var vi) in
      visit_local_init acc li
  ;;
  
  let rec visit_block acc block =
    List.fold_left visit_stmt acc block.bstmts

  and visit_stmt acc stmt =
    match stmt.skind with
    | Instr i -> visit_instr acc i
    | Return(Some e,_) -> visit_expr acc e
    | Return(None,_) -> acc
    | Goto _ | Break _ | Continue _ -> acc
    | If(e,b1,b2,_) ->
      let acc = visit_expr acc e in
      let acc = visit_block acc b1 in
      visit_block acc b2
    | Switch(e1,b1,_,_) ->
      let acc = visit_expr acc e1 in
      visit_block acc b1
    | Loop(_,b,_,_,_)
    | Block b -> visit_block acc b
    | UnspecifiedSequence l ->
      visit_block acc @@ Cil.block_from_unspecified_sequence l
    | Throw _ | TryCatch _ | TryFinally _ | TryExcept _ -> assert false


  let visit_fun acc vi =
    Kernel.feedback "visiting function %s" vi.vname;
    (* let vi = Kernel_function.get_vi kf in *)
    let acc = {acc with functions_visited = VISet.add vi acc.functions_visited} in
    try
      let fd = Kernel_function.get_definition @@ Globals.Functions.get vi in
      visit_block acc fd.sbody 
    with Kernel_function.No_Definition -> acc
  ;;
  
  (* Globals might be referencing other globals, including functions. *)
  let visit_global acc vi =
    let acc = {acc with globals_visited = VISet.add vi acc.globals_visited } in
    let initinfo = Globals.Vars.find vi in
    match initinfo.init with
    | None -> acc
    | Some init -> visit_init acc init
  ;;
  
  let result =
    let main_vi = Kernel_function.get_vi Main.main in
    let acc = {
      functions_used = VISet.singleton main_vi;
      globals_used = VISet.empty;
      globals_visited = VISet.empty;
      functions_visited =  VISet.empty;
      strings_used = StringSet.empty
    } in
    let rec loop acc = 
      let fun_to_visit = VISet.diff acc.functions_used acc.functions_visited in
      if not @@ VISet.is_empty fun_to_visit
      then loop @@ VISet.fold (fun vi acc -> visit_fun acc vi) fun_to_visit acc
      else
        let globals_to_visit = VISet.diff acc.globals_used acc.globals_visited in
        if not @@ VISet.is_empty globals_to_visit
        then loop @@ VISet.fold (fun vi acc -> visit_global acc vi) globals_to_visit acc
        else acc
    in
    let acc = loop acc in
    (acc.functions_used, acc.globals_used, acc.strings_used)
  ;;
  
end



(* Table where to register assertions. Note that it is imperative, but
   it should be easy to switch to a functional version (using the
   state monad to pass it). *)
module Assertion_Table = struct

  type kind =
    | SVComp of Cil_types.location
    | Alarm of Alarms.t * Cil_types.location

  (* Current level of nesting; assertions are not registered when
     inside a mu (they should rather be exported as loop
     invariants) *)

  (* Note: maybe both directions would be useful. *)
  (* Note: Uses a kind list instead of a kind, as a single term may
     correspond to several alarms.  *)
  type t = kind list Term.Hashtbl.t

  let create () = Term.Hashtbl.create 17

  let register table (u:Term.t) (v:kind) =
    let res =
      try Term.Hashtbl.find table u
      with Not_found -> [] in
    Term.Hashtbl.replace table u (v::res);;

  (* Iterate in term creation order, which is good because earlier assertions are created first.  *)
  let iter table f =
    let map = Term.Hashtbl.fold Term.Map.add table Term.Map.empty in
    Term.Map.iter (fun term list -> List.iter (f term) list) map
  ;;

  let pretty_kind fmt = function
    | SVComp loc -> Format.fprintf fmt "%a:svcomp" Cil_datatype.Location.pretty loc
    | Alarm (al,loc) -> Format.fprintf fmt "%a: alarm %a" Cil_datatype.Location.pretty loc Alarms.pretty al 

  let _pretty fmt (table,_) =
    iter table (fun x _ ->
      Format.fprintf fmt "@\n* @[%a@]" Term.pretty_short x);;

end

module Reachable_Table = struct

  (* The list of terms to check for reachability. *)
  type t = unit Term.Hashtbl.t

  let create() = Term.Hashtbl.create 17;;

  let register table term =
    Term.Hashtbl.replace table term ();;

  let iter t f = Term.Hashtbl.iter (fun x () -> f x) t;;

  let is_empty t = Term.Hashtbl.length t == 0
  
end

type callsite = Kernel_function.t * Cil_types.kinstr (* Which statement called wich function *)
module Callsite = (Datatype_sig.Prod2(Kernel_function)(Cil_datatype.Kinstr));;

type callstack = callsite list                       (* Head is latest call *)

module Callstack = Datatype_sig.List(Callsite);;

(* Translation of a statement. *)

module Build = Build_semantic_equations;;

(* TODO: rather than having 
    type continuation =  Stmt of Cil_types.stmt| Return

   We should have something like 
   type continuation =  BB of BasicBlock.t | Return *)

module SmartCons = Term_smart_constructors(* .Make(Ref_Addr) *)


let bin_of_bool ~level ~size cond =
  let one = (SmartCons.Binary.biconst size Z.one) in
  let zero = (SmartCons.Binary.biconst size Z.zero) in
  let one = SmartCons.Binary.assume ~size cond one in
  let typ =  (Immutable_dynamic_array.singleton (Term_types.Binary size)) in
  let then_ = SmartCons.Tuple.tuple_create (Immutable_dynamic_array.singleton one) in
  let zero = SmartCons.Binary.assume ~size (SmartCons.Boolean.not cond) zero in
  let else_ = SmartCons.Tuple.tuple_create (Immutable_dynamic_array.singleton zero) in
  let join = SmartCons.Tuple.nondet ~level typ [then_;else_] in
  SmartCons.Tuple.tuple_get join 0
;;

(* Note: we don't use bin_of_bool yet, because it is very slow on the KCG benchmark. *)
let _bin_of_bool ~level ~size cond = SmartCons.Binary.bofbool ~size cond;;

(* Forces the evaluation of each condition, in case this is interesting.  *)

let _bin_of_bool ~level ~size cond = 
  SmartCons.Binary.assume ~size
    (SmartCons.Boolean.(||) cond @@ SmartCons.Boolean.not cond) 
    (SmartCons.Binary.bofbool ~size cond);;

(* Also forces the evaluation of each condition; this works better,
   but slow on kcg. *)
(* TODO: check why this gains precision in exobci-step1. *)
let _bin_of_bool ~level ~size cond =
  let x = SmartCons.Binary.bofbool ~size cond in
  let one = SmartCons.Binary.assume ~size cond x in
  let typ =  (Immutable_dynamic_array.singleton (Term_types.Binary size)) in
  let then_ = SmartCons.Tuple.tuple_create (Immutable_dynamic_array.singleton one) in
  let zero = SmartCons.Binary.assume ~size (SmartCons.Boolean.not cond) x in
  let else_ = SmartCons.Tuple.tuple_create (Immutable_dynamic_array.singleton zero) in
  let join = SmartCons.Tuple.nondet ~level typ [then_;else_] in
  SmartCons.Tuple.tuple_get join 0
;;



let ptr_size = (Cil.bitsSizeOf Cil.voidPtrType);;

(* Note: keys of the trie are _reversed_ callstacks (head is main). *)
module Callstack_Trie = Mutable_radix_trie.Make(Callsite)



module Make(P:sig end) = struct

  let assert_table = Assertion_Table.create();;
  let reachable_table = Reachable_Table.create();;
  let current_stmt = ref (Cil_types.Kstmt Cil.dummyStmt);;
  let loop_nesting = ref (-30);;
  let callstack = ref [];;

module Lang = struct

  module Monadic_Arity = struct
    (* NONEED: Pass the current condition under which the term is built
       as a "state" of the continuation monad. This may be used to
       record, for each term, the conditions under which these terms
       are "executed". *)

    (* Pass the state of the world as a monad. *)
    type 'a m = 'a
    let return x = x
    let bind a f = f a 
    type 'r ar0 = 'r m
    type ('a,'r) ar1 = 'a -> 'r m
    type ('a,'b,'r) ar2 = 'a -> 'b -> 'r m
    type ('a,'b,'c,'r) ar3 = 'a -> 'b -> 'c -> 'r m
    type ('a,'r) variadic = 'a list -> 'r m

  end
  type 'a m = 'a Monadic_Arity.m

  let return = Monadic_Arity.return

  module Binary = struct
    include SmartCons.Binary
    let binary_unknown ~size = (SmartCons.Binary.bunknown_explicit ~size ~level:!loop_nesting);;    
  end
  module Boolean = SmartCons.Boolean

  type env = {
    var_addresses: Binary.binary VarMap.t;
    string_adresses: Binary.binary StringMap.t;
  }

  module Memory = struct
    type memory =
      | Bottom_Memory
      | Some_Memory of {
      (* MAYBE: speed-up memory model by not storing "simple"
         (never-referenced) variables into memory. Needs a
         pre-analysis; e.g. WP "RefUsage"; alternatively we could do
         that only for temporary variables. *)
      mem: Term.t;              (* The memory term. *)
      env: env                  (* Mapping from variables, strings etc. to Codex terms. *)
    }

    type binary = Term.t
    type boolean = Term.t
                     
    let assume a b =
      match b with
      | Bottom_Memory -> Bottom_Memory
      | Some_Memory b ->
        Some_Memory{mem = SmartCons.Memory.assume a b.mem;env=b.env }
    let load ~size a b = match a with
      | Bottom_Memory -> assert false
      | Some_Memory a -> SmartCons.Memory.load ~size a.mem b
    let store ~size a b c  = match a with
      | Bottom_Memory -> Bottom_Memory
      | Some_Memory a ->
        Some_Memory{mem = SmartCons.Memory.store ~size a.mem b c;env=a.env}
    let memcpy ~size a b c  = match a with
      | Bottom_Memory -> Bottom_Memory
      | Some_Memory a ->
        Some_Memory{mem = SmartCons.Memory.memcpy ~size a.mem b c;env=a.env}          
    let malloc ~id ~malloc_size m =
      match m with
      | Bottom_Memory -> assert false
      | Some_Memory m ->
        let ptr,mem = SmartCons.Memory.malloc ~id ~malloc_size m.mem in
        ptr,(Some_Memory{mem=mem;env=m.env})
    let free mem ptr = match mem with
      | Bottom_Memory -> Bottom_Memory
      | Some_Memory mem -> Some_Memory{mem = SmartCons.Memory.free mem.mem ptr;env=mem.env}
    let unknown ~level = Some_Memory{mem = SmartCons.Memory.unknown ~level;
                                     env= {var_addresses=VarMap.empty;
                                           string_adresses=StringMap.empty
                                          }}
  end

  let value_of_truth size  bool = (bin_of_bool ~level:!loop_nesting ~size bool);;

  type binary = Term.t
  type boolean = Term.t
  type memory = Memory.memory
  type continuation = Cil_types.stmt Region_analysis.edge

  (* For each outgoing edge of a statement, we return the memory when
     the edge is taken. *)
  type answer = (continuation * memory) list

  (* Give a unique malloc_id per hash. In particular, this allows that
     when there is a jump into a scope, both "malloced" local
     variables will have the same id, and thus can be strongly
     updated. *)
  let varinfo_id =
    let varinfo_id_hash = Cil_datatype.Varinfo.Hashtbl.create 17 in
    fun vi ->
      try Cil_datatype.Varinfo.Hashtbl.find varinfo_id_hash vi
      with Not_found ->
        let id =
          Transfer_functions.Malloc_id.fresh (Format.asprintf "%a" (* Varinfo_Enclosing_Function *)Cil_datatype.Varinfo.pretty vi) in
        Cil_datatype.Varinfo.Hashtbl.replace varinfo_id_hash vi id;
        id
  ;;

  (* Allocate a variable, and maybe fill it with some data. *)
  let allocate_var mem vi initf =
    match mem with
    | Memory.Bottom_Memory -> Memory.Bottom_Memory
    | (Memory.Some_Memory mem) ->
    Kernel.feedback "allocating %a" Cil_datatype.Varinfo.pretty vi;
    let id = varinfo_id vi in
    assert(not @@ VarMap.mem vi mem.env.var_addresses);
    let exception Function_Type in
    let addr,mem = try
        let malloc_size =
          try Cil.bytesSizeOf vi.Cil_types.vtype
          with Cil.SizeOfError _ ->
            if Cil.isFunctionType vi.Cil_types.vtype
            then raise Function_Type
            else
              Kernel.warning "Could not compute the size of type %a for %a, defaulting  to 1"
                Cil_datatype.Typ.pretty vi.Cil_types.vtype
                Cil_datatype.Varinfo.pretty vi;
            1
        in
        let size = 8 * malloc_size in
        let addr,mem = Memory.malloc ~id ~malloc_size (Memory.Some_Memory mem) in
        match initf with
        | None -> addr,mem       (* No initial write. *)
        | Some initf -> 
          let to_store = initf ~size in
          let mem = Memory.store ~size mem addr to_store in
          addr,mem
      (* Functions are not deallocated, so we don't have to allocate anything. 
         MAYBE: do the same for every global variables of unknown size? *)
      with Function_Type -> Binary.binary_unknown ~size:ptr_size, Memory.Some_Memory mem
    in
    let addr,(Memory.Some_Memory mem) = addr,mem in
    let var_addresses = VarMap.add vi addr mem.env.var_addresses in
    let mem = Memory.Some_Memory{mem with env = {mem.env with var_addresses}} in
    mem
  ;;

  let deallocate_var mem vi =
    let open Memory in
    match mem with
    | Bottom_Memory -> Bottom_Memory
    | Some_Memory mem ->
      Kernel.feedback "deallocating %a" Cil_datatype.Varinfo.pretty vi;
      let addr = VarMap.find vi mem.env.var_addresses in
      let (Memory.Some_Memory mem) = Memory.free (Some_Memory mem) addr in
      let var_addresses = VarMap.remove vi mem.env.var_addresses in
      Memory.Some_Memory{mem with env = {mem.env with var_addresses}}
  ;;
  
  let compile_block_entry mem block =
    let locals = block.Cil_types.blocals in
    let to_store ~size = 
      if Codex_options.UnknownUninitialized.get() then begin
        (* The SVComp competition has a wrong interpretation of the C
           standard, and assumes that uninitialized local variables can have
           any value (instead of no value). This block of code allows to
           have this interpretation. *)
        SmartCons.Binary.bunknown_explicit ~size ~level:!loop_nesting
      end
      else begin
        (SmartCons.Binary.buninit ~size)
      end
    in
    List.fold_left (fun (mem) vi -> allocate_var mem vi @@ Some to_store) mem locals
  ;;

  let compile_block_close mem block =
    (* Kernel.feedback "compile block close %a" Cil_datatype.Block.pretty block; *)
    let locals = block.Cil_types.blocals in
    List.fold_left (fun mem vi ->
        (* Kernel.feedback "freeing %a" Cil_datatype.Varinfo.pretty vi; *)
        deallocate_var mem vi
      ) mem locals
  ;;

    


  
  (* Watch the blocks opened and closed between two edges, and insert
     statements inbetween regarding the allocation/deallocation of
     local variables.  *)
  let block_crossing (cont,mem) =
    let blocks_opened,blocks_closed =
      let open Region_analysis in match cont with
      | Edge(s1,s2) -> Kernel_function.blocks_opened_by_edge s1 s2,
                       Kernel_function.blocks_closed_by_edge s1 s2
      | Exit stmt -> [],[]
    in
    let mem = List.fold_left (fun (mem) block ->
        compile_block_close mem block)
        mem blocks_closed in
    let (mem) = List.fold_left (fun (mem) block ->
        compile_block_entry mem block)
        (mem) blocks_opened in
    (cont,mem);;

  let blocks_crossing (cont_mems) =
    let cont_mems = 
      List.fold_left (fun (acc) contmem ->
          let (contmem) = block_crossing contmem in
          (contmem::acc)
        ) ([]) cont_mems in
    List.rev cont_mems


  let apply_cont kcont mem = blocks_crossing @@ ([(kcont,mem)]);;

  let choose_and_apply_cont cond kthen kelse mem =
    let notcond = SmartCons.Boolean.not cond in
    blocks_crossing @@
      ([(kthen, Memory.assume cond mem);
        (kelse, Memory.assume notcond mem)])
  ;;

  module ZSet = Set.Make(Z);;
  (* MAYBE: for the default case, it might be better to implement it
     as a cascade of "assume x != i". This requires a different
     format for the continuations, that would also be useful to
     handle "&&" and "||" in conditions. *)
  let select_and_apply_cont (value:binary) cases mem =
    let module Integer = Datatype.Integer in
    let size = Term.size_of value in

    let (regular_cases,integers,default_cont) = List.fold_left
      (fun acc (cases,cont) ->
        Build.Case.Set.fold (fun case (regular_cases,integers,default_cont) ->
          match case with
          | Build.Case.Default -> (regular_cases,integers,Some cont)
          | Build.Case.Case i ->
             let cond = SmartCons.Binary.beq ~size (SmartCons.Binary.biconst ~size i) value in
             ((cont, Memory.assume cond mem)::regular_cases,
              ZSet.add i integers,
              default_cont)
        ) cases acc) ([],ZSet.empty,None) cases
    in

    let l =
      match default_cont with
      (* In C, there is always a default case, which may be the
         syntactic successor of the switch. *)
      | None -> assert false (* regular_cases *)
      | Some cont ->
         let default_cond =
           ZSet.fold (fun i acc ->
             SmartCons.Boolean.(&&) acc (SmartCons.Boolean.not (SmartCons.Binary.beq ~size (SmartCons.Binary.biconst ~size i) value)))
             integers SmartCons.Boolean.true_
         in (cont,Memory.assume default_cond mem)::regular_cases
    in blocks_crossing @@ (l)
  ;;

  let no_continuation mem = return [];;

end

module Stmt = Cil_datatype.Stmt

module EltTrie(D:Datatype.S_with_collections) = struct

  module H = D.Hashtbl
  
  let replace elth (elt,callstack) term =
    let trie = 
      try H.find elth elt
      with Not_found ->
        let trie = Callstack_Trie.create () in
        H.replace elth elt trie;
        trie
    in Callstack_Trie.replace trie (List.rev callstack) term
  ;;

  let find elth (elt,callstack) term =
    let trie = 
      try H.find elth elt
      with Not_found -> raise Not_found
  in Callstack_Trie.find trie (List.rev callstack) 
  ;;

  let create() = H.create 17;;

  let pretty fmt elth =
    elth |> H.iter (fun elt trie ->
        Callstack_Trie.pp_trie Term.pretty_short fmt trie
        
      )
  
end

(* This works for lval that do not have eids, but also in case eids are incorrect... *)
module ExpInStmt = 
  Datatype.Pair_with_collections
    (Cil_datatype.Kinstr)
    (Cil_datatype.ExpStructEq)
    (struct let module_name = "ExpInStmt" end)

module LvalInStmt = 
  Datatype.Pair_with_collections
    (Cil_datatype.Kinstr)
    (Cil_datatype.LvalStructEq)
    (struct let module_name = "LvalInStmt" end)

(* TODO: An alarm trie. *)
module ExpTrie = struct
  include EltTrie(ExpInStmt)

end
module LvalTrie = EltTrie(LvalInStmt)

let exptrie = ExpTrie.create ();;
let lvaltrie = LvalTrie.create ();;




module Conv = struct
  module L = Lang
  type env = Lang.env
  let continuation_of_stmt _env stmt succ = Region_analysis.Edge(stmt, succ);;

  let loc_of_var env vi =
    Kernel.feedback "finding vi %a" Cil_datatype.Varinfo.pretty vi;
    VarMap.find vi env.Lang.var_addresses;;

  let loc_of_string env str = StringMap.find str env.Lang.string_adresses
    (* baddr ~size:ptr_size (Ref_addr_non_modular.from_string str);; *)
  let loc_of_wstring _env str = assert false
    (* baddr ~size:ptr_size (Ref_addr_non_modular.from_wstring str);; *)
  let return_continuation _env stmt = Region_analysis.Exit stmt


  (* TODO: Probably the wrong interface; we should do an "assume"
     instead + registration in the table. *)
  (* Sometimes we will want the assume to be on one term, and
     sometimes on several, e.g. to state that there must be no
     overflow. *)
  let register_alarm alarm loc term =
    if !loop_nesting == 0 then
      Assertion_Table.register assert_table term (Assertion_Table.Alarm(alarm,loc));
    (())
  ;;

  (* Note: constructing the exp to t erm does not seem to have any
     impact on the performances. Still we avoid to do it if not
     necessary. *)
  let register_lvalue lval bin =
    if Codex_options.needs_exp_term_mapping() && !loop_nesting == 0 then
      (* Kernel.feedback "Registering lvalue %a %a %a callstack %a loop nesting %d" Cil_datatype.Lval.pretty lval Term.pretty_short bin Cil_datatype.Kinstr.pretty state.current_stmt Callstack.pretty state.callstack !loop_nesting; *)
      LvalTrie.replace lvaltrie ((!current_stmt,lval),!callstack) bin;
    (())
  ;;
  
  let register_expression exp bin =
    (* Kernel.feedback "Registering expression %a %a %a callstack %a loop nesting %d" Cil_datatype.Exp.pretty exp Term.pretty_short bin Cil_datatype.Kinstr.pretty state.current_stmt Callstack.pretty state.callstack !loop_nesting; *)
    if Codex_options.needs_exp_term_mapping() && !loop_nesting == 0 then
      ExpTrie.replace exptrie ((!current_stmt,exp),!callstack) bin;
    (())
  ;;
  
  let register_boolean_expression exp bin =
    (* Kernel.feedback "registering exp %a" Cil_datatype.Exp.pretty exp; *)
    let size = Cil.bitsSizeOf (Cil.typeOf exp) in
    if Codex_options.needs_exp_term_mapping() && !loop_nesting == 0 then begin
      let bin = bin_of_bool ~level:!loop_nesting ~size bin in
      ExpTrie.replace exptrie ((!current_stmt,exp),!callstack) bin;
    end;
    (())
  ;;

  
  
end

module Translator = Build.Make(Lang)(Conv);;

let dummy_reachable_table = Reachable_Table.create();;

(* Runs the L and M' monads, and retrieve the result. *)
let run mem x =
  let open Translator in
  let (x,mem) = M'.Monadic_Arity.run {mem=Some mem} x in
  let mem = Extlib.the mem.mem in
  (x,mem)

let run (Lang.Memory.Some_Memory mem) f v =
  let (x,mem) = Translator.M'.Monadic_Arity.run {mem=Some (Some_Memory mem)} @@ f mem.env v in
  let mem = Extlib.the mem.mem in
  x,mem


(****************************************************************)

open Region_analysis;;

let statement_input_types = Immutable_dynamic_array.from_array [| Term_types.Memory |]
open Term_types;;

module Transfer_functions =
struct
  type abstract_value = Lang.Memory.memory

  let join2 a b =
    let open Lang.Memory in
    match a,b with
    | Bottom_Memory, x | x, Bottom_Memory -> x
    | Some_Memory a, Some_Memory b ->
      let doit ()=
        let f x = SmartCons.Tuple.tuple_create @@ Immutable_dynamic_array.singleton x in
        let res = SmartCons.Tuple.nondet ~level:(!loop_nesting) statement_input_types [f a.mem; f b.mem] in
        let mem = SmartCons.Tuple.tuple_get res 0 in
        Some_Memory{mem = mem; env = a.env}
      in
      
      (*  Fast path, which happen when we join in a common scope. *)
      if a.env.var_addresses == b.env.var_addresses
      then doit()
      else
        let common_varaddr = VarMap.merge (fun addr a b -> match a,b with
            | None, None -> assert false
            | Some a, Some b when a == b -> None
            | Some _, None | None, Some _ ->
              assert false (* We should have the same variables in the same scope. *)
            | Some a, Some b -> Some(a,b) (* Happens when there is a jump to inside of a block. *)
          ) a.env.var_addresses b.env.var_addresses  in

        if (VarMap.is_empty common_varaddr) (* Usually the case. *)

        then doit()
        else
          let acc = Immutable_dynamic_array.(singleton Term_types.Memory,
                                             singleton a.mem, singleton b.mem) in
          let (types,a',b') = VarMap.fold (fun vi (a,b) (types,a',b') ->
              Immutable_dynamic_array.(append types (Term_types.Binary ptr_size),
                                       append a' a, append b' b)
            ) common_varaddr acc
          in
          let res = SmartCons.Tuple.nondet ~level:(!loop_nesting) types
              [SmartCons.Tuple.tuple_create a'; SmartCons.Tuple.tuple_create b'] in
          let var_addresses,n = VarMap.fold (fun vi _ (map,i) ->
              let ab = SmartCons.Tuple.tuple_get res i in
              let map = VarMap.add vi ab map in
              map, i + 1) common_varaddr (a.env.var_addresses,1) in
          assert(n == Immutable_dynamic_array.length a');
          let mem = SmartCons.Tuple.tuple_get res 0 in
          let string_adresses =
            assert(a.env.string_adresses == b.env.string_adresses);
            a.env.string_adresses
          in
          Some_Memory{mem; env = {string_adresses; var_addresses}}
  ;;

  let join l = match l with
    | [x] -> x
    | a::b -> List.fold_left join2 a b
    | [] -> Bottom_Memory
  ;;
  
  let mu f mem_arg = 
    let open Lang.Memory in
    match mem_arg with
    | Bottom_Memory -> Bottom_Memory
    | Some_Memory mem_arg -> begin

      let arg =
        SmartCons.Tuple.tuple_create (Immutable_dynamic_array.singleton mem_arg.mem)
      in

      let var =
        let var = SmartCons.Tuple.var ~level:(!loop_nesting + 1) in
        SmartCons.Tuple.append_type var Memory;
        var
      in

      let mem_var = (SmartCons.Tuple.tuple_get var 0) in

      (* Pushing new counters for time. *)
      incr loop_nesting;
      let mem_body = f @@ Some_Memory{mem=mem_var;env=mem_arg.env} in
      decr loop_nesting;
      (* Popping it. *)
      
      match mem_body with
      | Bottom_Memory -> Some_Memory mem_arg
      | Some_Memory mem_body -> begin
          assert(VarMap.equal (==) mem_body.env.var_addresses mem_arg.env.var_addresses);
          let body = SmartCons.Tuple.tuple_create (Immutable_dynamic_array.from_array [| mem_body.mem |]) in
          Some_Memory{mem=SmartCons.Tuple.tuple_get (SmartCons.Tuple.mu_explicit ~level:(!loop_nesting) ~var ~body ~arg) 0;env=mem_arg.env}
        end
      end
  ;;

  let compile_node stmt input_mem =
    let open Lang.Memory in
    match input_mem with
    | Bottom_Memory -> assert false
    | Some_Memory input_mem ->
      current_stmt := Cil_types.Kstmt stmt;
      let answer = Translator.statement input_mem.env ~mem:(Some_Memory input_mem) stmt in
      let cont_mems = answer in
      cont_mems
  ;;
end

(* Compute the type of the argument to the function. *)
let compute_initial_var kf =
  let formals = Kernel_function.get_formals kf in
  let args = formals |> List.map (fun vi ->
      let size = Cil.bitsSizeOf vi.Cil_types.vtype in
      SmartCons.Binary.bunknown_explicit ~size ~level:0
    ) |> Array.of_list in
  Lang.Memory.unknown ~level:0, args
;;

(* Creates a memory suitable for use in the function (store the
   function argument in the C formal variables). *)
let compile_function_entry kf initial_mem arg =
  let formals = Kernel_function.get_formals kf in
  let (memory,_) = List.fold_left (fun (acc,i) vi ->
      let acc = Lang.allocate_var acc vi @@ Some (fun ~size -> Array.get arg i)  in
    (acc,i+1)) (initial_mem,0) formals in
  let body = Cil_types.((Kernel_function.get_definition kf).sbody) in
  Lang.compile_block_entry memory body
;;

let compile_function_exit ret_stmt kf mem =
  (* Note: the return may not be in the outermost block. *)
  let blocks = Kernel_function.find_all_enclosing_blocks ret_stmt in
  let mem = List.fold_left Lang.compile_block_close mem blocks in
  
  let formals = Kernel_function.get_formals kf in
  let mem = List.fold_left (fun mem vi -> Lang.deallocate_var mem vi) mem formals in
  mem
;;
  

let compile kf =
  let initial_mem, args = compute_initial_var kf in

  current_stmt := Cil_types.Kglobal;
  loop_nesting := 0;
  callstack := [];

  let setup = compile_function_entry kf initial_mem args in
  
  let module Node = (Region_analysis_stmt.MakeNode(struct
    let kf = kf
    include Transfer_functions
  end)) in
  let module Apply = Region_analysis.Make(Node) in

  let after = Apply.after setup in
  let return = Kernel_function.find_return kf in
  let [exit_term] = Node.Edge_Dict.get after (Exit return) in
  compile_function_exit return kf exit_term
;;
(* Note: [after] is also a result of compilation; the return term is
   not the only interesting one.

   Actually, for every assertion in the original program, we should
   map it to a Codex term (evaluating this term allows to know the
   status of alarms). And also perhaps for every statement and
   expression. *)

(****************************************************************)
(* Special version that inline calls. *)

(* TODO: Term.t Immutable_dynamic_array.t -> Term.t
   Immutable_dynamic_array.t: we return several arguments and there is
   no need to put them in a single term.

   The input array has the following structure: position 0 is the memory;
   remaining positions contain the arguments.

   For the output, position 0 is the memory, and position 1 the
   eventual return binary.

*)

let rec compile_inline kf mem args =

  Kernel.feedback "compiling %a" Callstack.pretty !callstack;
  let setup = compile_function_entry kf mem args in

  let module Transfer_functions_inline = struct
    include Transfer_functions;;

    open Cil_types

    (* TODO: Compilation of builtins should be done here. *)
    (* TODO: Depending on the return type of the function, we should have two unknowns:
       unknown_ptr and unknown_int. *)
    let is_builtin x = Translator.is_builtin
      (* List.mem x ["malloc";"free";"alloca";"__fc_vla_alloc";"__fc_vla_free";"exit"; *)
      (*             "Frama_C_bzero";"calloc";"realloc"; *)
      (*             "__VERIFIER_assert"; "__VERIFIER_error"; "__VERIFIER_assume"] *)
    ;;

    (* For each function that returns something, we must implement
       both the Call and the Local_init version. *)
    let malloc ret f args instr_loc stmt input_mem =
      let open Lang.Memory in
      let sizeexp = match args with
        | [x] -> x
        | _ -> failwith "Wrong number arguments to malloc"
      in
      let malloc_size = match Cil.constFoldToInt ~machdep:true sizeexp with
        | Some size -> Integer.to_int size
        | None -> assert false
      in
      let id = Codex.Transfer_functions.Malloc_id.fresh ("malloc" ^ string_of_int stmt.sid) in
      let ptr,Some_Memory mem = Lang.Memory.malloc ~id ~malloc_size input_mem in
      let mem = match ret with
        | None -> Some_Memory mem
        | Some lval ->
          let value = ptr in
          Translator.store_lvalue ~instr_loc mem.env lval value (Term.size_of value) (Some_Memory mem)
      in
      let [succ] = stmt.succs in
      Lang.blocks_crossing @@ [(Edge(stmt,succ),mem)]
    ;;

    let free ret f args instr_loc stmt mem =
      let arg = match args with
        | [x] -> x
        | _ -> failwith "Wrong number of arguments to free"
      in
      let (arg,mem) = run mem Translator.expression arg in
      let mem = Lang.Memory.free mem arg in
      let [succ] = stmt.succs in          
      Lang.blocks_crossing @@ [(Edge(stmt,succ),mem)]
    ;;

    let exit ret f args instr_loc stmt mem =
      let mem = Lang.Memory.Bottom_Memory in
      (* If exit is properly declared with __no_return attribute,
             there should be no successor, but it is not always the case. *)
      stmt.succs |> List.map (fun succ -> Edge(stmt,succ),mem) |> Lang.blocks_crossing
    ;;

    let assume ret f args instr_loc stmt mem =
      let arg = match ret,args with
        | None, [x] -> x
        | _ -> failwith "Wrong number of arguments/return value to assume"
      in
      let (cond,mem) = run mem Translator.cond_node arg in
      let mem = Lang.Memory.assume cond mem in
      let [succ] = stmt.succs in
      Lang.blocks_crossing @@ [(Edge(stmt,succ),mem)]
    ;;

    let verifier_error ret f args instr_loc stmt mem =
      let Lang.Memory.Some_Memory mem = mem in
      let [] = args in
      let None = ret in
      if !loop_nesting == 0 then
        Reachable_Table.register reachable_table mem.mem;
      (* If __VERIFIER_error was properly declared with __no_return attribute, 
             there should be no successor, but we never know. *)
      stmt.succs |> List.map (fun succ -> Edge(stmt,succ),Lang.Memory.Bottom_Memory)
      |> Lang.blocks_crossing
    ;;      

    let verifier_assert ret f args instr_loc stmt mem =      
      let [arg] = args in
      let None = ret in
      let (assertion,mem) =
        run mem Translator.cond_node arg in
      if !loop_nesting == 0 then begin
        Assertion_Table.register assert_table assertion (Assertion_Table.SVComp instr_loc);
        (* Kernel.feedback "registering term %a " Term.pretty assertion; *)
      end;
      let [succ] = stmt.succs in
      Lang.blocks_crossing @@ [(Edge(stmt,succ),mem)]
    ;;
    
    let bzero ret f args instr_loc stmt mem =
      let arg1,arg2 = match ret,args with
        | None, [arg1;arg2] -> arg1,arg2
        | _ -> failwith "Wrong number of arguments/return value to bzero"
      in
      let size = match Cil.constFoldToInt ~machdep:true arg2 with
        | Some int -> int
        | None -> failwith "bzero called with non-constant size not handled" in
      let (address,mem) =
        run mem Translator.expression arg1 in
      let size = Integer.to_int size in
      let size = size * 8 in
      let zero = SmartCons.Binary.biconst ~size Z.zero in
      (* Write a big chunk of zero. *)
      let mem = Lang.Memory.store ~size mem address zero in
      let [succ] = stmt.succs in
      Lang.blocks_crossing @@ [(Edge(stmt,succ),mem)]
    ;;
      
    let show_each ret f args instr_loc stmt mem =
      if (ret <> None) then failwith "Frama_C_show_each does not return a value";

      (* MAYBE: As this is more a debug function, maybe we should not change mem and state. *)
      let args,Lang.Memory.Some_Memory mem =
        List.fold_left (fun (acc,mem) arg ->
            let (arg,mem) =
              run mem Translator.expression arg in
            arg::acc,mem) ([],mem) args
      in
      let args = List.rev args in
      let tuple = Immutable_dynamic_array.from_array (Array.of_list (mem.mem::args)) in
      let len = (String.length "Frama_C_show_each") in
      let suffix = String.sub f.vname len  ((String.length f.vname) - len) in
      let nexttuple = SmartCons.Tuple.builtin (Builtin.Show_each_in suffix) @@ SmartCons.Tuple.tuple_create tuple in
      let nextmem = Lang.Memory.Some_Memory{mem=SmartCons.Tuple.tuple_get nexttuple 0;
                                            env=mem.env} in
      let [succ] = stmt.succs in
      Lang.blocks_crossing @@ [(Edge(stmt,succ),nextmem)]
    ;;

    let not_handled ret f args instr_loc stmt m =
      failwith ("Function not handled: " ^ f.vname)
    ;;
    

    (* Calling + inlining a function. *)
    let call_sub ret f args instr_loc stmt mem =
      assert (Kernel_function.is_definition (Globals.Functions.get f));

      (* Prepare the call. *)
      let array = Array.make ((List.length args)) (SmartCons.Boolean.false_) in
      let (_,mem) = List.fold_left (fun (i,mem) arg ->
          let (binary,mem) =
            run mem Translator.expression arg in
          array.(i) <- binary;
          (i+1,mem)) (0,mem) args in

      let called_kf = Globals.Functions.get f in
      let [succ] = stmt.succs in (* May not be the case if there is a noreturn attribute? *)
      let tuple_arg = array in

      let old_callstack = !callstack in
      let new_callstack = (called_kf,Kstmt stmt)::old_callstack in
      callstack := new_callstack;
      let retmem,retval = compile_inline called_kf mem tuple_arg in
      current_stmt := Kstmt stmt;
      callstack := old_callstack;

      let mem = match retmem with
        | Lang.Memory.Bottom_Memory -> Lang.Memory.Bottom_Memory
        | Lang.Memory.Some_Memory mem ->
          (match ret,retval with
           | None, _ -> retmem
           | Some _, None -> Codex_log.fatal "Expected a return value, but the function does not return one."
           | Some lval, Some value ->
             Translator.store_lvalue ~instr_loc mem.env lval value
               (Term.size_of value) (Lang.Memory.Some_Memory mem))
      in
      Lang.blocks_crossing @@ [(Edge(stmt,succ),mem)]
    ;;



    
    let compile_node stmt (input_mem:Lang.Memory.memory) =
      (* let open Lang.Memory in *)
      match input_mem with
      | Lang.Memory.Bottom_Memory ->
        let next = stmt.succs |> List.map @@ fun succ -> (Edge(stmt,succ),Lang.Memory.Bottom_Memory) in
        Lang.blocks_crossing @@
        (match stmt.skind with Return _ -> (Exit stmt, Bottom_Memory)::next | _ -> next)
      | Lang.Memory.Some_Memory _ -> 

      (* let state = !r_state in *)
      current_stmt := Kstmt stmt;
      let res = match stmt.skind with
        (* SVComp-Style assertion. *)
        (* In the SVComp competition, this function calls verifier_error, but not in my 
           earlier tests.
           TODO: Change my earlier tests instead, to use ACSL invariants. *)
        | Instr(Call(ret,{enode=Lval(Var(vi),NoOffset)},args,loc))
          when vi.vname = "__VERIFIER_assert"
            && (try let _ = Globals.Functions.find_def_by_name "__VERIFIER_assert" in false
                with Not_found -> true) ->
          verifier_assert ret vi args loc stmt input_mem
        | Instr(Call(ret,{enode=Lval(Var(vi),NoOffset)},args,loc))
          when vi.vname = "__VERIFIER_error" ->
          verifier_error ret vi args loc stmt input_mem
        | Instr(Call(ret,{enode=Lval(Var(vi),NoOffset)},args,loc))
          when vi.vname = "__VERIFIER_assume" ->
          assume ret vi args loc stmt input_mem
        | Instr(Call(ret,{enode=Lval(Var(vi),NoOffset)},args,loc))
          when vi.vname = "exit" ->
          exit ret vi args loc stmt input_mem
        | Instr(Call(ret,{enode=Lval(Var(vi),NoOffset)},args,loc))
          when vi.vname = "alloca" ->
          not_handled ret vi args loc stmt input_mem
        | Instr(Call(ret,{enode=Lval(Var(vi),NoOffset)},args,instr_loc))
          when vi.vname = "malloc" || vi.vname = "__fc_vla_alloc" ->
          malloc ret vi args instr_loc stmt input_mem
        | Instr(Local_init(ret,ConsInit(f,args,_),instr_loc))
          when f.vname = "malloc" || f.vname = "__fc_vla_alloc" ->
          malloc (Some(Var(ret),NoOffset)) f args instr_loc stmt input_mem
        | Instr(Call(ret,{enode=Lval(Var(vi),NoOffset)},args,loc))
          when vi.vname = "calloc" ->
          not_handled ret vi args loc stmt input_mem
        | Instr(Call(ret,{enode=Lval(Var(vi),NoOffset)},args,loc))
          when vi.vname = "realloc" ->
          not_handled ret vi args loc stmt input_mem
        | Instr(Call(ret,{enode=Lval(Var(vi),NoOffset)},args,instr_loc))
          when vi.vname = "free" || vi.vname = "__fc_vla_free" ->
          free ret vi args instr_loc stmt input_mem
        (* Frama-C builtins. *)
        | Instr(Call(ret,{enode=Lval(Var(vi),NoOffset)},args,loc))
          when Extlib.string_prefix "Frama_C_show_each" vi.vname ->
          show_each ret vi args loc stmt input_mem
        | Instr(Call(ret,{enode=Lval(Var(vi),NoOffset)},args,loc))
          when vi.vname = "Frama_C_bzero" ->
          bzero ret vi args loc stmt input_mem
            
        | Instr(Call(ret,{enode=Lval(Var(f),NoOffset)},args,instr_loc))
          when (Kernel_function.is_definition (Globals.Functions.get f)) ->
          call_sub ret f args instr_loc stmt input_mem
        | Instr(Local_init(ret,ConsInit(f,args,_),instr_loc))
          when (Kernel_function.is_definition (Globals.Functions.get f)) ->
          call_sub (Some(Var(ret),NoOffset)) f args instr_loc stmt input_mem

        | _ ->
          let res = compile_node stmt input_mem in
          res
      in
      res
  end in
  let module Node = (Region_analysis_stmt.MakeNode(struct
    let kf = kf
    include Transfer_functions_inline
  end)) in
  let module Apply = Region_analysis.Make(Node) in

    (* Note: the region analysis used to be done only once per function;
       with the assertion table and world, this is not true anymore.*)
  let after = Apply.after setup in
  let return = Kernel_function.find_return kf in
  let [exit_mem] = Node.Edge_Dict.get after (Exit return) in

  let Cil_types.Return(retexp,_) = return.Cil_types.skind in
  let exit_mem,retvalue = 
    match retexp with
    | None -> (exit_mem,None)
    | Some exp ->
      let (binary,mem) =
        match exit_mem with
        | Bottom_Memory -> Lang.Binary.bottom ~size:(Cil.bitsSizeOf @@ Cil.typeOf exp), Lang.Memory.Bottom_Memory
        | Some_Memory _ ->
          run exit_mem Translator.expression exp in
      (mem,Some binary)
  in compile_function_exit return kf exit_mem,retvalue
;;

(* Given a memory term, returns a memory term where the initial values
   of global variables have been written. If libentry, only 
   const variables are written.  *)
let write_initial_state ~libentry mem globals_used = 
  let module Memory = Lang.Memory in
  let module Binary = SmartCons.Binary in
  let open Cil_types in

  (* We need two passes: because they may be mutually referencing each
     other, we must first allocate the variables, and then write
     pointer to each others. But to avoid writing "unknown" that would
     be immediately overwritten, we try to immediately write the
     correct value when feasible. Thus we try to process the
     dependencies of a variable before this variable, and separate
     allocation only for recursively-defined global variables. *)

  let module VarSet = Cil_datatype.Varinfo.Set in

  (* Note: I could just allocate everything, and then initialize everything,
     it would be simpler. *)
  (* First pass: allocate all the variables, and initialize when
     feasible. *)
  let do_global var (mem,allocated_vars,initialized_vars) =

    (* Const values are always known, except when volatile. Arrays of
       consts are also known. *)
    let rec is_const typ =
      (Cil.typeHasAttribute "const" typ
       && not (Cil.typeHasQualifier "volatile" typ))
      || match typ with
      | TArray(typ,_,_,_) -> is_const typ
      | _ -> false
    in

    let unknown_initial_value =
      (* Extern declarations are always unknown *)
      (var.vstorage == Extern && not var.vdefined)
      (* We could be more precise here: if there is const somewhere in
         a subtype, then some part of the value may be known (e.g. in
         a array of structs, some fields could be const). *)      
      || (libentry && not (is_const var.vtype)) in
    let {init} = Globals.Vars.find var in
    let init_unknown ~size = Binary.bunknown_explicit ~size ~level:0 in
    let init_zero ~size = Binary.biconst ~size Z.zero in
    let allocated_vars = VarSet.add var allocated_vars in
    (* MAYBE: Be smart, and initialize variables on which we depend,
       when this avoids writing unknown. Probably not worth it. *)
    let initial_valuef,initialized_vars =
      if unknown_initial_value
      then Some init_unknown, VarSet.add var initialized_vars
      else match init with
      | None -> Some init_zero, VarSet.add var initialized_vars
      | Some(SingleInit e) -> begin
          match Cil.constFoldToInt ~machdep:true e with
          | Some i ->
            Some (fun ~size -> Binary.biconst ~size (Translator.z_of_integer i)),
            VarSet.add var initialized_vars
          | None -> (* Some init_unknown *)None, initialized_vars (* Not initialized. *)
        end
      | Some(CompoundInit _) ->
        (* We do not initialize, as foldLeftCompound ~implicit:true
           adds the missing elements. *)
        (* Some init_zero*) None, initialized_vars
    in
    let mem = Lang.allocate_var mem var initial_valuef in
    mem,allocated_vars,initialized_vars
  in

  let rec loop1 ((mem,allocated_vars,initialized_vars) as acc) =
    let todo = VarSet.diff globals_used allocated_vars in
    if VarSet.is_empty todo then acc
    else
      let acc = VarSet.fold do_global todo acc in
      loop1 acc
  in
  let (mem,allocated_vars,initialized_vars) = loop1 (mem,VarSet.empty,VarSet.empty) in
  
  (* Second pass: all the variables where allocated; initialize the
     remaining ones. *)
  let initialize_var var mem =
    let {init} = Globals.Vars.find var in
    let init = match init with None -> assert false | Some x -> x in
    
    (* Use the memory as the accumulator. Also maintains lv, that
             cannot go in the accumulator. *)
    let rec doit lv init typ mem =
      let size = Cil.bitsSizeOf typ in
      match init with
      | SingleInit(e) ->
        (* Note: as these should be constant expressions and
           lvalues, mem should not change. But everything should
           still work if we were analyzing C++. *)
        let (value,mem) = run mem  Translator.expression e in
        let (address,mem) = run mem Translator.lvalue lv in
        assert(address.bitfield == None);
        let mem = Memory.store ~size mem address.address value in
        (mem)
      | CompoundInit(ct,initl) -> 
        let doinit off init typ acc = doit (Cil.addOffsetLval off lv) init typ acc in
        (* Note: [implicit:true] handles incomplete initializers for us. *)
        Cil.foldLeftCompound
          ~implicit:true
          ~doinit ~ct ~initl ~acc:(mem)
    in
    doit (Var var, NoOffset) init var.vtype mem
  in

  let mem = VarSet.fold initialize_var (VarSet.diff allocated_vars initialized_vars) mem in
  mem

;;

let initialize_strings strings_used mem =
  let open Lang in
  let open Lang.Memory in
  Datatype.String.Set.fold (fun str mem ->
      let malloc_size = (String.length str + 1) in
      let size = 8 * malloc_size in
      let id = Codex.Transfer_functions.Malloc_id.fresh str in
      let addr,mem = Memory.malloc ~id ~malloc_size mem in
      let to_store =
        let bitvector =
          (* big-endian representation of integers. *)
          if false then 
            let rec loop acc i =
              if i < String.length str then
                let char = String.get str i in
                let value = Char.code char in
                let acc = Z.(lor) (Z.shift_left acc 8) (Z.of_int value) in
                loop acc (i + 1)
              else Z.shift_left acc 8 (* trailing zero *)
            in loop Z.zero 0 
          else
          (* little-endian one. *)
            let rec loop acc i =
              if i < 0 then acc
              else
                let char = String.get str i in
                let value = Char.code char in
                let acc = Z.(lor) (Z.shift_left acc 8) (Z.of_int value) in
                loop acc (i - 1)
            in loop Z.zero (String.length str - 1)
        in
        Lang.Binary.biconst ~size bitvector
      in
      let mem = Memory.store ~size mem addr to_store in
      let mem = match mem with
        | Bottom_Memory -> Bottom_Memory
        | Some_Memory mem ->
          Some_Memory{mem with env = {mem.env with string_adresses =
                                                     StringMap.add str addr mem.env.string_adresses}}
      in mem
    ) strings_used mem
;;

let initialize_function_ptrs functions_used mem =
  (* TODO: This relies on the fact that size of functions is defined,
     which is not the case in MSVC. *)
  Cil_datatype.Varinfo.Set.fold (fun f mem ->
      Lang.allocate_var mem f @@ Some (fun ~size -> Lang.Binary.bunknown_explicit ~size ~level:0)
    ) functions_used mem
;;


let compile kf =

  let module P = PreAnalysis(struct let main = kf end) in
  let (functions_used, globals_used, strings_used) = P.result in
  
  let mem,args = compute_initial_var kf in

  loop_nesting := 0;
  callstack := [kf,Cil_types.Kglobal];
  current_stmt := Cil_types.Kglobal;

  let mem = initialize_strings strings_used mem in
  let mem = initialize_function_ptrs functions_used mem in
  let mem = write_initial_state ~libentry:(Kernel.LibEntry.get()) mem globals_used in

  let memterm = 
    let resultmem,_ = compile_inline kf mem args in
    match resultmem with
    | Lang.Memory.Bottom_Memory -> None
    | Lang.Memory.Some_Memory mem -> Some mem.mem
  in
  let find_in_exptrie (ki,exp) = ExpInStmt.Hashtbl.find exptrie (ki,exp) in
  (memterm,assert_table,reachable_table,find_in_exptrie)
;;

end



let compile kf =
  let module M = Make() in
  M.compile kf
;;
