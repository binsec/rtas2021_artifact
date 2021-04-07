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

(* Stack of hashtbl, used to ditch the results of a fixpoint
   computation by just removing the lastest element on the stack. 
   
   Very useful to implement abstract domains whose dimension
   identifiers are terms. *)


open Term_type

(* The type of stack of hash tables. The stack is persistent (non-head
   elements can still be used), but the hash tables they contain are
   imperative. *)
type 'a t
val create: unit -> 'a t
val push: 'a t -> 'a t

(* Find iterates on the stack trying to find a value for the term in each hash table,
   stopping on the first it finds (if any). *)
val maybe_find: 'a t -> term -> 'a option (* Find and return None if not found. *)
val find: 'a t -> term -> 'a    (* Find and fails if not found. *)
val find_exn: 'a t -> term -> 'a (* Find and raise an exception if not found *)
val mem: 'a t -> term -> bool

(* Find and replace. *)
val update: default:'a -> 'a t -> term -> ('a -> 'a) -> unit

val replace: 'a t -> term -> 'a -> unit
