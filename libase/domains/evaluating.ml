(**************************************************************************)
(*                                                                        *)
(*  This file is part of the Codex plugin of Frama-C.                     *)
(*                                                                        *)
(*  Copyright (C) 2007-2015                                               *)
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

(* Map of terms to their associated values. Each couple
   (term,abstraction) is an abstract value. *)

(* TODO: Should have a partitionning argument too; i.e. should not be
   given a partitionned memory lattice, but should do the
   partitionning itself. At least, it should propagate the decisions
   to split and to merge decisions. *)

(* The current implemementation consists in lifting a set of lattices
   (representing boolean values, scalars, and memory stores) into
   domains, using the implication hash. *)

module Log = Codex_log.Make(struct
    let category = "evaluating";;
    let debug = false
  end);;


module Term_domain = struct
  open Term_constructors;;

  let tuple_is_empty _ t = match t.kind with
    | Nondet(_,_,x) when Term_type.Term_set.is_empty x -> true
    | _ -> false

  let elt_is_empty _ t = match t.kind with
    | Tuple_get(x,_) when tuple_is_empty () x -> true
    | _ -> false

  let boolean_is_empty = elt_is_empty
end


(****************************************************************)

(* Convert_Ref_Addr allows to convert between the reference address
   stored in the terms that are interpreted, which may differ from the
   one used in the lattice. *)
open Domain_sig;;

module type Arg = sig
  include Base
end

(* Stack of hashtbl, used to ditch the results of a fixpoint
   computation by just removing the lastest element on the stack. *)
(* TODO: Use the new module for this in term. *)
(* module Term_Hashtbl_Stack = Term_hashtbl_stack;; *)
module Term_Hashtbl_Stack = struct

    type 'a t = ('a Term.Hashtbl.t) list

    let create () = [Term.Hashtbl.create 17];;

    (* Find looks things on the stack, while replace always replaces
       on top. [maybe_find] should be used in rare occasions, e.g. the
       pretty-printer. *)
    let rec maybe_find x v = match x with
      | [] ->  None
      | a::b ->
         try Some(Term.Hashtbl.find a v)
         with Not_found -> maybe_find b v

    let rec find x v = match x with
      | [] -> assert false      (* Should not happen; probably we forgot to evaluate something. *)
      | a::b ->
         try Term.Hashtbl.find a v
         with Not_found -> find b v

    let rec mem x v = match x with
      | [] -> false
      | a::b ->
         if Term.Hashtbl.mem a v
         then true
         else mem b v

    let replace x = Term.Hashtbl.replace (List.hd x)
    let push x = (Term.Hashtbl.create 17)::x
    let pop x = List.tl x
end

