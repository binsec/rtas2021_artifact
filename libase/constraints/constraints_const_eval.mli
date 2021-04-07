(**************************************************************************)
(*                                                                        *)
(*  This file is part of the Codex plugin of Frama-C.                     *)
(*                                                                        *)
(*  Copyright (C) 2007-2018                                               *)
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

(* Evaluation of constant constraints. *)

module Make(Constraints:Constraints_constraints_sig.Constraints):sig

  exception Empty
  exception Not_a_constant

  (* Returns the value if the constraint is indeed a value; empty if
     it is the empty value; and not a singleton if it is not constant. *)
  val integer: Term_constructors_gadt.integer Constraints.t -> Z.t
  val boolean: Term_constructors_gadt.boolean Constraints.t -> bool
  (* Also returns the size of the constraint. *)
  val binary: Term_constructors_gadt.binary Constraints.t -> Z.t
  

end
