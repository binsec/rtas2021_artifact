(**************************************************************************)
(*                                                                        *)
(*  This file is part of Codex.                                           *)
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
module type Letter = sig
  type t
  val equal : t -> t -> bool
  val pretty : Format.formatter -> t -> unit
  val hash : t -> int
end

module type S = sig
  type letter

  type t

  val empty : t
  val epsilon : t

  val join : t -> t -> t
  val append : t -> letter -> t
  val append_star : t -> t -> t
  val equal : t -> t -> bool
  val hash : t -> int
  val pretty : Format.formatter -> t -> unit
end

(** Tagged regular expressions.  The tag is used for cons-hashing and share as
    many data structures as possibles. Tags should not be used outside of the
    implementation. *)
type 'l tagged_regex =
  | Empty
  | Epsilon
  | Join of int * 'l tagged_regex * 'l tagged_regex
  | Append of int * 'l tagged_regex * 'l
  | AppendStar of int * 'l tagged_regex * 'l tagged_regex

module Make (L : Letter) : S
    with type letter = L.t
    and type t = L.t tagged_regex
= struct
  type letter = L.t

  let empty = Empty
  let epsilon = Epsilon

  module HashedRegex = struct
    type t = L.t tagged_regex

    let tag = function
    | Empty -> 0
    | Epsilon -> 1
    | Join (x,_,_) -> x
    | Append (x,_,_) -> x
    | AppendStar (x,_,_) -> x

    let shallow_equal x y =
      tag x = tag y

    let equal x y = match x, y with
    | Empty,Empty -> true
    | Epsilon,Epsilon -> true
    | Join (_,x1,y1), Join (_,x2,y2) ->
        shallow_equal x1 x2 && shallow_equal y1 y2
    | Append (_,x1, l1), Append (_,x2, l2) ->
        shallow_equal x1 x2 && L.equal l1 l2
    | AppendStar (_,x1,y1), AppendStar (_,x2,y2) ->
        shallow_equal x1 x2 && shallow_equal y1 y2
    | _ -> false

    let hash x =
      let empty_h = Hashtbl.hash 0 in
      let epsilon_h = Hashtbl.hash 1 in
      match x with
      | Empty -> empty_h
      | Epsilon -> epsilon_h
      | Join (_,x,y) ->
          Hashtbl.hash (0, tag x, tag y)
      | Append (_,r,l) ->
          Hashtbl.hash (1, tag r, L.hash l)
      | AppendStar (_,prefix,body) ->
          Hashtbl.hash (2, tag prefix, tag body)
  end

  module RegexHashSet = Weak.Make(HashedRegex)

  module Datatype = struct
    type t = L.t tagged_regex
    let equal = (==)
    let hash = HashedRegex.tag
  end

  include Datatype

  let hash_set_init_size = 2000
  let hash_set =
    RegexHashSet.create hash_set_init_size

  let tag_counter = ref 2

  let maybe_share new_regex =
    let ret = RegexHashSet.merge hash_set new_regex in
    if ret == new_regex then begin
      incr tag_counter
    end;
    ret

  let join x y =
    if x == y then x
    else match x,y with
    | Empty,_ -> y
    | _,Empty -> x
    | _ -> maybe_share (Join (!tag_counter,x,y))

  let append r l =
    match r with
    | Empty -> empty
    | _ ->
        maybe_share (Append (!tag_counter,r,l))

  let append_star r x =
    match r with
    | Empty -> empty
    | _ -> maybe_share (AppendStar (!tag_counter,r,x))

  let pretty fmt r =
    let rec aux precedence fmt r =
      let open Format in
      let pp format = fprintf fmt format in
      match r with
      | Empty -> pp "<empty>"
      | Epsilon -> pp "<epsilon>"
      | Join (_,x,y) ->
          (if precedence > 0
          then fprintf fmt "(%a@ +@ %a)"
          else fprintf fmt "%a@ +@ %a") (aux 0) x (aux 0) y
      | Append (_,r,l) ->
          (if precedence > 1
          then fprintf fmt "@[<hov 1>(%a@,%a)@]"
          else fprintf fmt "@[<hov 1>%a@,%a@]") (pp_if_not_epsilon 1) r L.pretty l
      | AppendStar (_,r,x) ->
          (if precedence > 1
          then fprintf fmt "@[<hov 1>(%a@,%a*)@]"
          else fprintf fmt "@[<hov 1>%a@,%a*@]") (pp_if_not_epsilon 1) r (aux 2) x
    and pp_if_not_epsilon precedence fmt r = match r with
      | Epsilon -> ()
      | _ -> aux precedence fmt r
    in
    aux 0 fmt r

  module RH = Hashtbl.Make(Datatype)

    
  let pretty_with_sharing fmt (x:t) =
    let tbl = RH.create 17 in
    let rec f x =
      if not @@ RH.mem tbl x
      then begin
          begin match x with
          | Epsilon -> ()
          | Empty -> ()
          | Append(tag,a,b) ->
             let xa = f a in
             ignore @@ Format.fprintf fmt "let %4d = append(%s,%a)@," tag xa L.pretty b
          | Join(tag,a,b) ->
             let xa = f a in
             let xb = f b in
             ignore @@ Format.fprintf fmt "let %4d = join(%s,%s)@," tag xa xb
          | AppendStar(tag,a,b) ->
             let xa = f a in
             Format.fprintf fmt "  @[<v>";
             let xb = f b in
             Format.fprintf fmt "@]";
             ignore @@ Format.fprintf fmt "let %4d = appendstar(%s,%s)@," tag xa xb
          end;
          RH.replace tbl x ()
        end;
      match x with
      | Epsilon -> "epsilon"
      | Empty -> "empty"
      | Append(tag,_,_) | Join(tag,_,_) | AppendStar(tag,_,_) ->
         string_of_int tag
    in Format.fprintf fmt "@[<v>";
       let res = f x in
       Format.fprintf fmt "%s@]" res
  ;;

  let pretty = pretty_with_sharing
    
    
end