module Make_Memory_Binary_Boolean(D:Arg) =
struct

  let name = "Evaluating.Make_Memory_Binary_Boolean(" ^ D.name ^ ")";;
  let unique_id = Domain_sig.Fresh_id.fresh name;;

  type any =
    | AnyBoolean of D.boolean
    | AnyBinary of int * D.binary
    | AnyInteger of D.integer
    | AnyMemory of D.memory

  

  
  module State = struct

    (* The fact store is a tuple with the state of all the parts. *)
    type t = {
      boolean: D.boolean Term_Hashtbl_Stack.t;
      binary: D.binary Term_Hashtbl_Stack.t;
      memory: D.memory Term_Hashtbl_Stack.t;
      tuple: any list Term_Hashtbl_Stack.t;
      integer: D.integer Term_Hashtbl_Stack.t          
    };;

    let push {memory;integer;boolean;binary;tuple} =
      { memory = Term_Hashtbl_Stack.push memory;
        binary = Term_Hashtbl_Stack.push binary;
        tuple = Term_Hashtbl_Stack.push tuple;
        boolean = Term_Hashtbl_Stack.push boolean;
        integer = Term_Hashtbl_Stack.push integer;        
      }

    let pop {memory;integer;boolean;binary;tuple} =
      { memory = Term_Hashtbl_Stack.pop memory;
        binary = Term_Hashtbl_Stack.pop binary;
        tuple = Term_Hashtbl_Stack.pop tuple;
        integer = Term_Hashtbl_Stack.pop integer;        
        boolean = Term_Hashtbl_Stack.pop boolean;
      }

  end

  open State

  (****************************************************************)
  (* Contexts, i.e. imperative state of the domain, organized using nested scopes. *)

  (* TODO: to put in a Mu namespace. *)
  type mu_context = {
    mu_ctx: context;
    parent_ctx: context;
    domain_mu_context: D.mu_context;
  }

  and root_context = {
    domain_root_context: D.root_context;
    root_ctx: context;
  }

  and context_ =
    | Mu_context of mu_context
    | Root_context of root_context

  and context =
    { mutable state: State.t;
      domain_context: D.Context.t;
      ctx:context_
    }
  ;;

  module Context = struct
    type t = context
    let level x = D.Context.level x.domain_context
  end

  let domain_context ctx = match ctx.ctx with
    | Mu_context ctx -> D.mu_context_upcast ctx.domain_mu_context
    | Root_context ctx -> D.root_context_upcast ctx.domain_root_context

  let [@inline always] domain_context ctx = ctx.domain_context;;

  let root_context() =
    let domain_root_context = D.root_context () in
    let root_state =
      { memory = Term_Hashtbl_Stack.create ();
        boolean = Term_Hashtbl_Stack.create ();
        integer = Term_Hashtbl_Stack.create ();        
        binary = Term_Hashtbl_Stack.create ();
        tuple = Term_Hashtbl_Stack.create ();
      }
    in
    let rec root_ctx =
      { domain_root_context;
        root_ctx = ctx
      }
    and ctx = {state=root_state;
               domain_context = D.root_context_upcast domain_root_context;
               ctx = Root_context root_ctx}
    in root_ctx
  ;;

  let context_intro_tuple_formal _ = assert false

  let mu_context_upcast ctx = ctx.mu_ctx;;
  let root_context_upcast ctx = ctx.root_ctx;;

  let mu_context_open ctx =
    let domain_mu_context = D.mu_context_open (domain_context ctx) in
    let state = State.push ctx.state in
    let rec mu_context =
    {
      mu_ctx = context;
      parent_ctx = ctx;
      domain_mu_context;
    }
    and context = { state; ctx =  Mu_context mu_context;
                    domain_context = D.mu_context_upcast domain_mu_context }
    in mu_context
  ;;


  (****************************************************************)
  (* Helper modules that help writing new interpreters.  *)

  module Get = struct
    type state = context
    let boolean ctx = Term_Hashtbl_Stack.find ctx.state.boolean
    let integer ctx = Term_Hashtbl_Stack.find ctx.state.integer        
    let binary ~size ctx = Term_Hashtbl_Stack.find ctx.state.binary
    let memory ctx = Term_Hashtbl_Stack.find ctx.state.memory
    let tuple ctx = Term_Hashtbl_Stack.find ctx.state.tuple
  end

  module Set = struct
    type state = context
    type result = unit
    let boolean ctx term value =
      Log.debug "setting %a to \n%a"
        Term.pretty_short term (D.boolean_pretty (domain_context ctx)) value;
      Term_Hashtbl_Stack.replace ctx.state.boolean term value

    let integer ctx term value =
      Log.debug "setting %a to \n%a"
        Term.pretty_short term (D.integer_pretty (domain_context ctx)) value;
      Term_Hashtbl_Stack.replace ctx.state.integer term value

    let binary ~size ctx term value =
      Log.debug "setting %a to \n%a"
        Term.pretty_short term (D.binary_pretty ~size (domain_context ctx)) value;
      Term_Hashtbl_Stack.replace ctx.state.binary term value

    let memory ctx term value =
      (* Kernel.feedback "setting %a to \n%a"              Term.pretty_short term (D.memory_pretty (domain_context ctx)) value; *)
      Log.debug "setting %a to \n%a"
        Term.pretty_short term (D.memory_pretty (domain_context ctx)) value;
      Term_Hashtbl_Stack.replace ctx.state.memory term value

    let tuple ctx term value =
      (* Log.debug ~category "setting %a to \n%a" *)
      (*   Term.pretty_short term (D.tuple_pretty (domain_context ctx)) value; *)
      Term_Hashtbl_Stack.replace ctx.state.tuple term value

  end

  (****************************************************************)
  (* Application of the modules above to get a fact_store.  *)


  (****************************************************************)
  (* Domain interface. *)

  (* TODO: Simplify terms using the underlying abstract domain
     (notably, using constant propagation). This requires smart
     constructors that can query the underlying abstract value.

     Note that currently, the evaluation is done lazily, and not
     during the construction of the terms. The main reason for this is
     that there can be syntactic simplifications happening while
     constructing terms, thus useless computations (of intermediate
     terms not appearing in the final term) would be performed. In
     general, truth_value is what triggers an evaluation.

     However, lazy evaluation does not allow online simplification of
     terms; e.g. when the domain to which the term is evaluated can
     prove that an expression is constant, it can be replaced with
     that constant, which can triggers further syntactic
     simplifications. This requires eager evaluation.

     A way to combine the best of both world would be to have terms,
     that lazily evaluate to other terms, that eagerly evaluate to
     values. The first layer performs many syntactic simplification
     that avoids the useless computations; while the eager layer give
     access to the remaining simplifications. When required,
     "simplification" would allow to return the simplified terms (or
     change the abstract value to the simplified term in place). *)
        
  (* let split_integer _ = assert false;; *)

  (* Change the printers to also display the value. Note that this can
     lead to performing more evaluations (and sooner) than would
     otherwise be done when Print_Force_Lazyness is set. *)
  let make_printer ctx eval hashstack pretty (fmt:Format.formatter) x =
    let domain_ctx = domain_context ctx in
      let value = eval ctx x in
      Format.fprintf fmt "%a (value %a)" Term.pretty x (pretty domain_ctx) value
  ;;


  type boolean = Term.t
  type binary = Term.t
  type integer = Term.t
  type memory = Term.t
  type tuple = Term.t
  type 'a typ =
    | Boolean : boolean typ
    | Binary : int -> binary typ
    | Integer : integer typ
    | Memory : memory typ



  let typed_serialize_elt: D.Context.t -> any -> any -> 'a D.in_acc -> (any,'a) D.result =
    fun ctx a b acc ->
      let open D in
      match a,b with
      | AnyMemory a, AnyMemory b ->
        let D.Result(included,acc,d) = D.serialize_memory ctx a b acc in
        D.Result(included,acc,(fun x -> let m,x = d x in AnyMemory m,x))
      | AnyBoolean a, AnyBoolean b->
        let D.Result(included,acc,d) = D.serialize_boolean ctx a b acc in
        D.Result(included,acc,(fun x -> let b,x = d x in AnyBoolean b,x))
      | AnyInteger a, AnyInteger b ->
        let D.Result(included,acc,d) = D.serialize_integer ctx a b acc in
        D.Result(included,acc,(fun x -> let i,x = d x in AnyInteger i,x))
      | AnyBinary(sizea,a), AnyBinary(sizeb,b)
        when sizea == sizeb ->
        let D.Result(included,acc,d) = D.serialize_binary ~size:sizea ctx a b acc in
        D.Result(included,acc,(fun x -> let i,x = d x in AnyBinary(sizea,i),x))
      | _ -> Codex_log.fatal "Dynamic type mismatch"       (* Type mismatch. *)
  ;;

  let typed_serialize_tuple2 ctx a b acc =
    (* We need polymorphic recursion here. *)
    let rec loop: type a. a D.in_acc -> _ -> _ -> (any list,a) D.result =
      fun (included,acc as in_acc) a b ->
      match a,b with
      | [],[] -> D.Result(included, acc, (fun x -> [],x))
      | [], _ | _, [] -> assert false
      | a::taila,b::tailb ->
        let D.Result(included,acc,d_elt) = typed_serialize_elt ctx a b in_acc in
        let D.Result(included,acc,d_loop) = loop (included,acc) taila tailb in
        D.Result(included, acc,(fun x ->
            let tl,x = d_loop x in
            let hd,x = d_elt x in
            hd::tl,x))
    in
    loop acc a b
  ;;

  (****************************************************************)
  (* Mu and "big-step" driving of the iteration loop *)

  (* Note: one of the main difference with the previous "mu_context_*"
     interface, besides the small-step/big-step difference, is that
     this function must associate a value to an existing term, while
     the above one builds a new term.

     However, both work similarly using a stack of context to store
     temporarily the results of fixpoint computation. *)


  let ar2_bin_bin_bin ~size f ctx t1 t2 t =
    Set.binary ~size ctx t @@ f ~size (domain_context ctx) (Get.binary ~size ctx t1) (Get.binary ~size ctx t2)
  ;;

  let ar2_bin_bin_bool ~size f ctx t1 t2 t =
    Set.boolean ctx t @@ f ~size (domain_context ctx) (Get.binary ~size ctx t1) (Get.binary ~size ctx t2)
  ;;

  let ar0_bin ~size f ctx t =
    Set.binary ~size ctx t @@ f (domain_context ctx)

  let ar1_bin_bin ~size f ctx t1 t =
    Set.binary ~size ctx t @@ f (domain_context ctx) (Get.binary ~size ctx t1)

  let ar1_bool_bin ~size f ctx t1 t =
    Set.binary ~size ctx t @@ f (domain_context ctx) (Get.boolean ctx t1)
    


  let is_evaled ctx t =
    let {memory;integer;boolean;binary;tuple} = ctx.state in
    match Term.type_of t with
    | Term_types.Memory -> Term_Hashtbl_Stack.mem memory t
    | Term_types.Binary _ -> Term_Hashtbl_Stack.mem binary t
    | Term_types.Integer -> Term_Hashtbl_Stack.mem integer t                               
    | Term_types.Boolean -> Term_Hashtbl_Stack.mem boolean t
    | Term_types.Tuple _ -> Term_Hashtbl_Stack.mem tuple t
  ;;

  (* This breaks the recursion between Fixpoint and eval. *)
  (* let ref_eval = ref (fun _ _ -> assert false);; *)

  let rec eval_boolean ctx term =
    eval ctx term;
    Get.boolean ctx term

  and eval_integer ctx term =
    eval ctx term;
    Get.integer ctx term

  and eval_binary ctx term =
    eval ctx term;
    let size = Term.type_of term in
    Get.binary ~size ctx term

  and eval_memory ctx term =
    eval ctx term;
    Get.memory ctx term

  and eval_tuple ctx term =
    eval ctx term;
    Get.tuple ctx term

  (* Note: using assertions, user-provided or already computed loop
     invariants can lead to having a smaller "previous" value (not yet
     implemented). But to ensure convergence by widening, it is
     important that the widening is given the "previous" value
     computed by the previous fixpoint iteration (i.e. the one stored
     in the ctx), not the one "reduced" by the condition.  *)
  (* Note: we cannot reuse the mu_context functions we defined earlier, because they would
     create new terms, and we do not want that. *)
  and forward_eval_mu parent_ctx muterm varterm body actual_arg =
    let actual_arg_value = eval_tuple parent_ctx actual_arg in
    let mu_ctx = mu_context_open parent_ctx in
    let d_muctx = mu_ctx.domain_mu_context in
    let ctx = mu_ctx.mu_ctx in
    let open Term_types in
    let rec loop tuple =
      (* Log.debug "Loop################ %a %a" Term.pretty varterm *)
      (*   (D.tuple_pretty (domain_context ctx)) tuple; *)
      Term_Hashtbl_Stack.replace ctx.state.State.tuple varterm tuple;
      let body_tuple = eval_tuple ctx body in
      (* Log.debug ~category "End loop################ body %a %a" Term.pretty body *)
      (*   (D.tuple_pretty (domain_context ctx)) body_tuple; *)
      let D.Result(included,acc,d_tuple) = typed_serialize_tuple2 ctx.domain_context tuple body_tuple (true,D.empty_tuple) in
      let fixpoint_reached,subf = D.typed_fixpoint_step d_muctx (included,acc) in
      if fixpoint_reached
      then fst @@ d_tuple @@ subf ~close:true
      else
        let result_tuple = fst @@ d_tuple @@ subf ~close:false in
        (ctx.state <- State.push (State.pop ctx.state); loop result_tuple)

    in
    let final_tuple = loop actual_arg_value(* init_formal *) in
    (* Log.debug ~category "Over ################ %a %a" Term.pretty varterm *)
    (*   (D.tuple_pretty (domain_context ctx)) final_tuple; *)
    Set.tuple parent_ctx muterm final_tuple

  
  and forward_eval_one ctx t =
    Log.debug "Eval %a" Term.pretty_short t;
    match t.Term_constructors.kind with
      
    (* Tuple constructors. *)
    | Term_constructors.Tuple_get(tup,i) -> begin
        (* Note: Not very efficient, but these tuples are small anyway. *)
        match Term.type_of t with
        | Term_types.Memory ->
          let AnyMemory m = List.nth (eval_tuple ctx tup) i in
          Set.memory ctx t m
        | Term_types.Integer ->
          let AnyInteger m = List.nth (eval_tuple ctx tup) i in
          Set.integer ctx t m
        | Term_types.Boolean ->
          let AnyBoolean m = List.nth (eval_tuple ctx tup) i in
          Set.boolean ctx t m
        | Term_types.Binary size ->
          (* Codex_log.feedback "tuple %d size %d %a" i size Term.pretty tup ; *)
          (* let x = List.nth (eval_tuple ctx tup) i in
           * Codex_log.feedback "x is %a" D.Domain_any x; *)
          let AnyBinary(s1,m) = List.nth (eval_tuple ctx tup) i in
          assert(size == s1);
          Set.binary ~size ctx t m                        
        | _ -> assert false
      end
    | Term_constructors.Tuple_create(types,tup) ->
      let tup = Immutable_dynamic_array.fold_left (fun acc x ->
          match Term.type_of x with
          | Term_types.Memory -> (AnyMemory (eval_memory ctx x))::acc
          | Term_types.Binary size -> (AnyBinary(size, eval_binary ctx x))::acc
          | Term_types.Integer -> (AnyInteger(eval_integer ctx x))::acc
          | Term_types.Boolean -> (AnyBoolean(eval_boolean ctx x))::acc
          | _ -> assert false
        ) [] tup in
      let tup = List.rev tup in
      Set.tuple ctx t tup
    | Term_constructors.Mu(_time,var,loop,init) -> forward_eval_mu ctx t var loop init
    | Term_constructors.Nondet(level,types,values) ->
      let values = Term.Set.elements values in
      begin match values with
        | [] -> Set.tuple ctx t @@ (assert false)
          (* D.tuple_bottom (Immutable_dynamic_array.to_list types) (domain_context ctx) (* Note: can this happen? *) *)

      | [l] -> Set.tuple ctx t @@ Get.tuple ctx l
      | a::b -> Set.tuple ctx t @@
        List.fold_left (fun acc b ->
            let D.Result(_,tup,d_tuple) =
              typed_serialize_tuple2 ctx.domain_context acc (Get.tuple ctx b) (true,D.empty_tuple)
            in
            let res,_ = d_tuple (D.typed_nondet2 (domain_context ctx) tup) in
            res
          ) (Get.tuple ctx a) b

      end
    | Term_constructors.Builtin(b,t1) ->
      let input_tuple = Get.tuple ctx t1 in
      (match b with
       | Builtin.Show_each_in s ->
         let AnyMemory memory = List.hd input_tuple in
         let args = List.map (function
             | AnyBinary(size,x) -> let x:D.binary = x in (size,x)
             | _ -> assert false) @@ List.tl input_tuple
         in 
         let res = D.builtin_show_each_in s ctx.domain_context args memory in
         Set.tuple ctx t @@ [AnyMemory res]
      )
      
    (* Boolean constructors. *)
    | Term_constructors.True -> Set.boolean ctx t @@
      D.Boolean_Forward.true_ (domain_context ctx)
    | Term_constructors.False -> Set.boolean ctx t @@
      D.Boolean_Forward.false_ (domain_context ctx)
    | Term_constructors.Not t1 -> Set.boolean ctx t @@
      D.Boolean_Forward.not (domain_context ctx) (Get.boolean ctx t1)
    | Term_constructors.And(t1,t2) -> Set.boolean ctx t @@
      D.Boolean_Forward.(&&) (domain_context ctx) (Get.boolean ctx t1) (Get.boolean ctx t2)
    | Term_constructors.Or(t1,t2) -> Set.boolean ctx t @@
      D.Boolean_Forward.(||) (domain_context ctx) (Get.boolean ctx t1) (Get.boolean ctx t2)        

    (* Binary constructors. *)
    | Term_constructors.Biadd(size,nsw,nuw,t1,t2) -> (ar2_bin_bin_bin ~size) (D.Binary_Forward.biadd ~nsw ~nuw) ctx t1 t2 t
    | Term_constructors.Bisub(size,nsw,nuw,t1,t2) -> (ar2_bin_bin_bin ~size) (D.Binary_Forward.bisub ~nsw ~nuw) ctx t1 t2 t
    | Term_constructors.Bimul(size,nsw,nuw,t1,t2) -> (ar2_bin_bin_bin ~size) (D.Binary_Forward.bimul ~nsw ~nuw) ctx t1 t2 t
    | Term_constructors.Bisdiv(size,t1,t2) -> (ar2_bin_bin_bin ~size) D.Binary_Forward.bisdiv ctx t1 t2 t
    | Term_constructors.Bismod(size,t1,t2) -> (ar2_bin_bin_bin ~size) D.Binary_Forward.bismod ctx t1 t2 t                                                                                               
    | Term_constructors.Biudiv(size,t1,t2) -> (ar2_bin_bin_bin ~size) D.Binary_Forward.biudiv ctx t1 t2 t
    | Term_constructors.Biumod(size,t1,t2) -> (ar2_bin_bin_bin ~size) D.Binary_Forward.biumod ctx t1 t2 t
    | Term_constructors.Bshl(size,nsw,nuw,t1,t2) -> (ar2_bin_bin_bin ~size) (D.Binary_Forward.bshl ~nsw ~nuw) ctx t1 t2 t
    | Term_constructors.Bashr(size,t1,t2) -> (ar2_bin_bin_bin ~size) D.Binary_Forward.bashr ctx t1 t2 t
    | Term_constructors.Blshr(size,t1,t2) -> (ar2_bin_bin_bin ~size) D.Binary_Forward.blshr ctx t1 t2 t
    | Term_constructors.Bor(size,t1,t2) -> (ar2_bin_bin_bin ~size) D.Binary_Forward.bor ctx t1 t2 t
    | Term_constructors.Bxor(size,t1,t2) -> (ar2_bin_bin_bin ~size) D.Binary_Forward.bxor ctx t1 t2 t
    | Term_constructors.Band(size,t1,t2) -> (ar2_bin_bin_bin ~size) D.Binary_Forward.band ctx t1 t2 t                                              
    | Term_constructors.Beq(size,t1,t2) -> ar2_bin_bin_bool ~size   D.Binary_Forward.beq ctx t1 t2 t
    | Term_constructors.Bisle(size,t1,t2) -> ar2_bin_bin_bool ~size D.Binary_Forward.bisle ctx t1 t2 t
    | Term_constructors.Biule(size,t1,t2) -> ar2_bin_bin_bool ~size D.Binary_Forward.biule ctx t1 t2 t
    | Term_constructors.Biconst(size,k) -> ar0_bin ~size (D.Binary_Forward.biconst ~size k) ctx t
    | Term_constructors.Bunknown(size,(level,_)) -> ar0_bin ~size (D.binary_unknown ~size ~level) ctx t
    | Term_constructors.Buninit(size) -> ar0_bin ~size (D.Binary_Forward.buninit ~size) ctx t
    | Term_constructors.Bconcat(size,t1,t2) -> 
      let size1 = Term.size_of t1 in
      let size2 = Term.size_of t2 in
      Set.binary ~size ctx t @@
      D.Binary_Forward.bconcat ~size1 ~size2 (domain_context ctx) (Get.binary ctx ~size:size1 t1) (Get.binary ctx ~size:size2 t2)
    | Term_constructors.Bextract(size,index,t1) ->
      let oldsize = Term.size_of t1 in
      ar1_bin_bin ~size (D.Binary_Forward.bextract ~oldsize ~index ~size) ctx t1 t

    | Term_constructors.Buext(size,t1) ->
      let oldsize = Term.size_of t1 in
      ar1_bin_bin ~size (D.Binary_Forward.buext ~size ~oldsize) ctx t1 t
    | Term_constructors.Bsext(size,t1) ->
      let oldsize = Term.size_of t1 in      
      ar1_bin_bin ~size (D.Binary_Forward.bsext ~size ~oldsize) ctx t1 t
    | Term_constructors.Valid(size, acc_typ, t1) ->
       Set.boolean ctx t @@ D.Binary_Forward.valid ~size acc_typ (domain_context ctx) (Get.binary ~size ctx t1)
    | Term_constructors.Bofbool(size,t1) ->
      ar1_bool_bin ~size (D.Binary_Forward.bofbool ~size) ctx t1 t      

    | Term_constructors.Bshift(size,offset,max,t1) ->
      ar1_bin_bin ~size (D.Binary_Forward.bshift ~size ~offset ~max) ctx t1 t

    | Term_constructors.Bindex(size,t1,k,t2) ->
      Set.binary ~size ctx t @@
      D.Binary_Forward.bindex ~size k (domain_context ctx) (Get.binary ~size ctx t1) (Get.binary ~size ctx t2)

    | Term_constructors.Bindex _ -> assert false      
  

    (* Memory constructors. *)
    | Term_constructors.Memunknown(level,_) -> Set.memory ctx t @@ D.Memory_Forward.unknown ~level (domain_context ctx)
    | Term_constructors.Load(size,store,address) ->
      Set.binary ~size ctx t @@
      D.Memory_Forward.load ~size (domain_context ctx) (Get.memory ctx store) (Get.binary ~size:32 ctx address)
    | Term_constructors.Store(size,store,address,value) ->
      Set.memory ctx t @@
      D.Memory_Forward.store ~size (domain_context ctx) (Get.memory ctx store) (Get.binary ~size:32 ctx address) (Get.binary ~size ctx value)
    | Term_constructors.Memcpy(size,store,from,to_) ->
      Set.memory ctx t @@
      D.Memory_Forward.memcpy ~size (domain_context ctx) (Get.memory ctx store) (Get.binary ~size:32 ctx from) (Get.binary ~size:32 ctx to_) 
    | Term_constructors.Free(mem,ptr) ->
      Set.memory ctx t @@
      D.Memory_Forward.free (domain_context ctx) (Get.memory ctx mem) (Get.binary ~size:(Codex_config.ptr_size()) ctx ptr)
    | Term_constructors.Malloc(id,malloc_size,mem) ->
      let dctx = (domain_context ctx) in
      let addr,mem' =
        D.Memory_Forward.malloc ~id  ~malloc_size dctx  (Get.memory ctx mem) in
      let tup =
          [AnyBinary (Codex_config.ptr_size(),addr);
           AnyMemory mem'] in
      Set.tuple ctx t tup
    | Term_constructors.Assume(cond,v) ->
      let dctx = domain_context ctx in
      let condv = (Get.boolean ctx cond) in
       (match (Term.type_of v) with
         | Term_types.Memory -> Set.memory ctx t @@ D.assume_memory dctx condv (Get.memory ctx v)
         | Term_types.Integer -> Set.integer ctx t @@ D.assume_integer dctx condv (Get.integer ctx v)
         | Term_types.Boolean -> Set.boolean ctx t @@ D.assume_boolean dctx condv (Get.boolean ctx v)                           
         | Term_types.Binary size -> Set.binary ~size ctx t @@ D.assume_binary ~size dctx condv (Get.binary ~size ctx v)                           
         | _ -> assert false)
    | _ -> Term.identify "evaluating" t

  (* Evaluate a term, and all its siblings that have not yet been
     evaluated for the hypothesis [h]. *)
  and eval ctx term =
    let filter t = not (is_evaled ctx t) in
    let f t = forward_eval_one ctx t in
    Term_dag.reverse_topological_iter [term] filter f
  ;;

  let boolean_pretty ctx = make_printer ctx eval_boolean ctx.state.State.boolean D.boolean_pretty
  let integer_pretty ctx = make_printer ctx eval_integer ctx.state.State.integer D.integer_pretty      
  let binary_pretty ~size ctx = make_printer ctx eval_binary ctx.state.State.binary (D.binary_pretty ~size)
  let memory_pretty ctx = make_printer ctx eval_memory ctx.state.State.memory D.memory_pretty
  (* let tuple_pretty ctx = make_printer ctx eval_tuple ctx.state.State.tuple D.tuple_pretty *)

  
  (* ref_eval := eval;; *)

  let satisfiable ctx b =
    D.satisfiable (domain_context ctx) (eval_boolean ctx b)
  ;;

  module Query = struct
    
    module Boolean' = D.Query.Boolean_Lattice
    module Q':module type of D.Query
      with module Boolean_Lattice := Boolean' 
       and type Binary_Lattice.t = D.Query.Binary_Lattice.t
       and type Integer_Lattice.t = D.Query.Integer_Lattice.t
      = D.Query
    include Q'

    module Boolean_Lattice = struct
      include Lattices.Quadrivalent
    end

    let convert_to_quadrivalent x = x
            

    
    let boolean ctx t =
      let open Term_constructors in
      let open Domain_sig in
      (* Note: The syntactic match cannot be more precise than using
         eval, but may be faster. *)
      match t.kind with
      | True -> Quadrivalent.True
      | False -> Quadrivalent.False
      | _ when Term_domain.boolean_is_empty () t -> Quadrivalent.Bottom
      | _ -> D.Query.convert_to_quadrivalent
        @@ D.Query.boolean (domain_context ctx) (eval_boolean ctx t)

    (* let inject_boolean ctx t v = D.Query.inject_boolean (domain_context ctx) (eval_boolean ctx t) v *)
    let inject_boolean ctx t v = assert false
    let binary ~size ctx t = D.Query.binary ~size (domain_context ctx) (eval_binary ctx t)
    let integer ctx t = D.Query.integer (domain_context ctx) (eval_integer ctx t)
    let reachable ctx t = D.Query.reachable (domain_context ctx) (eval_memory ctx t)
  end

  let should_focus : size:int -> Context.t -> memory -> binary -> (binary * int * int) option =
    fun ~size ctx memory addr -> assert false

  let may_alias ~size:_ = assert false

end
