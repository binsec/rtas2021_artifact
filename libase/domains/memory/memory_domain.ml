(**************************************************************************)
(*                                                                        *)
(*  This file is part of Codex.                                           *)
(*                                                                        *)
(*  Copyright (C) 2007-2017                                               *)
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

(* A memory domain using the modules corresponding to Memory_sig. The
   main requirement here is that the values in memory correspond to
   the addresses. *)


(* MAYBE: put with_bottom here. This would handle the cases where we
   return memory_empty which is not catched first by an alarm. *)
module Make
    (Scalar:Domain_sig.Base)
    (Operable_Value:Memory_sig.Operable_Value_Whole with module Scalar = Scalar)
    (Whole_Memory:Memory_sig.Whole_Memory
     with module Scalar = Scalar
      and type Value.binary = Operable_Value.binary
      and type address = Operable_Value.binary)
  :sig
    include Domain_sig.Base
      with type binary = Operable_Value.binary
      and type boolean = Scalar.boolean
      and module Context = Operable_Value.Scalar.Context
      and module Binary_Forward = Operable_Value.Binary_Forward
  end
= struct

  let name = "Memory_domain(" ^ Scalar.name ^ ")";;
  let unique_id = Domain_sig.Fresh_id.fresh name;;

  type size = int;;  
  type boolean_mapping = int
  type integer_mapping = int
  
  module Types = struct
    type boolean = Scalar.boolean
    type integer = Scalar.integer

    type binary = Operable_Value.binary

    type memory = {
      cond: boolean;
      mem:  Whole_Memory.memory
    }
  end
  include Types

  module Integer = Scalar.Integer
  module Boolean = Scalar.Boolean                     
  module Binary = Operable_Value.Binary

  (**************** Context ****************)

  type mu_context = Scalar.mu_context
  module Context = Scalar.Context

  let root_context_upcast = Scalar.root_context_upcast
  let root_context = Scalar.root_context
  type root_context = Scalar.root_context

  (**************** Pretty printing ****************)

  let memory_pretty ctx fmt mem = (Whole_Memory.pretty ctx) fmt mem.mem

  let binary_pretty ~size ctx fmt x = Operable_Value.binary_pretty ~size ctx fmt x
  let boolean_pretty ctx fmt = Scalar.boolean_pretty ctx fmt
  let integer_pretty ctx fmt = Scalar.integer_pretty ctx fmt

  (**************** Tuple fonctions ****************)

  let typed_serialize_mem: Scalar.Context.t -> memory -> memory -> 'a Scalar.in_acc -> (memory,'a) Scalar.result =
    fun ctx a b (included, acc) ->
      let equal_cond = Scalar.Boolean.equal a.cond b.cond in
      let Scalar.Result(included,acc,d_cond) =
        (* Note: equal_cond is probably rarely true. *)
        if equal_cond then Scalar.Result(included,acc,fun x -> a.cond,x)
        else Scalar.serialize_boolean ctx a.cond b.cond (included,acc)
      in
      let Scalar.Result(included,acc,d_mem) = Whole_Memory.serialize ctx a.mem b.mem
        (included, acc) in
      Scalar.Result(included,acc,fun x ->
          let mem,x = d_mem x in          
          let cond,x = d_cond x in
          (* Codex_log.feedback "Deserializing mem %a" (memory_pretty ctx) {cond;mem};
           * Codex_log.feedback "Deserializing cond %a" (boolean_pretty ctx) cond;
           * Codex_log.feedback "Old consds %a %a" (boolean_pretty ctx) a.cond (boolean_pretty ctx) b.cond; *)
          {cond;mem},x
        )
  ;;
                                
  let memory_empty ctx = {
      mem = Whole_Memory.memory_empty ctx;
      cond = Scalar.Boolean_Forward.true_ ctx;
    }
  ;;

  (**************** Fixpoint computation ****************)

  let mu_context_upcast = Scalar.mu_context_upcast
  let mu_context_open = Scalar.mu_context_open

  (* Fixpoint computation and nondet. *)
  
  type 'a in_tuple = 'a Scalar.in_tuple
  type 'a in_acc = 'a Scalar.in_acc
  type 'a out_tuple = 'a Scalar.out_tuple

  type empty_tuple = Scalar.empty_tuple
  let empty_tuple = Scalar.empty_tuple

  type ('a,'b) result = ('a,'b) Scalar.result = Result: bool * 'some in_tuple * ('some out_tuple -> 'a * 'b out_tuple) -> ('a,'b) result    

  let serialize_memory ctx = typed_serialize_mem ctx;;
  let serialize_binary ~size ctx = Operable_Value.serialize ~size ctx
  let serialize_integer ctx = Scalar.serialize_integer ctx
  let serialize_boolean ctx = Scalar.serialize_boolean ctx

  let typed_fixpoint_step ctx = Scalar.typed_fixpoint_step ctx
  let typed_nondet2 ctx = Scalar.typed_nondet2 ctx

  let assume_memory ctx newcond mem =
    let cond = Scalar.assume_boolean ctx newcond mem.cond in
    (* This is already done in the levels above. *)
    (* (match Scalar.Query.convert_to_quadrivalent @@ Scalar.Query.boolean ctx cond
     *   with
     *   | Quadrivalent_Lattice.False | Quadrivalent_Lattice.Bottom -> assert false
     *   | _ -> *) {mem with cond}

  let assume_binary = Operable_Value.assume
  let assume_boolean = Scalar.assume_boolean
  let assume_integer = Scalar.assume_integer

  
  (**************** Transfer functions. ****************)

  module Boolean_Forward = Scalar.Boolean_Forward
  module Binary_Forward = Operable_Value.Binary_Forward
  module Integer_Forward = Scalar.Integer_Forward

  module Memory_Forward = struct
    let assume = assume_memory;;

    let load ~size ctx mem at =
      (* Codex_log.feedback "load %a" (binary_pretty ~size ctx) at; *)
        let res = Whole_Memory.load ~size ctx mem.mem at in
        assume_binary ~size ctx mem.cond res
    ;;

    (* TODO: Implement memcpy. *)
    let memcpy ~size ctx mem from to_ = assert false
    
    let store ~(size:int) ctx mem (at:binary) (value:binary) =
      (* Codex_log.feedback "################storing %a" (Operable_Value.binary_pretty ~size ctx) at; *)
      try
        { mem = Whole_Memory.store ~size ctx mem.mem at value;
          cond = mem.cond }
      with Memory_sig.Memory_Empty -> memory_empty ctx 
    ;;

    let _store ~(size:int) ctx mem (at:binary) (value:binary) =
      let res = store ~size ctx mem at value in
        Codex_log.feedback "AFter store %a <- %a on@ %a:@\nResult= %a@."
          (binary_pretty ~size ctx) at (binary_pretty ~size ctx) value
          (memory_pretty ctx) mem (memory_pretty ctx) res;
        res
    ;;
          
    
    
    let malloc ~id ~malloc_size ctx mem =
      let ptr,mem' = Whole_Memory.malloc ~id ~malloc_size ctx mem.mem in
      ptr, {mem=mem'; cond = mem.cond}
    ;;

    let free ctx mem ptr =
      { cond = mem.cond;
        mem = Whole_Memory.free ctx mem.mem ptr
      }
    ;;

    let unknown ~level ctx =
      (* assert(level == ctx.level); *)
      { cond = Boolean_Forward.true_ ctx;
        mem = Whole_Memory.unknown ~level ctx
      }
  end

  (**************** Queries ****************)

  module Query = struct
    include (Scalar.Query:module type of Scalar.Query
             with module Boolean_Lattice = Scalar.Query.Boolean_Lattice
              and module Integer_Lattice = Scalar.Query.Integer_Lattice
              and module Binary_Lattice := Scalar.Query.Binary_Lattice)           
    module Binary_Lattice = Operable_Value.Query.Binary_Lattice
    let reachable ctx mem =
      Scalar.Query.convert_to_quadrivalent @@ boolean ctx mem.cond
    ;;
    let binary ~size ctx x = Operable_Value.Query.binary ~size ctx x
    let binary_to_ival ~signed ~size x = Operable_Value.Query.binary_to_ival ~size ~signed x
    let binary_to_known_bits ~size x = Operable_Value.Query.binary_to_known_bits ~size x        
    let binary_fold_crop = Operable_Value.Query.binary_fold_crop
    let binary_is_empty = Operable_Value.Query.binary_is_empty                             
    let binary_is_singleton ~size x = Operable_Value.Query.binary_is_singleton ~size x        
  end

  let memory_is_empty _ = assert false
  let binary_is_empty ~size _ = assert false
  let integer_is_empty _ = assert false
  let boolean_is_empty _ = assert false

  (**************** Builtins. ****************)

  include Builtin.Make(Types)(Context)

  let builtin_show_each_in s ctx args mem =
    let pp fmt =
      Format.fprintf fmt "Finite_memory: show each%s:@[<hv>" s;
      args |> List.iter (fun (size,x) ->
          Format.fprintf fmt "%a@\n" (Operable_Value.binary_pretty ~size ctx) x
        );
      Format.fprintf fmt "@]"
    in
    Codex_log.feedback "%t" pp;
    mem
  ;;

  let binary_empty ~size ctx = Operable_Value.binary_empty ~size ctx

  let reachable ctx mem = Scalar.satisfiable ctx mem.cond;;
  let satisfiable ctx b = Scalar.satisfiable ctx b

  let integer_empty ctx = Scalar.integer_empty ctx
  let boolean_empty ctx = Scalar.boolean_empty ctx      

  let binary_unknown = Operable_Value.binary_unknown
  let integer_unknown = Scalar.integer_unknown
  let boolean_unknown = Scalar.boolean_unknown

  
  let memory_pretty ctx = memory_pretty ctx

  let union _ = assert false
                     

  let should_focus : size:int -> Scalar.Context.t -> memory -> binary -> (binary * int * int) option = fun ~size ctx memory address ->
    Whole_Memory.should_focus ~size ctx memory.mem address

  let may_alias : size:int -> Scalar.Context.t -> binary -> binary -> bool = fun ~size ctx addr1 addr2 ->
    Whole_Memory.may_alias ~size ctx addr1 addr2
end
