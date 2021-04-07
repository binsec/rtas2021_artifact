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

(* Type and datatype definitions for term.  *)

(* Abstract type for variables. Variables are uniquified, i.e.
   variables at different binding sites are different (as opposed to
   the use of De Bruijn indexes). This allows to attach to each
   variable the set of values it has during execution. *)
module Var:sig
  include Datatype_sig.S
  val create: unit -> t
end

module rec Term_datatype:(Datatype_sig.S
    with type t = (Var.t, Term_set.t, Z.t Term_map.t, Parents.t) Term_constructors._term)
and Parents:sig
    type t
    val create: unit -> t (* Note: Initially empty. *)
    val add: t -> Term_datatype.t -> unit
    val iter: (Term_datatype.t -> unit) -> t -> unit
    val fold: (Term_datatype.t -> 'a -> 'a) -> t -> 'a -> 'a
    val to_list: t -> Term_datatype.t list
    val length_more_than: t -> int -> bool
end
and Term_set:(Extstdlib.Set.S with type elt = Term_datatype.t)
and Term_map:(Extstdlib.Map.S with type key = Term_datatype.t)


(* Note: comparison is done using the terms id, i.e. terms are sorted
   by recency (which also corresponds to a topological ordering of the
   term DAG). *)

type term = Term_datatype.t
type kind = (Var.t, term,
             Term_set.t,
             Z.t Term_map.t) Term_constructors._kind

(* For internal use only. *)
val ref_fprint_term: (Format.formatter -> term -> unit) ref;;

module Term_hashtbl:Hashtbl.S with type key = term
