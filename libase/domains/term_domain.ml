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

(* MAYBE: Term_smart_constructors should be subsumed by this.  Or at
   least should have the same interface. *)

(* Note: there is no top operators for the term lattices.  Instead, we
   use "var", that introduces a new variable (existentially), and
   gives thus more information (i.e. allows to detect equality). *)

(* Note: a widening operation could be defined on terms: we take two
   term t1 and t2, with t2 built from t1, and we create a "mu" node
   with t1 as the initial argument, and the part in t2 not in t1 as
   the loop body. But it does not interacts well with the
   simplifications done by the smart constructors. The "mu" operator,
   that tells when we enter a loop, is thus better for terms. *)

(* module Make() = *)
include struct

  module SmartCons = Term_smart_constructors;;


  module Type = struct
    type boolean = Term_type.term
    type integer = Term_type.term
    type binary = Term_type.term
    type memory = Term_type.term
  end
  include Type

  type 'a mu_context_status =
    | Ready
    | After_fixpoint_step of 'a (* Store intermediate results between fixpoint_step and close *)
    | After_close
      

  type mu_context = {
    mu_ctx: context;              (* Context corresponding to this mu_context *)
    mu_parent_ctx: context;       (* Parent of the current context. *)
    mutable mu_status: Term.t mu_context_status;
  }

  and root_context = {
    root_ctx: context;            (* Context corresponding to this root_context *)
  }

  and context_ =
    | Mu_context of mu_context
    | Root_context of root_context

  and context =
    { ctx:context_;
      binding_depth: int; 
    }
  ;;

  let level ctx = ctx.binding_depth;;
  
  module Context = struct
    type t = context
    let level = level
  end

  include Builtin.Make(Type)(Context)  

  module Ref_Addr = Ref_Addr

  module Convert = struct
    module From_Arity = Basis_sig.Pure_Arity.Forward
    module To_Arity = Domain_sig.Context_Arity_Forward(Context)
    let g x = fun _ -> x
    let ar0 = g
    let ar1 = g
    let ar2 = g
    let ar3 = g
    let variadic = g
  end

  module Boolean = struct
    include Term_type.Term_datatype
    let name = "term_domain_boolean"
  end
  module Boolean_Forward = Transfer_functions_conversions.Convert_Boolean_Forward(Convert)(SmartCons.Boolean)

  module Integer = struct
    include Term_type.Term_datatype
    let name = "term_domain_integer"
  end
  module Integer_Forward = struct
    include Transfer_functions_conversions.Convert_Integer_Forward(Convert)(SmartCons.Integer)
    let iunknown () ctx =
      SmartCons.Integer.iunknown_explicit ~level:(level ctx)
  end

  module Binary = struct
    include Term_type.Term_datatype
    let name = "term_domain_binary"
  end
  module Binary_Forward = struct
    include Transfer_functions_conversions.Convert_Binary_Forward(Convert)(SmartCons.Binary)
    let bunknown ~size ctx =
      SmartCons.Binary.bunknown_explicit ~size ~level:(level ctx)
  end

  module Memory = struct
    include Term_type.Term_datatype
    let name = "term_domain_memory"
  end
  module Memory_Forward = struct
    include Transfer_functions_conversions.Convert_Memory_Forward(Convert)(SmartCons.Memory)
  end

  open Term_constructors;;
  open Term_types;;

  let tuple_is_empty _ t = match t.kind with
    | Nondet(_,_,x) when Term_type.Term_set.is_empty x -> true
    | _ -> false

  let elt_is_empty _ t = match t.kind with
    | Tuple_get(x,_) when tuple_is_empty () x -> true
    | _ -> false

  let memory_is_empty = elt_is_empty
  let binary_is_empty ~size = elt_is_empty
  let boolean_is_empty = elt_is_empty

  let truth_value _ t =
    let open Term_constructors in
    let open Domain_sig in
    match t.kind with
    | True -> Quadrivalent.True
    | False -> Quadrivalent.False
    | _ when boolean_is_empty () t -> Quadrivalent.Bottom
    | _ -> Quadrivalent.Top


  let memory_empty = SmartCons.Memory.bottom
  let binary_empty ~size _ = SmartCons.Binary.bottom ~size
  let integer_empty _ = SmartCons.Integer.bottom
  let boolean_empty _ = SmartCons.Boolean.bottom

  let binary_unknown ~size ~level ctx = SmartCons.Binary.bunknown_explicit ~size ~level
  let integer_unknown ~level ctx = SmartCons.Integer.iunknown_explicit ~level
  let boolean_unknown ~level ctx = (* SmartCons.Boolean. *)assert false

  
  let builtin b ~from_types ~to_types ctx tup = assert false
    (* SmartCons.Tuple.builtin b tup *)
  
  let tuple_create _ arr = assert false
    (* let array = Immutable_dynamic_array.map Term_types.Sum.get arr in *)
    (* SmartCons.Tuple.tuple_create array;; *)

  let tuple_get i typ ctx tup =
    let x = SmartCons.Tuple.tuple_get tup i in
    (Term_types.Prod.select typ Term_types.Sum.inj) x
  ;;

  let new_tuple_get i ctx tup typ = assert false
  
  (* MAYBE: Should the world be given as an argument? Probably not, it
     should be hidden like here. Probably the user of the module
     should never call world_fresh, and bunknown should also "compute
     its world" itself. *)
  let tuple_nondet  ctx l = assert false
    (* SmartCons.Tuple.nondet ~level:(level ctx) x (Array.to_list l) *)

  let tuple_nondet2  ctx l = assert false
  let tuple_bottom  ctx l = assert false    
  
  let root_context_upcast ctx = ctx.root_ctx

  let root_context() =
    let rec root_ctx = {
      root_ctx = ctx;
    }
    and ctx = {
      ctx = Root_context root_ctx;
      binding_depth = 0;
    }
    in root_ctx
  ;;

  let mu_context_upcast ctx = ctx.mu_ctx

  let mu_context_open ctx =
    let new_binding_depth = ctx.binding_depth + 1 in
    let rec new_ctx = {
      ctx = Mu_context new_mu_ctx;
      binding_depth = new_binding_depth;
    }
    and new_mu_ctx = {
      mu_ctx = new_ctx;
      mu_parent_ctx = ctx;
      mu_status = Ready
    }
    in new_mu_ctx
  ;;
  
  let name = "Term_domain"
  let unique_id = Domain_sig.Fresh_id.fresh name


  let satisfiable ctx bool =
    match bool.kind with
    | False -> Domain_sig.Unsat
    | _ -> Domain_sig.Unknown
  ;;

  let reachable _ = assert false


  type 'a in_tuple =
    | EmptyIn: unit in_tuple
    | ConsInInteger: Term.t * Term.t * 'a in_tuple -> ('a * Term_constructors_gadt.integer) in_tuple
    | ConsInBinary: Term.t * Term.t * 'a in_tuple -> ('a * Term_constructors_gadt.binary) in_tuple    
    | ConsInBoolean: Term.t * Term.t * 'a in_tuple -> ('a * Term_constructors_gadt.boolean) in_tuple
    | ConsInMemory: Term.t * Term.t * 'a in_tuple -> ('a * Term_constructors_gadt.memory) in_tuple        
  ;;

  type 'a in_acc = bool * 'a in_tuple

  type 'a out_tuple =
    | EmptyOut: unit out_tuple
    | ConsOutInteger: Term.t * 'a out_tuple -> ('a * Term_constructors_gadt.integer) out_tuple
    | ConsOutBinary: Term.t * 'a out_tuple -> ('a * Term_constructors_gadt.binary) out_tuple    
    | ConsOutBoolean: Term.t * 'a out_tuple -> ('a * Term_constructors_gadt.boolean) out_tuple
    | ConsOutMemory: Term.t * 'a out_tuple -> ('a * Term_constructors_gadt.memory) out_tuple    

  type ('a,'b) result =
      Result: bool * 'some in_tuple * ('some out_tuple -> 'a * 'b out_tuple) -> ('a,'b) result
  type empty_tuple = unit
  let empty_tuple = EmptyIn

  let serialize_memory ctx a b (inc,acc) =
    let acc = ConsInMemory(a,b,acc) in
    Result(inc,acc,fun (ConsOutMemory(r,acc)) -> r,acc)
                                   
  let serialize_binary ~size ctx a b (inc,acc) =
    let acc = ConsInBinary(a,b,acc) in
    Result(inc,acc,fun (ConsOutBinary(r,acc)) -> r,acc)
                                         
  let serialize_integer ctx a b (inc,acc) =
    let acc = ConsInInteger(a,b,acc) in
    Result(inc,acc,fun (ConsOutInteger(r,acc)) -> r,acc)

  let serialize_boolean ctx a b (inc,acc) =
    let acc = ConsInBoolean(a,b,acc) in
    Result(inc,acc,fun (ConsOutBoolean(r,acc)) -> r,acc)

  let typed_nondet2 ctx (in_tup: _ in_tuple) =
    (* Just create the nondet term. *)
    assert false
  ;;

  let typed_fixpoint_step ctx (in_tup: _ in_acc) =
    (* If we need new variables, introduce them and return false;
       else create the mu term and return true. *)
    assert false

  let assume_memory _ctx _ _ = assert false
  let assume_binary ~size _ _ = assert false
  let assume_boolean _ctx _ _ = assert false
  let assume_integer _ctx _ _ = assert false

  let memory_pretty ctx = Term.pretty
  let binary_pretty ~size ctx = Term.pretty
  let integer_pretty ctx = Term.pretty
  let boolean_pretty ctx = Term.pretty
  let tuple_pretty _ = assert false
  let integer_is_empty _ = assert false

  module Query = struct
    module Boolean_Lattice = Lattices.Dummy
    module Integer_Lattice = Lattices.Dummy
    module Binary_Lattice = struct
      include Lattices.Dummy
      let includes ~size = assert false                
      let is_bottom ~size = assert false
      let bottom ~size = assert false
      let top ~size = assert false
      let widen ~size ~previous _ = assert false
      let includes_or_widen ~size ~previous _ = assert false
      let inter ~size _ = assert false
      let join ~size _ = assert false
      let pretty ~size _ = assert false
      let singleton ~size = assert false
    end
                       
    let boolean ctx bool = Boolean_Lattice.top
    let reachable _ = assert false
    let binary ~size _ _ = Binary_Lattice.top ~size
    let integer _ _ = Integer_Lattice.top
    let convert_to_ival _ = assert false
    let convert_to_quadrivalent x = Lattices.Quadrivalent.Top
    let binary_to_ival ~signed ~size x = assert false
    let binary_to_known_bits ~size x = assert false      
    let binary_fold_crop ~size x = assert false
    let binary_is_empty ~size x = assert false
    let binary_is_singleton ~size x = assert false
    let is_singleton_int x = None
    
  end

  let union _ = assert false

  let should_focus : size:int -> Context.t -> memory -> binary ->
    (binary * int * int) option = fun ~size:_ _ctx _memory _addr ->
      None

  let may_alias : size:int -> Context.t -> binary -> binary -> bool =
    fun ~size:_ _ctx _memory _addr -> true
end
