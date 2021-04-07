(**************************************************************************)
(*                                                                        *)
(*  This file is part of the Codex plugin of Frama-C.                     *)
(*                                                                        *)
(*  Copyright (C) 2007-2015                                               *)
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
(* Default smart constructors. *)

module No_rewrite = Term_smart_constructors_no_rewrite
module Administrative_reduction = Term_smart_constructors_administrative_reduction.Make
module Bdd_rewrite = Term_smart_constructors_bdd_rewrite.Make                                    

module I1 = No_rewrite
module I2 = Administrative_reduction(I1)
module I3 = (* Bdd_rewrite *)(I2)
(* TODO: Normalize commutativity, rewrite to linear forms, constant propagation, tseitin transformation... *)

module I = I3

module OldTuple = I.Tuple
module I':module type of I with module Tuple := OldTuple = I
include I'
module Tuple = struct
  open Term_constructors;;
  include OldTuple;;
  
  (* [Eager_evaluating_domain] assumes that this rewrite exists. *)
  let tuple_get tuple i =
  match tuple.kind with
  | Tuple_create(_,array) -> Immutable_dynamic_array.get array i
  | _ -> tuple_get tuple i
end
