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

module Make(D:Domain_sig.Base) = struct

  let name = "With_Bottom(" ^ D.name ^ ")";;
  let unique_id = Domain_sig.Fresh_id.fresh name;;

  module Types = struct
    type binary = D.binary option
    type memory = D.memory option
    type integer = D.integer option
    type boolean = D.boolean option
  end
  include Types

  module Context = D.Context

  type root_context = D.root_context
  let root_context_upcast = D.root_context_upcast
  let root_context = D.root_context

  include Builtin.Make(Types)(Context)

  type mu_context = D.mu_context
  let mu_context_upcast = D.mu_context_upcast
  let mu_context_open = D.mu_context_open
                      
  let [@inline always] ar0 f = fun ctx -> Some (f ctx)
  let [@inline always] ar1 f = fun ctx a -> match a with
    | None -> None
    | Some a -> Some(f ctx a)
  let [@inline always] ar2 f = fun ctx a b -> match a,b with
    | Some a, Some b -> Some (f ctx a b)
    | None, _ | _, None -> None
  ;;
  let [@inline always] ar3 f = fun ctx a b c -> match a,b,c with
    | Some a, Some b,  Some c -> Some (f ctx a b c)
    | _ -> None
  ;;

  let [@inline always] assume f = fun ctx cond x -> match cond,x with
    | Some cond, Some x -> begin
      match D.Query.convert_to_quadrivalent @@ D.Query.boolean ctx cond with
        | Quadrivalent_Lattice.False | Quadrivalent_Lattice.Bottom -> None
        | Quadrivalent_Lattice.True -> Some x
        | _ ->Some(f ctx cond x)
      end
    | _ -> None

  (* We can also spend more time to test the satifiability of boolean
     conditions.  *)
  let [@inline always] _assume f = fun ctx cond x -> match cond,x with
    | Some cond, Some x -> begin
      match D.Query.convert_to_quadrivalent @@ D.Query.boolean ctx cond with
        | Quadrivalent_Lattice.False | Quadrivalent_Lattice.Bottom -> None
        | Quadrivalent_Lattice.True -> Some x
        | _ -> begin match D.satisfiable ctx cond with
            | Domain_sig.Unsat -> None
            | _ -> Some(f ctx cond x)
          end
    end
    | _ -> None
  ;;
  
  
  module Memory_Forward = struct
    let store ~size = ar3 @@ D.Memory_Forward.store ~size
    let memcpy ~size = ar3 @@ D.Memory_Forward.memcpy ~size        
    let load ~size = ar2 @@ D.Memory_Forward.load ~size
    let malloc ~id ~malloc_size ctx mem = match mem with
      | None -> None,None
      | Some mem ->
        let addr,newmem = D.Memory_Forward.malloc ~id ~malloc_size ctx mem in
        Some(addr),Some(newmem)
          
    let free ctx mem ptr = match mem,ptr with
      | None, _ | _, None -> None
      | Some mem, Some ptr -> Some(D.Memory_Forward.free ctx mem ptr)
    let unknown ~level ctx = Some(D.Memory_Forward.unknown ~level ctx)
  end

  module Boolean_Forward = struct
    let (||) = ar2 D.Boolean_Forward.(||)
    let (&&) = ar2 D.Boolean_Forward.(&&)        
    let not = ar1 D.Boolean_Forward.not
    let true_ = ar0 D.Boolean_Forward.true_
    let false_ = ar0 D.Boolean_Forward.false_
  end

  module Integer_Forward = struct
    let one = ar0 D.Integer_Forward.one
    let zero = ar0 D.Integer_Forward.zero
    let iconst k = ar0 @@ D.Integer_Forward.iconst k
    let ile = ar2 D.Integer_Forward.ile
    let ieq = ar2 D.Integer_Forward.ieq
    let isub = ar2 D.Integer_Forward.isub
    let ixor = ar2 D.Integer_Forward.ixor
    let ior = ar2 D.Integer_Forward.ior
    let iand = ar2 D.Integer_Forward.iand
    let ishr = ar2 D.Integer_Forward.ishr
    let ishl = ar2 D.Integer_Forward.ishl
    let imod = ar2 D.Integer_Forward.imod
    let idiv = ar2 D.Integer_Forward.idiv
    let imul = ar2 D.Integer_Forward.imul
    let iadd = ar2 D.Integer_Forward.iadd
    let itimes k =ar1 @@ D.Integer_Forward.itimes k
  end

  module Binary_Forward = struct
    let beq   ~size = ar2 (D.Binary_Forward.beq   ~size)
    let biule ~size = ar2 (D.Binary_Forward.biule ~size)
    let bisle ~size = ar2 (D.Binary_Forward.bisle ~size)       
    let biadd ~size ~nsw ~nuw = ar2 (D.Binary_Forward.biadd ~size ~nsw ~nuw)
    let bisub ~size ~nsw ~nuw = ar2 (D.Binary_Forward.bisub ~size ~nsw ~nuw)
    let bimul ~size ~nsw ~nuw = ar2 (D.Binary_Forward.bimul ~size ~nsw ~nuw)
    let bxor ~size = ar2 (D.Binary_Forward.bxor ~size)
    let band ~size = ar2 (D.Binary_Forward.band ~size)
    let bor ~size = ar2 (D.Binary_Forward.bor ~size)
    let bsext ~size ~oldsize = ar1 @@ D.Binary_Forward.bsext ~size ~oldsize
    let buext ~size ~oldsize = ar1 @@ D.Binary_Forward.buext ~size ~oldsize
    let bofbool ~size = ar1 @@ D.Binary_Forward.bofbool ~size
    let bchoose ~size cond = ar1 @@ D.Binary_Forward.bchoose ~size cond
    let bashr ~size = ar2 (D.Binary_Forward.bashr ~size)
    let blshr ~size = ar2 (D.Binary_Forward.blshr ~size)
    let bshl ~size ~nsw ~nuw = ar2 (D.Binary_Forward.bshl ~size ~nsw ~nuw)
    let bisdiv ~size = ar2 (D.Binary_Forward.bisdiv ~size)
    let biudiv ~size = ar2 (D.Binary_Forward.biudiv ~size)
    let bconcat ~size1 ~size2 = ar2 @@ D.Binary_Forward.bconcat ~size1 ~size2
    let bismod ~size = ar2 (D.Binary_Forward.bismod ~size)
    let biumod ~size = ar2 (D.Binary_Forward.biumod ~size)
    let bextract ~size ~index ~oldsize = ar1 @@ D.Binary_Forward.bextract ~size ~index ~oldsize
    let valid ~size acc_typ = ar1 @@ D.Binary_Forward.valid ~size acc_typ
    let valid_ptr_arith ~size = ar2 @@ D.Binary_Forward.valid_ptr_arith ~size
    let biconst ~size k = ar0 @@ D.Binary_Forward.biconst ~size k
    let buninit ~size = ar0 @@ D.Binary_Forward.buninit ~size
    let bshift ~size ~offset ~max = ar1 @@ D.Binary_Forward.bshift ~size ~offset ~max
    let bindex ~size k = ar2 @@ D.Binary_Forward.bindex ~size k
  end

  module Binary = Datatype_sig.Option(D.Binary)
  module Integer = Datatype_sig.Option(D.Integer)
  module Boolean = Datatype_sig.Option(D.Boolean)      

 (**************** Pretty printing ****************)

  let memory_pretty ctx fmt = function
    | None -> Format.fprintf fmt "<bottom>"
    | Some x -> D.memory_pretty ctx fmt x

  let binary_pretty ~size ctx fmt = function
    | None -> Format.fprintf fmt "<bottom>"
    | Some x -> D.binary_pretty ~size ctx fmt x

  let boolean_pretty ctx fmt = function
    | None -> Format.fprintf fmt "<bottom>"
    | Some x -> D.boolean_pretty ctx fmt x

  let integer_pretty ctx fmt = function
    | None -> Format.fprintf fmt "<bottom>"
    | Some x -> D.integer_pretty ctx fmt x

  (**************** Serialization, fixpoind and nondet. ****************)

  (* Here, we build both the entire list of arguments passed to
     serialize, plus a "summary" that tells if one of the tuple is
     bottom. Then, we call nondet/fixpoint step only if necessary. *)
  type 'b maybe_bottom =
    | IsBottom of {a_bottom:bool;b_bottom:bool}
    | NoBottom of 'b D.in_tuple

  type 'a in_list =
    | Empty: unit in_list
    | Cons: 'a * 'a * 'b in_list -> ('a * 'b) in_list

  type ('a,'b) pair = private ('a * 'b)

  type 'c in_tuple =
      In: { list: 'a in_list;
            sub: 'b maybe_bottom; } -> (('a,'b) pair) in_tuple

  type 'a in_acc = bool * 'a in_tuple

  type empty_tuple = (unit,D.empty_tuple) pair
  let empty_tuple = In{ list=Empty; sub=NoBottom(D.empty_tuple)}

  (* The resulting computation: have we computed something, or should
     we juste take one of the arguments (or none). *)
  type 'c out_tuple =
    | OutNoBottom: 'b D.out_tuple -> (_,'b) pair out_tuple
    | OutKeepA: 'a in_list -> ('a,_) pair out_tuple
    | OutKeepB: 'a in_list -> ('a,_) pair out_tuple          
    | OutNone: ('a,'b) pair out_tuple
  ;;

  type ('a,'b) result = Result: bool * 'some in_tuple * ('some out_tuple -> 'a * 'b out_tuple) -> ('a,'b) result    

  (* Higher-ranked polymorphism is required here, and we need a record for that. *)
  type 'elt higher = {subf: 'tl. D.Context.t -> 'elt -> 'elt -> 'tl D.in_acc -> ('elt,'tl) D.result  } [@@unboxed]

  (* Note: OCaml infers the wrong type (no principal type), we have to help it here. *)
  let serialize (type elt) (type c)
      {subf} ctx a b (included, (In acc:c in_tuple)) : (elt option,c) result =
    let deserialize d = function
      | OutNoBottom tup ->
        let res,tup = d tup in
        Some res,OutNoBottom tup
      | OutKeepA (Cons(a,b,tail)) -> a,OutKeepA(tail)
      | OutKeepB (Cons(a,b,tail)) -> b,OutKeepB(tail)
      | OutNone -> None,OutNone
    in
    let list = Cons(a,b,acc.list) in
    let dummy_d = fun _ -> assert false in
    match acc.sub with
    | NoBottom sub -> begin
        match a,b with
        | Some a, Some b ->
          let D.Result(included,sub,d) = subf ctx a b (included,sub) in
          Result(included, In{list;sub=NoBottom sub}, deserialize d)
        | _ ->
          let sub = IsBottom{a_bottom=(a = None);b_bottom=(b = None)} in
          Result((b = None) && included, In{list;sub},deserialize dummy_d)
      end
    | IsBottom {a_bottom;b_bottom} ->
      let a_bottom = a_bottom || a = None in
      let b_bottom = b_bottom || b = None in
      let sub = IsBottom{a_bottom;b_bottom} in
      Result(b_bottom && included, In{list;sub},deserialize dummy_d)
  ;;

  let serialize_memory  ctx a b acc = serialize {subf=D.serialize_memory}  ctx a b acc
  let serialize_integer ctx a b acc = serialize {subf=D.serialize_integer} ctx a b acc
  let serialize_boolean ctx a b acc = serialize {subf=D.serialize_boolean} ctx a b acc
  let serialize_binary ~size ctx a b acc = serialize 
      {subf=fun ctx a b acc -> D.serialize_binary ~size ctx a b acc}
      ctx a b acc

  (* Note: OCaml infers the wrong type (no principal type), we have to help it here. *)      
  let typed_nondet2 (type c) ctx (x: c in_tuple) : c out_tuple =
    let (In acc) = x in
    match acc.sub with
    | NoBottom l ->
      OutNoBottom (D.typed_nondet2 ctx l)
    | IsBottom{a_bottom=false;b_bottom=false} -> assert false
    | IsBottom{a_bottom=true;b_bottom=true} ->
      OutNone
    | IsBottom{a_bottom=true;b_bottom=false} ->
      OutKeepB acc.list
    | IsBottom{a_bottom=false;b_bottom=true} ->
      OutKeepA acc.list
  ;;

  (* Note: OCaml infers the wrong type (no principal type), we have to help it here. *)
  let typed_fixpoint_step (type c) ctx (included, (In acc:c in_tuple)) : bool * (close:bool -> (c out_tuple)) =
    match acc.sub with
    | NoBottom l ->
      let bool,continuef = D.typed_fixpoint_step ctx (included, l) in
      bool,fun ~close ->
        let sub = continuef ~close in
        OutNoBottom sub
    | IsBottom{a_bottom=true} ->
      true, fun ~close -> OutNone
    | IsBottom{b_bottom} ->
      assert(b_bottom);
      true, fun ~close -> OutKeepA acc.list
  ;;

  (**************** Queries ****************)
  
   module Query = struct
    include D.Query
    let reachable ctx = function
      | None -> Quadrivalent_Lattice.Bottom
      | Some b -> D.Query.reachable ctx b

    let boolean ctx = function
      | None -> D.Query.Boolean_Lattice.bottom
      | Some b -> D.Query.boolean ctx b
    ;;
    let binary ~size ctx = function
      | None -> D.Query.Binary_Lattice.bottom ~size
      | Some b -> D.Query.binary ~size ctx b
    ;;
    let integer ctx = function
      | None -> D.Query.Integer_Lattice.bottom
      | Some b -> D.Query.integer ctx b
    ;;
  end

  let memory_is_empty _ = assert false
  let binary_is_empty ~size _ = assert false
  let integer_is_empty _ = assert false
  let boolean_is_empty _ = assert false

  let builtin_show_each_in string ctx args memory =
    let exception ExcNone in
    try 
      match memory with
      | None -> raise ExcNone
      | Some memory ->
        let args = List.map (function (size,None) -> raise ExcNone | (size,Some x) -> size,x) args in
        Some(D.builtin_show_each_in string ctx args memory)
    with ExcNone -> None
  (* let builtin b ~from_types ~to_types ctx l =
   *   match tuple_to_subtuple l with
   *   | None -> assert false
   *   | Some l ->
   *     let l = D.builtin b ~from_types ~to_types ctx l in
   *     subtuple_to_tuple l
   * ;; *)

  let assume_memory  = assume D.assume_memory
  let assume_binary ~size = assume @@ D.assume_binary ~size
  let assume_boolean = assume D.assume_boolean
  let assume_integer = assume D.assume_integer

  
  let binary_empty ~size ctx = Some(D.binary_empty ~size ctx)
  let integer_empty ctx = Some (D.integer_empty ctx)
  let boolean_empty ctx = Some (D.boolean_empty ctx)

  let binary_unknown  ~size ~level ctx = Some(D.binary_unknown ~level ~size ctx)
  let integer_unknown ~level  ctx = Some (D.integer_unknown ~level ctx)
  let boolean_unknown ~level  ctx = Some (D.boolean_unknown ~level ctx)

  
  let reachable ctx = function
    | None -> Domain_sig.Unsat
    | Some x -> D.reachable ctx x
  ;;

  let satisfiable ctx = function
    | None -> Domain_sig.Unsat
    | Some x -> D.satisfiable ctx x

  let union _ = assert false
  
  let should_focus ~size ctx memory addr =
    match memory,addr with
    | None,_ | _,None -> None
    | Some m, Some a ->
      begin
        match D.should_focus ~size ctx m a with
        | Some (b,size,ofs) -> Some (Some b, size, ofs)
        | None -> None
      end

  let may_alias ~size ctx addr1 addr2 =
    match addr1,addr2 with
    | None,_ | _,None -> true
    | Some a1, Some a2 -> D.may_alias ~size ctx a1 a2

end
