 (**************************************************************************)
(*                                                                        *)
(*  This file is part of Codex.                                           *)
(*                                                                        *)
(*  Copyright (C) 2013-2021                                               *)
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
(* Standard abstractions, used to communicate between lattices. *)

(* Abstraction for booleans. *)
module Quadrivalent = struct
  type t = Bottom | True | False | Top
end

(**************** Abstractions for bitvectors. ****************)

(* Min, max.  Note that on bitvectors there is a maximum, so there is
   no need for infinity. *)
type interval = Z.t * Z.t;;
type signed_interval = interval;;
type unsigned_interval = interval;;

(* First bitvector = 0 where the bits must be 0, 1 otherwise.
   Second bitvector = 1 where the bits must be 1, 0 otherwise.  
   Both Z must be positive. *)
type known_bits = Z.t * Z.t

(* Represents a set q * Z + r, where q is the first element, and r the second. 
   - If q is negative, this represents bottom.
   - If q is 0, this represents a singleton.
   - If q is >0, then 0 <= r < q. *)
(* NB: we could have another abstraction representing truncated division. *)
type congruence = Z.t * Z.t


(* Bitvectors are finite, thus we can explicitely enumerate bitvectors. *)
(* MAYBE: use a smarter representation, e.g. based on Binary Decision
   Diagrams (or Ternary Decision Diagrams) *)
module BVSet = Set.Make(Z)
                          

module Interval = struct
  type t = interval
  let join ~size:_ (min1,max1) (min2,max2) = (Z.min min1 min2, Z.max max1 max2);;
  let inter ~size:_ (min1,max1) (min2,max2) = (Z.max min1 min2, Z.min max1 max2);;  
    
  (* Any element where max is smaller than min is bottom. *)
  let bottom ~size:_ = Z.one, Z.zero
                              
  (* top is different in signed and unsigned intervals.  *)
  
end

module Known_bits = struct
  type t = known_bits
  let inter0 = Z.(land)
  let inter1 = Z.(lor)
  let inter ~size:_ (x0,x1) (y0,y1) = (inter0 x0 y0, inter1 x1 y1);;

  let bottom ~size = (Z.zero, Z.extract Z.minus_one 0 size);;  

  let top ~size = (Z.extract Z.minus_one 0 size, Z.zero)
end

(* Intervals in different ranges.  *)
module Signed_interval = Interval
module Unsigned_interval = Interval  

module Congruence = struct
  type t = congruence
  let bottom ~size:_ = (Z.minus_one, Z.minus_one)
  let top ~size:_ = Z.one, Z.zero   (* 1 * Z + 0 = R *)
  let singleton ~size x = Z.zero, Z.extract x 0 size
end

