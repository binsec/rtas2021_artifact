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
(* Misc. utilitary functions. *)

open Term_constructors;;

(* When printing does not work... *)


let identify_kind string t =
  Codex_log.debug "In %s" string;
  match t with
  | Var _ -> assert false
  | Assume _ -> assert false
  | Nondet _ -> assert false
  | Mu _ -> assert false
  | Tuple_get _ -> assert false
  | Tuple_create _ -> assert false
  | True -> assert false
  | False -> assert false
  | And _ -> assert false
  | Or _ -> assert false
  | Not _ -> assert false
  | Biconst _ -> assert false
  | Buninit _ -> assert false     
  | Biadd _ -> assert false
  | Bisub _ -> assert false     
  | Bimul _ -> assert false
  | Biudiv _ -> assert false
  | Bisdiv _ -> assert false
  | Band _ -> assert false
  | Bor _ -> assert false
  | Bxor _ -> assert false
  | Beq _ -> assert false
  | Bisle _ -> assert false
  | Biule _ -> assert false
  | Bconcat _ -> assert false
  | Bextract _ -> assert false
  | Bsext _ -> assert false
  | Buext _ -> assert false
  | Bofbool _ -> assert false             
  | Iconst _ -> assert false
  | Iadd _ -> assert false
  | Isub _ -> assert false
  | Ieq _ -> assert false
  | Ile _ -> assert false            
  | Imul _ -> assert false
  | Idiv _ -> assert false
  | Imod _ -> assert false
  | Ior _ -> assert false
  | Iand _ -> assert false
  | Ixor _ -> assert false
  | Ishl _ -> assert false
  | Ishr _ -> assert false
  | Iunknown _ -> assert false                    
  | Itimes _ -> assert false
  | Bunknown _ -> assert false
  | Biumod _ -> assert false
  | Bismod  _ -> assert false
  | Bshl  _ -> assert false
  | Bashr  _ -> assert false
  | Blshr  _ -> assert false
  | Valid  _ -> assert false
  | Store  _ -> assert false
  | Load _ -> assert false
  | Lambda _ -> assert false
  | Separated  _ -> assert false
  | Offset _ -> assert false
  | Builtin _ -> assert false
  | Malloc _ -> assert false
  | Bshift _ -> assert false
  | Bindex _ -> assert false
  | Memcpy _ -> assert false    

let identify string term = identify_kind string term.kind


(**************** Free variables and max level. ****************)

(* This weak hash table allows to cache the calls to free_vars. *)
module Term_hashtbl = Ephemeron.K1.Make(Term_type.Term_datatype);;
module Term_set = Term_type.Term_set
let free_vars_hash: Term_set.t Term_hashtbl.t = Term_hashtbl.create 17;;

let rec free_vars term =
  try Term_hashtbl.find free_vars_hash term
  with Not_found ->
    let res = free_vars_ term in
    Term_hashtbl.replace free_vars_hash term res;
    res

and free_vars_set tl =
  Term_set.fold (fun x acc -> Term_set.union acc @@ free_vars x) tl Term_set.empty

and free_vars_list tl =
  List.fold_left (fun acc x -> Term_set.union acc @@ free_vars x) Term_set.empty tl

and free_vars_array ar =
  Immutable_dynamic_array.fold_left (fun acc x -> Term_set.union acc @@ free_vars x) Term_set.empty ar

and free_vars_ term =
  let open Term_constructors in
  match term.kind with
  | Var _ -> Term_set.singleton term
  | True | False | Biconst _ | Buninit _
  | Iconst _ | Bunknown _ | Iunknown _ -> Term_set.empty
  | Not t | Itimes(_,t) | Lambda(_,t) | Builtin(_,t) | Offset t
  | Bextract(_,_,t) |  Bsext(_,t) | Buext(_,t) | Valid(_,_,t) | Bofbool(_,t)
  | Tuple_get(t,_) -> free_vars t
  | Idiv(x,y) | Imod(x,y) | Imul(x,y) | Iadd(x,y) | Isub(x,y)
  | Ishl (x,y) | Ishr (x,y) | Iand (x,y) | Ior (x,y) | Ixor (x,y)
  | And(x,y) | Or(x,y)
  | Ieq(x,y) | Ile(x,y)
  | Load(_,x,y) | Separated(x,_,y,_) | Beq(_,x,y) | Assume(x,y)
  | Bisle(_,x,y) | Biule(_,x,y) | Bconcat(_,x,y)
  | Biadd(_,_,_,x,y)  | Bisub(_,_,_,x,y)  | Bimul(_,_,_,x,y) | Band(_,x,y) | Bor(_,x,y) | Bxor(_,x,y)
  | Bisdiv(_,x,y) | Biudiv(_,x,y) | Bismod(_,x,y) | Biumod(_,x,y)
  | Bashr(_,x,y) | Blshr(_,x,y) | Bshl(_,_,_,x,y) -> Term_set.union (free_vars x) (free_vars y)
  | Nondet(_,_,l) -> free_vars_set l
  | If_not_bottom(l,x) -> Term_set.union (free_vars x) @@ free_vars_set l
  | Store(_size,x,y,z) ->
    Term_set.union (Term_set.union (free_vars x) (free_vars y)) (free_vars z)
  | Mu(_time,_var,_loop,init) ->
    let initvars = free_vars init in
    let loop_vars = free_vars _loop in
    Term_set.union initvars (Term_set.remove _var loop_vars)
  | Tuple_create(_, l) -> free_vars_array l

