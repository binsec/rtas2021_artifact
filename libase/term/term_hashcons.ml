(**************************************************************************)
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

open Term_type;;
open Term_constructors;;
open Term_pretty;;
open Term_types;;

module Log = Codex_log.Make(struct
    let category = "hashcons";;
    let debug = false
  end);;

(* Computation of hashes of term kinds (and equality between term kinds) *)
module Term_Kind_Hash = struct

  let hash_pair x y = x * 599 + y * 799
  let hash_triple x y z = x * 281 + y * 599 + z * 799
  let hash_quadruple x y z u  = x * 281 + y * 599 + z * 799 + u * 997
  let rec hash_list hash_elt initial_hash = function
    | [] -> initial_hash
    | x::xs -> hash_list hash_elt (initial_hash * 599 + hash_elt x) xs

  let hash_set hash_elt initial_hash set =
    Term_set.fold (fun x acc -> acc * 599 + (hash_elt x)) set initial_hash

  let hash_head x = Obj.tag (Obj.repr x)

  let hash_time (i,j) = hash_pair i j
  
  let rec hash_type = function
    | Memory -> 0
    | Boolean -> 1
    | Integer -> 2
    | Binary size -> 3 + 17 * size
    | Tuple x -> 4 + 97 * hash_types (Lazy.force_val x)

  and hash_types l = (hash_list hash_type 1 (Immutable_dynamic_array.to_list l))

  let hash_access_type = function
  | Transfer_functions_sig.Read -> 0
  | Transfer_functions_sig.Write -> 1

  (* Note: we use the id, rather than the hashes, of the subterms,
     because 1. they are more unique, and 2. they do not have to be
     re-computed recursively. *)
  let hash_subterms = function
    | True -> 32
    | False -> 34
    | Builtin(b,args) -> Builtin.hash b + 281 * args.id
    | Buninit(size) -> size + 36
    | Iconst(u) -> 799 * (Z.hash u) + 281
    | Tuple_get(t,i) -> t.id + i
    | Tuple_create(_, a) -> hash_list (fun x -> x.id) 0 (Immutable_dynamic_array.to_list a)
    | Bextract(a,b,t) -> t.id + a*281 + b *111
    | Bshift(a,k,_,t) -> t.id + a*281 + 3 + k * 599
    | Bindex(a,t,k,e) -> t.id + a*281 + 3 + k * 599 + 977 * e.id
    | Bofbool(a,t) -> t.id + a*281 + 3                       
    | Valid(a,at,t) -> t.id + a*281 + 2 + hash_access_type at
    | Buext(a,t) -> t.id + a*281 + 1
    | Bsext(a,t) -> t.id + a*281
    | Bunknown(_,t) -> 899 + hash_time t
    | Iunknown t -> 13811 + hash_time t
    | Memunknown t -> 19231 + hash_time t                      
    | Not t | Offset t -> 1 + t.id
    | Itimes(n,t) -> Z.hash n * t.id
    | Idiv(x,y) | Imod(x,y) | Imul(x,y) | Iadd(x,y) | Isub(x,y) | Ieq(x,y) | Ile(x,y)
    | Ishl (x,y) | Ishr (x,y) | Iand (x,y) | Ior (x,y) | Ixor (x,y)
    (* TODO: change the hash for each constructors, maybe using the tag *)
    | And(x,y) | Or(x,y)
    | Biadd(_,_,_,x,y) | Bisub(_,_,_,x,y) | Bimul(_,_,_,x,y) | Bor(_,x,y) | Band(_,x,y) | Bxor(_,x,y) | Bconcat(_,x,y)
    | Beq(_,x,y) | Bisle(_,x,y) | Biule(_,x,y) | Assume(x,y)
    | Bisdiv(_,x,y) | Biudiv(_,x,y) | Bismod(_,x,y) | Biumod(_,x,y)
    | Bshl(_,_,_,x,y) | Bashr(_,x,y) | Blshr(_,x,y) | Free(x,y)
    | Valid_ptr_arith(_,x,y) -> hash_pair x.id y.id
    | Nondet(w,t,s) -> hash_types t + hash_set (fun t -> t.id) (hash_time w) s
    | If_not_bottom(s,y) -> hash_set (fun t -> t.id) y.id s
    | Var(_, x,_) -> Var.hash x
    | Store(size,store,addr,value) ->
      hash_quadruple addr.id (Hashtbl.hash size) value.id store.id
    | Memcpy(size,store,from,to_) ->
      1+ hash_quadruple from.id (Hashtbl.hash size) to_.id store.id        
    | Load(size,store,addr) ->
      hash_triple addr.id (Hashtbl.hash size) store.id
    | Malloc(id,size,mem) -> hash_triple (Transfer_functions.Malloc_id.hash id) size mem.id
    | Lambda(var,body) -> (Var.hash var) + 281 * body.id
    | Biconst(size,const) -> (Z.hash const) + 281 * size
    | Separated(addra,sizea,addrb,sizeb) ->
      hash_quadruple addra.id sizea addrb.id sizeb
    | Mu(time,v,loop,init) ->
      hash_quadruple (hash_time time) v.id loop.id init.id
    (* | x -> Term_utils.identify_kind "hash_subterms" x *)

  let hash_kind t = hash_head t + 31 * (hash_subterms t)

  let rec equal_type x1 x2 = match x1, x2 with
    | Binary s1, Binary s2 -> s1 == s2
    | Memory, Memory -> true
    | Boolean, Boolean -> true
    | Integer, Integer -> true
    | Tuple t1, Tuple t2 -> equal_types (Lazy.force_val t1) (Lazy.force_val t2)
    | _ -> false
       
  and equal_types t1 t2 = Immutable_dynamic_array.for_all2 equal_type t1 t2
  ;;


  (* This should be called only by the Hashtbl functor, and when the
     hashes of t1 and t2 are equal. *)
  let equal_kind t1 t2 =
    Log.debug "equal_kind: %a %a"
      (pretty_kind pretty_simple) t1
      (pretty_kind pretty_simple) t2;

    (* Same as List.for_all2, but lists without the same length count
       as false. *)
    let list_for_all2 f l1 l2 = 
      try List.for_all2 f l1 l2
      with Invalid_argument _ -> false
    in

    match (t1,t2) with
    | True, True | False, False -> true
    | Itimes(k1,t1), Itimes(k2,t2) -> Z.equal k1 k2 && t1 == t2
    | Iadd(a1,b1), Iadd(a2,b2)
    | Isub(a1,b1), Isub(a2,b2)
    | Ieq(a1,b1), Ieq(a2,b2)
    | Ile(a1,b1), Ile(a2,b2)                        
    | Imul(a1,b1), Imul(a2,b2)
    | Idiv(a1,b1), Idiv(a2,b2)
    | Imod(a1,b1), Imod(a2,b2)
    | Ishl(a1,b1), Ishl(a2,b2)
    | Ishr(a1,b1), Ishr(a2,b2)
    | Iand(a1,b1), Iand(a2,b2)
    | Ixor(a1,b1), Ixor(a2,b2)
    | Ior(a1,b1), Ior(a2,b2)
    | And(a1,b1), And(a2,b2)
    | Or(a1,b1), Or(a2,b2)
    | Free(a1,b1), Free(a2,b2)
      -> a1 == a2 && b1 == b2
    | Not(t1), Not(t2) | Offset(t1), Offset(t2) ->
      t1 == t2
    | Iconst k1, Iconst k2 -> Z.equal k1 k2
    | Store(i1,store1,add1,val1), Store(i2,store2,add2,val2) ->
      add1 == add2 && i1 == i2 && val1 == val2
      && store1 == store2
    | Memcpy(i1,store1,add1,val1), Memcpy(i2,store2,add2,val2) ->
      add1 == add2 && i1 == i2 && val1 == val2
      && store1 == store2         
    | Load(i1,store1,add1), Load(i2,store2,add2) ->
      add1 == add2 && i1 == i2 && store1 == store2
    | Var(_,v1,_), Var(_,v2,_) -> Var.equal v1 v2
    | Lambda(v1,body1), Lambda(v2,body2) ->
      Var.equal v1 v2 && body1 == body2
    | Separated(a1,sizea1,b1,sizeb1), Separated(a2,sizea2,b2,sizeb2) ->
      a1 == a2 && sizea1 == sizea2 && b1 == b2 && sizeb1 == sizeb2
    | Mu(t1,v1,a1,b1), Mu(t2,v2,a2,b2) ->
      if (t1 == t2)
      then (assert(v1 == v2 && a1 == a2 && b1 == b2);
            true)
      else false
    | Beq(size1,a1,b1),Beq(size2,a2,b2)
    | Bisle(size1,a1,b1),Bisle(size2,a2,b2)
    | Biule(size1,a1,b1),Biule(size2,a2,b2)
    | Bashr(size1,a1,b1),Bashr(size2,a2,b2)
    | Blshr(size1,a1,b1),Blshr(size2,a2,b2)
    | Band(size1,a1,b1),Band(size2,a2,b2)
    | Bor(size1,a1,b1),Bor(size2,a2,b2)
    | Bxor(size1,a1,b1),Bxor(size2,a2,b2)
    | Bconcat(size1,a1,b1),Bconcat(size2,a2,b2)
    | Bisdiv(size1,a1,b1),Bisdiv(size2,a2,b2)
    | Biudiv(size1,a1,b1),Biudiv(size2,a2,b2)
    | Bismod(size1,a1,b1),Bismod(size2,a2,b2)
    | Biumod(size1,a1,b1),Biumod(size2,a2,b2) ->
      size1 == size2 && a1 == a2 && b1 == b2
    | Bimul(size1,nsw1,nuw1,a1,b1),Bimul(size2,nsw2,nuw2,a2,b2)
    | Biadd(size1,nsw1,nuw1,a1,b1),Biadd(size2,nsw2,nuw2,a2,b2)
    | Bisub(size1,nsw1,nuw1,a1,b1),Bisub(size2,nsw2,nuw2,a2,b2)
    | Bshl(size1,nsw1,nuw1,a1,b1),Bshl(size2,nsw2,nuw2,a2,b2) ->
      size1 == size2 && a1 == a2 && b1 == b2 && nsw1 = nsw2 && nuw1 = nuw2      
    | Bextract(size1,idx1,b1),Bextract(size2,idx2,b2) ->
       size1 == size2 && idx1 == idx2 && b1 == b2
    | Bshift(size1,k1,max1,b1),Bshift(size2,k2,max2,b2) ->
      size1 == size2 && k1 == k2 && b1 == b2 && max1 == max2
    | Bindex(size1,b1,k1,e1),Bindex(size2,b2,k2,e2) ->
      size1 == size2 && k1 == k2 && b1 == b2 && e1 == e2                                    
    | Biconst(size1,k1),Biconst(size2,k2) ->
       size1 == size2 && Z.equal k1 k2
    | Valid(size1,t1,b1),Valid(size2,t2,b2) ->
      size1 = size2 && t1 = t2 && b1 == b2
    | Bofbool(size1,b1),Bofbool(size2,b2)      
    | Bsext(size1,b1),Bsext(size2,b2)
    | Buext(size1,b1),Buext(size2,b2) ->
      size1 == size2 && b1 == b2
    | Malloc(id,size,mem), Malloc(id2,size2,mem2) -> id = id2 && size = size2 && mem == mem2
    | Builtin(b1,a1), Builtin(b2,a2) -> a1 == a2 && Builtin.equal b1 b2
    | Assume(c1,t1), Assume(c2,t2) -> c1 == c2 && t1 == t2
    | If_not_bottom(a1,b1), If_not_bottom(a2,b2) -> b1 == b2 && Term_set.equal a1 a2
    | Nondet(w1,t1,s1), Nondet(w2,t2,s2) ->
      (* Note: we use the world to uniquely identify the term. *)
      if w1 == w2 then 
        (assert (equal_types t1 t2 && Term_set.equal s1 s2);
         true)
      else false
    | Tuple_create(_, ar1), Tuple_create(_, ar2) ->
       Immutable_dynamic_array.length ar1 == Immutable_dynamic_array.length ar2
       && Immutable_dynamic_array.for_all2 (==) ar1 ar2
    | Tuple_get (t1, i1), Tuple_get (t2, i2) ->
       i1 == i2 && t1 == t2
    | Bunknown(s1,t1), Bunknown(s2,t2) -> t1 == t2 && (assert (s1 == s2); true)
    | Iunknown t1, Iunknown t2 -> t1 = t2
    | Memunknown t1, Memunknown t2 -> t1 = t2                                  
    | Buninit(s1), Buninit(s2) -> (assert (s1 == s2); true)       
    | _,_ ->
      (if (Obj.tag (Obj.repr t1) == Obj.tag (Obj.repr t2))
       then let sub1 = Term_dag.subterms_kind t1 in
            let sub2 = Term_dag.subterms_kind t2 in
            if list_for_all2 (==) sub1 sub2
            then Log.error
              "Either the case %d %a, %a was forgotten in equal_kind, \
or the hash function is not very good..." (Obj.tag (Obj.repr t1))
              (pretty_kind pretty_simple) t1 (pretty_kind pretty_simple) t2);
      false
end

module WeakHash = Weak.Make(struct
  type t = term
  let equal a b = Term_Kind_Hash.equal_kind a.kind b.kind
  let hash a = Term_Kind_Hash.hash_kind a.kind
end)

module Intern = struct
  let htbl = WeakHash.create 97
  let id_counter = ref 1
  (* MAYBE: Pre-create Parents.create() too? *)

  (* Intern is the function that returns the canonical term from a
     term kind. In addition, we initialize some information such as
     the list of parents of a term. *)
  (* Note the way that WeakHash works: you have to create the
     hashconsed value before trying to put it in the hashtable; and
     check afterwards if it was already in the table. *)
  let intern kind =
    Log.debug "trying interning %a" (pretty_kind pretty_simple) kind;
    let id = !id_counter in
    let tentative_term = { id; kind; parents = Parents.create()} in
    let term = WeakHash.merge htbl tentative_term in
    if term == tentative_term then begin
      Log.debug "interning %a: id %d" (pretty_kind pretty_simple) kind id;
      incr id_counter;
      let update_parent_of subterm = Parents.add subterm.parents term in
      let subterms = (Term_dag.subterms_kind kind) in
      List.iter update_parent_of subterms;
    end;

    term
end

let intern = Intern.intern;;  
