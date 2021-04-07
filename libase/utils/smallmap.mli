(**************************************************************************)
(*                                                                        *)
(*  This file is part of the Codex plugin of Frama-C.                     *)
(*                                                                        *)
(*  Copyright (C) 2007-2019                                               *)
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

(* Memory-efficient replacement for maps, useful when we have a lot of
   small maps. *)

module Make (Ord : Map.OrderedType) : sig
  type key = Ord.t
  type 'a t
           
  val bindings: 'a t -> (key * 'a) list
  val add: key -> 'a -> 'a t -> 'a t
  val empty: 'a t
  val find: key -> 'a t -> 'a
  val fold: (key -> 'a -> 'b -> 'b) -> 'a t -> 'b -> 'b
    
end 