(* The level of a variable term. *)
let level term =
  let open Term_constructors in
  match term.kind with
  | Var(_,_,lvl) -> lvl
  | _ -> assert false
;;


(* Not that simple: we have to collect all the levels seen in a body,
   to be able to retrieve the max when removing the inner-most loop. *)
module IntSet = Datatype_sig.Set(Datatype_sig.Int);;
let max_level_hash: IntSet.t Term_hashtbl.t = Term_hashtbl.create 17;;

(* This is wrong, notably for bunknowns. But maybe for nondet too. *)
(* TODO: Take what I had in term_deps. *)
let rec max_level term =
  try Term_hashtbl.find max_level_hash term
  with Not_found ->
    let res = max_level_ term in
    Term_hashtbl.replace max_level_hash term res;
    (* Kernel.feedback "term %d level %a" term.id IntSet.pretty res; *)
    res
and max_level_ term =
  let open Term_constructors in
  match term.kind with
  | Var(_,_,lvl) -> IntSet.singleton lvl
  | True | False | Biconst _ | Buninit _ | Iconst _ -> IntSet.singleton (-1) (* Constants. *)
  | Bunknown (_,(lvl,_)) | Iunknown (lvl,_) -> IntSet.singleton lvl
  | Not t | Itimes(_,t) | Lambda(_,t) | Builtin(_,t) | Offset t
  | Bextract(_,_,t) |  Bsext(_,t) | Buext(_,t) | Valid(_,_,t) | Bofbool(_,t)
  | Tuple_get(t,_) -> max_level t
  | Idiv(x,y) | Imod(x,y) | Imul(x,y) | Iadd(x,y) | Isub(x,y)
  | Ishl (x,y) | Ishr (x,y) | Iand (x,y) | Ior (x,y) | Ixor (x,y)
  | And(x,y) | Or(x,y)
  | Ieq(x,y) | Ile(x,y)
  | Load(_,x,y) | Separated(x,_,y,_) | Beq(_,x,y) | Assume(x,y)
  | Bisle(_,x,y) | Biule(_,x,y) | Bconcat(_,x,y)
  | Biadd(_,_,_,x,y)  | Bisub(_,_,_,x,y)  | Bimul(_,_,_,x,y) | Band(_,x,y) | Bor(_,x,y) | Bxor(_,x,y)
  | Bisdiv(_,x,y) | Biudiv(_,x,y) | Bismod(_,x,y) | Biumod(_,x,y)
  | Bashr(_,x,y) | Blshr(_,x,y) | Bshl(_,_,_,x,y) -> max (max_level x) (max_level y)
  | Nondet((lvl,_),_,l) -> Term_set.fold (fun x s ->
      IntSet.union s @@ max_level x) l (IntSet.singleton lvl) 
  (* | If_not_bottom(l,x) -> Term_set.union (free_vars x) @@ free_vars_set l *)
  | Store(_size,x,y,z) -> IntSet.union (max_level x) @@ IntSet.union (max_level y) (max_level z)
  | Mu((lvl,_),_var,_loop,init) ->
    let body_lvl = level _var in
    (* Kernel.feedback "init %a loop %a body_lvl %d lvl %d result %a" IntSet.pretty (max_level init) IntSet.pretty (max_level _loop) body_lvl lvl IntSet.pretty (    IntSet.union (max_level init) (IntSet.remove lvl @@ max_level _loop)); *)
    IntSet.union (max_level init) (IntSet.remove body_lvl @@ max_level _loop)
  | Tuple_create(_, l) -> l |> Immutable_dynamic_array.map max_level |> Immutable_dynamic_array.to_list |> Extstdlib.List.reduce IntSet.union

  
  (* let free_vars = free_vars term in *)
  (* Term_set.fold  (fun (var:Term_type.term) (acc:int) -> *)
  (*     let open Term_constructors in *)
  (*     match var.kind with *)
  (*     | Var(_,_,lvl) -> if acc >= lvl then acc else lvl *)
  (*     | _ -> assert false) free_vars (-1) *)
;;

let max_level t = IntSet.max_elt @@ max_level t
(* let max_level t = let res = max_level t in *)
(*   if res > 5 then Kernel.feedback assert (res < 5); res *)
