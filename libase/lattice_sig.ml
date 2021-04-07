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

(* Extends datatype: elements now have a lattice structure. *)
module type Join_Semi_Lattice = sig
  include Datatype_sig.S
  val join: t -> t -> t

  (* In a fixpoint iteration of a function F, [previous] is the
     previous element of the fixpoint iteration sequence. The other
     element is the newly computed (tentative) element,
     i.e. F(previous).

     If the new element is included in [previous], [true] is returned,
     together with [new] (the smaller element): the post-fixpoint of F
     has been found (further calls to F can decrease the sequence).

     Else, the returned element should be used as the next element of
     the fixpoint iteration sequence; the operator guarantees its
     convergence. 

     For lattices of finite height, the widening part can just perform
     an over-approximation of the join; however note that it is not
     required that the sequence is monotonic. *)
  val includes_or_widen: previous:t -> t -> (bool * t)

  val includes: t -> t -> bool
  val widen: previous:t -> t -> t
end

(* Optional, standard, operations on lattices. *)

(* Over-approximation of intersection. *)
module type With_Inter = sig
  type t
  val inter: t -> t -> t
end

module type With_Bottom = sig
  type t
  val bottom: t                 (* One of the bottoms. *)
  val is_bottom: t -> bool      (* Can be imprecise; e.g. always returning false is correct. *)
end

module type With_Top = sig
  type t
  val top: t                 (* One of the tops. *)
end


module type Join_Semi_Lattice_With_Bottom = sig
  include Join_Semi_Lattice;;
  include With_Bottom with type t:= t
end

module type Join_Semi_Lattice_With_Inter_Bottom = sig
  include Join_Semi_Lattice;;
  include With_Inter with type t:= t
  include With_Bottom with type t:= t
end


module type With_Equality = sig
  (* Note: this is the "=" transfer function, not equality of abstract
     values. While it is possible to quotient "equal" using this
     equality relation, this is not mandatory, so "eq" values may not
     be "equal" (and possibly, "equal" values may not be "eq"). *)
  type t
  val eq: t -> t -> Quadrivalent_Lattice.t
end

(* For a list of lattice transformers: see e.g. Florian Martin thesis. *)
