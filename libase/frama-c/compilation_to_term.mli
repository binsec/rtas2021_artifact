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

open Codex
module Assertion_Table:sig
  type t
  type kind =
    | SVComp of Cil_types.location
    | Alarm of Alarms.t * Cil_types.location

  val pretty_kind: Format.formatter -> kind -> unit
  val iter: t -> (Term.t -> kind -> unit) -> unit
end

module Reachable_Table:sig
  type t
  val iter: t -> (Term.t -> unit) -> unit
  val is_empty: t -> bool
end


module Callsite:Datatype_sig.S
  with type t = Kernel_function.t * Cil_types.kinstr
module Callstack_Trie:module type of Mutable_radix_trie.Make(Callsite)


(* Returns the exit term and assertions for a function. *)
val compile: Kernel_function.t -> (Term.t option * Assertion_Table.t * Reachable_Table.t *
                                   (Cil_types.kinstr * Cil_types.exp -> Term.t Callstack_Trie.t));;
