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

(* In theory this should be the signature for the "Term" package. *)
(* TODO: Compile "Term" as a separate package or library. *)

include Datatype_sig.S
  with type t =
         (Term_type.Var.t, Term_type.Term_set.t,
          Z.t Term_type.Term_map.t, Term_type.Parents.t)
           Term_constructors._term

module Map:Extstdlib.Map.S with type key = t
module Set:Extstdlib.Set.S with type elt = t and type t = Term_type.Term_set.t
module Hashtbl:Hashtbl.S with type key = t

val pretty_short: Format.formatter -> t -> unit
val pretty_type: Format.formatter -> Term_types.t -> unit
  
type size = int
val size_of: t -> size

type term_type = Term_constructors.term_type

module Var:Datatype_sig.S with type t = Term_type.Var.t

val type_of: t -> term_type

module Hashtbl_Stack:module type of Term_hashtbl_stack

module WeakHashTbl:Ephemeron.S with type key = t

module SmartCons:module type of Term_smart_constructors

val identify: string -> t -> 'a

(* Returns the set of free variables of a term, as a set of (Var x) terms. *)
val free_vars: t -> Term_type.Term_set.t

(* Returns the maximum level, i.e. the outermost loop where a term can
   be evaled. A term has level x if it or one of its subters it is a
   variable with de Bruijn level x. Constants have a level of -1. *)
val max_level: t -> int

(* http://research.microsoft.com/en-us/um/people/leonardo/mit2011.pdf
   Reduction des fonctions commutatives vers uninterpreted functions.
 *)

val used_more_than_once: t -> bool

