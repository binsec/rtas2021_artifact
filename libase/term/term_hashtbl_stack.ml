(**************************************************************************)
(*                                                                        *)
(*  This file is part of the Codex plugin of Frama-C.                     *)
(*                                                                        *)
(*  Copyright (C) 2007-2016                                               *)
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

(* Note: using Weak hash tbl may not be ok here: as we have mem, 
   it is possible that some code use mem and then assume that 
   the binding is present. *)
module Term_Hashtbl = Term_type.Term_hashtbl
(* module Term_Hashtbl = Ephemeron.K1.Make(Term_type.Term_datatype) *)

type 'a t = ('a Term_Hashtbl.t) list

let create () = [Term_Hashtbl.create 17];;

(* Find looks things on the stack, while replace always replaces
   on top. [maybe_find] should be used in rare occasions, e.g. the
   pretty-printer. *)
let rec maybe_find x v = match x with
  | [] ->  None
  | a::b ->
     try Some(Term_Hashtbl.find a v)
     with Not_found -> maybe_find b v

let rec find_exn x v = match x with
  | [] -> raise Not_found
  | a::b ->
     try Term_Hashtbl.find a v
     with Not_found -> find_exn b v

let find x v =
  try find_exn x v
  with Not_found -> Term_utils.identify "find" v
(* Should not happen; probably we forgot to evaluate something. *)

let rec mem x v = match x with
  | [] -> false
  | a::b ->
     if Term_Hashtbl.mem a v
     then true
     else mem b v

(* TODO: replace and update using the highest possible term.
   Careful when backward propagating: the condition may have a
   depth further than the value.  *)
let replace x = Term_Hashtbl.replace (List.hd x)

(* Note: using references, we could avoid the need to hash the
   term twice. *)
let update ~default x key f =
  let hd = List.hd x in
  let orig =
    try (find_exn x key)
    with Not_found -> default
  in Term_Hashtbl.replace hd key (f orig)      
;;

let push x = (Term_Hashtbl.create 17)::x
