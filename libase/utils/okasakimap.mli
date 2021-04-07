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

(* Implementation of Chris Okasaki's Fast mergeable interval maps. *)

(* All functions try to preserve physical equality whenever possible. *)
module type S = sig
  type 'a t
  type key
  val empty: 'a t
  val singleton: key -> 'a -> 'a t
  val find: key -> 'a t -> 'a
  (* The function tells what to do when a value was already
     present. *)
  val insert : (old:'a -> value:'a -> 'a) -> key -> 'a -> 'a t -> 'a t
  val add : key -> 'a -> 'a t -> 'a t

  (* As we do not store the keys in the map, we can only access its id. *)
  val unioni : (int -> 'a -> 'a -> 'a) -> 'a t -> 'a t -> 'a t
  val interi : (int -> 'a -> 'a -> 'a) -> 'a t -> 'a t -> 'a t  
  
  val union : ('a -> 'a -> 'a) -> 'a t -> 'a t -> 'a t
  val inter : ('a -> 'a -> 'a) -> 'a t -> 'a t -> 'a t

  val iter: 'a t -> (int -> 'a -> unit) -> unit
  
  (* union_inter funion finter a b c is equivalent to union funion ta @@ inter finter tb tc. *)
  (* val union_inter:
   *   funion:('a -> 'a -> 'a) ->
   *   finter:('a -> 'a -> 'a) -> 'a t -> 'a t -> 'a t -> 'a t *)

end

include S with type key = int

(* Note: This requires keys to be uniquely identified by an integer,
   i.e. (Key.to_int a) == (Key.to_int b) means (Key.equal a b) (to_int
   is injective). *)
module Make(Key:sig
  type t
  val to_int: t -> int
end):S with type key = Key.t
