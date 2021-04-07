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
open Term_constructors;;
open Term_types;;
(* This infers the type, but does not attempt to type check:
   we return as soon as possible.

   TODO: Possibly, memoize the results. I doubt this is useful: in
   most cases, computing the type is trivial. Maybe: we should store
   the type in the term.*)
let rec type_of t = match t.kind with
  | Var({contents=types}, _,_) -> Term_types.Tuple (Lazy.from_val types)
  | True | False | And _ | Or _ | Not _ | Beq _ | Valid _
  | Bisle _ | Biule _ -> Boolean
  | Load(s,_,_) | Biadd(s,_,_,_,_) | Bisub(s,_,_,_,_) 
  | Bimul(s,_,_,_,_) | Biudiv(s,_,_) | Bisdiv(s,_,_)
  | Band(s,_,_) | Bor(s,_,_) | Bxor(s,_,_)
  | Bextract(s,_,_) | Buext(s,_) | Bsext(s,_) | Bofbool(s,_) | Bconcat(s,_,_)
  | Biconst(s,_) | Bshift(s,_,_,_)| Bindex(s,_,_,_)
  | Bshl(s,_,_,_,_) | Bashr(s,_,_) | Blshr(s,_,_)
  | Bismod(s,_,_) | Biumod(s,_,_) | Bunknown(s,_) | Buninit(s) -> Binary(s)
  | Nondet(_,types,_) -> Tuple (Lazy.from_val types)
  | Assume(_,x) | If_not_bottom(_,x) -> type_of x
  (* Better take the type information from the variables (fast), as getting
     the type from a Tuple_create constructor can be slow. *)
  | Mu(_,var,_,_) -> type_of var
  | Store(_,_,_,_) | Memcpy(_,_,_,_) | Memunknown _ -> Memory
  (* Avoids calls to map. *)
  | Tuple_get({kind=Tuple_create(_, l)},i) -> type_of (Immutable_dynamic_array.get l i)
  | Malloc(_,_,_) ->
    Term_types.Tuple(lazy (Immutable_dynamic_array.from_list [Binary(Codex_config.ptr_size());Memory]))
  | Free(_,_) -> Memory
  | Tuple_get({kind=Malloc _},0) -> Binary(Codex_config.ptr_size())
  | Tuple_get({kind=Malloc _},1) -> Memory
  | Tuple_get(a,i) ->
     let Tuple(v) = type_of a in
     Immutable_dynamic_array.get (Lazy.force_val v) i
  | Tuple_create(t, l) -> Tuple(t)
  | Iconst _ | Iadd (_, _) | Imul (_, _) | Idiv (_, _) | Imod (_, _) | Isub (_,_)
  | Itimes (_, _) | Ieq _ | Ile _ | Ishl (_, _) | Ishr (_, _) 
  | Iand (_, _) | Ior (_, _) | Ixor (_, _) | Iunknown _ -> Integer
  | Builtin(b,t) ->
    begin match b with
      | Builtin.Show_each_in _ -> 
        Term_types.Tuple (lazy (Immutable_dynamic_array.singleton Term_types.Memory))
    end
    (* let Term_types.Tuple types = type_of t in
     * Term_types.Tuple (lazy (b.Builtin.typ @@ Lazy.force types)) *)
  | _ -> Term_utils.identify "Term_type_inference.type_of" t


(* Get the size of a binary term. *)
let rec size_of term = match term.kind with
  | Biadd(size,_,_,_,_) | Bisub(size,_,_,_,_) | Biconst(size,_) -> size
  | Bimul(size,_,_,_,_) | Band(size,_,_) | Bor(size,_,_) | Bxor(size,_,_) -> size
  | Beq(size,_,_) -> size
  | Bconcat(size,_,_) | Bextract(size,_,_) | Buext(size,_) | Bsext(size,_) | Bofbool(size,_)
  | Bshift(size,_,_,_) | Bindex(size,_,_,_) 
  | Bisdiv(size,_,_) | Biudiv(size,_,_) | Bismod(size,_,_) | Biumod(size,_,_) | Bunknown(size,_)
  | Bshl(size,_,_,_,_) | Bashr(size,_,_) | Blshr(size,_,_) -> size
  | Load(size,_,_) | Buninit(size) -> size
  | Assume(_,t) | If_not_bottom(_,t) -> size_of t
  (* Size through inference. MAYBE: SHould always be the case? *)
  (* MAYBE: cache the call to "typ" in the builtin. *)
  | Tuple_get _ | Builtin _ ->
     (match type_of term with
     | Binary size -> size
     | _ -> Codex_log.fatal "not a binary term")
  (* Not handled, or not a binary term *)
  | _ -> Term_utils.identify "Term_type_inference.size_of" term

;;
