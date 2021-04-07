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

module Var = Term_type.Var;;
type size = int

type term_type = Term_constructors.term_type

include Term_type.Term_datatype
module Map = Term_type.Term_map
module Set = Term_type.Term_set
module Hashtbl = Term_type.Term_hashtbl
module Term_datatype = Term_type.Term_datatype

let size_of = Term_type_inference.size_of
include Term_smart_constructors;;
let type_of = Term_type_inference.type_of

let pretty_short = Term_pretty.pretty_short
let pretty_type = Term_pretty.pretty_type

(* let subst = Term_subst.subst *)

module Hashtbl_Stack = Term_hashtbl_stack

let identify = Term_utils.identify
let free_vars = Term_utils.free_vars
let max_level = Term_utils.max_level

module WeakHashTbl = Ephemeron.K1.Make(Term_datatype)

module SmartCons = Term_smart_constructors

let used_more_than_once x = Term_type.Parents.length_more_than x.Term_constructors.parents 2
