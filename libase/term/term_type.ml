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

(* This module defines the types used in the Term package.  *)

(* It is important for variables of the program to have "unique"
   names, even for bound variables; because the free variables of the
   terms of the fact store use these names to refer to the variables
   of the program (i.e. the fact "x < 3" refers to a specific "x" of
   the program). In particular, we should not use De Bruijn indexes. *)
module Var:sig
  include Datatype_sig.S
  val create: unit -> t
end = struct
  include Datatype_sig.Int
  let pretty fmt x = Format.fprintf fmt "x%d" x
  let count = ref 0
  let create () = incr count; !count
end

open Term_constructors;;

module rec Term_datatype:(sig
  include Datatype_sig.S with type t = (Var.t, Term_set.t, Z.t Term_map.t, W.t) _term;;
  val ref_fprint_term: (Format.formatter -> t -> unit) ref
end)
= 
  struct
    type t = (Var.t, Term_set.t, Z.t Term_map.t, W.t) _term
    let ref_fprint_term = ref (fun _fmt (_term:t) -> assert false);;

    let name = "Codex.term"

    (* This Obj.magic is necessary to avoid a "undefined recursive
       module" exception; W being the "safe" module. *)
    let reprs:t list = [{id=237;kind=True;parents=(Obj.magic 1)(* W.create 17 *) }]

    let compare a b = Pervasives.compare a.id b.id (* a.id - b.id? *)
    let pretty fmt term = !ref_fprint_term fmt term
    let equal a b = a.id == b.id
    let hash t = t.id
  end
and W:(Weak.S with type data = Term_datatype.t) = Weak.Make(Term_datatype)
and Term_map:(Extstdlib.Map.S with type key = Term_datatype.t) = Extstdlib.Map.Make(Term_datatype)
and Term_set:(Extstdlib.Set.S with type elt = Term_datatype.t) = Extstdlib.Set.Make(Term_datatype)
               

(* Note: we use a weak hash table to store the parents, so that we
   can easily remove parents, for instance when a term gets replaced
   by a simpler one. *)
module Parents = struct
  type t = W.t
  let create () = W.create 17
  let add set term = W.add set term
  let iter = W.iter
  let fold = W.fold
  let to_list p = W.fold (fun a b -> a::b) p []

  (* Full iteration: can be costly. *)
  let length t =
    Codex_log.warning "Parents.length is slow; do not call it";
    W.count t
      
  let length_more_than t x =
    try
      let _ = W.fold (fun _ x ->
        let v = x -1 in if v == 0 then raise Exit else v) t x in
      false
    with Exit -> true
end

let ref_fprint_term = Term_datatype.ref_fprint_term;;

type term =                                       (Var.t, Term_set.t,
                                        Z.t Term_map.t, Parents.t)
                                         Term_constructors._term

(* type term = Term_datatype.t *)

type kind = (Var.t, term,
             Term_set.t,
             Z.t Term_map.t) Term_constructors._kind

(* TODO (MAYBE):
 - Add type information to terms
   - Strictly bottom-up type inference:
     - Type information explicitely given to variables
     - Term infer/check its type from type of subterms
   - Integrate with the smart constructors
   - Note: type information does not allow dispatch between domains
     (e.g. "store" and "load" return different types, but both handled
      by memory domain)
 *)

module Term_hashtbl = Hashtbl.Make(Term_datatype)
