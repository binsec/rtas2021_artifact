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

open Term_type;;

val identify: string -> term -> 'a
val identify_kind: string -> kind -> 'a

(* Returns the set of free variables of a term, as a set of (Var x) terms. *)
val free_vars: term -> Term_set.t

(* Returns the maximum level, i.e. the outermost loop where a term can
   be evaled. A term has level x if it or one of its subters it is a
   variable with de Bruijn level x. *)
val max_level: term -> int

(* For variale terms, the level of the term. Correspond to max_level
   for these terms. *)
val level: term -> int
