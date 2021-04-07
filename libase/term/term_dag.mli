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

open Term_type;;

(* Retrieve a list of immediate subterms for a repr or a term.   *)
val subterms: term -> term list
val subterms_kind: kind -> term list

(* Iterator to iterate on terms in postfix DFS order. Each term is
   visited only once. *)
val postfix_dfs: (term -> unit) -> term -> unit

(* Terms can be viewed as a DAG where each node is a term, and there
is an edge from a to b if b is an immediate subterm of a. Moreover,
the increasing unique id that is assigned to terms allows to sort them
topologically. Topological iteration thus traverses the nodes from
roots to leafs (i.e. from terms to subterms, which corresponds to
backward evaluation), while reverse topological iteration corresponds
to forward evaluation. *)

(* Given a list of roots, recursively traverse all the nodes while
   they satisfy [filter]; then iterate on them using reverse
   topological order. *)
val reverse_topological_iter: term list -> (term -> bool) -> (term -> unit) -> unit

(* Worklist-based iteration: [worklist init f] first populates a
worklist with [init], then iteratively calls f, which removes an
element from a worklist and returns a list of elements to add to the
worklist; until the worklist is empty. [topogical_worklist] always
selects the element closest to the root, while
[reverse_topological_iter] selects the elements closest to the
leaves. *)
val topological_worklist: term list -> (term -> term list) -> unit;;
val reverse_topological_worklist: term list -> (term -> term list) -> unit;;  

(* TODO: Utiliser la terminologie to_subterms/from_subterms, plus claire a mon avis. *)
(* TODO: Worklist-based iteration with a fixpoint. *)
module To_Subterms_Dataflow(L:sig
    type t
    val join: t -> t -> t                        (* Used when a term is the subterm of two terms. *)
    (* val widen: previous:t -> t -> t              (\* Used in mus. *\) *)
    val includes: t -> t -> bool                 (* Used in mus. *)
    val pretty: Format.formatter -> t -> unit    (* Pretty-printer *)
    val transfer: term -> t -> (term * t) list   (* Returns the new value for the subterms. *)
  end):sig
  val compute: term -> L.t -> L.t Term_type.Term_hashtbl.t (* Compute for an initial term and value. *)
end

