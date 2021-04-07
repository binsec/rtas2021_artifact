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

(* Basis are abstraction of a set of values. Compared to abstract
   domains, they abstract a single dimension, rather than a set of
   dimensions.  They have a pure, functional interface. See "Mine, A
   few graph-based relational numerical abstract domains", for a
   definition of integer basis. *)

(* Abstract domains a functionally pure interface, that concretizes
   into a set of values (not into a set of variable -> value
   mappings); e.g. Ival. *)
module Pure_Arity = struct
  module Forward = struct
    type 'r ar0 = 'r
    type ('a,'r) ar1 = 'a -> 'r
    type ('a,'b,'r) ar2 = 'a -> 'b -> 'r
    type ('a,'b,'c,'r) ar3 = 'a -> 'b -> 'c -> 'r
  end

  (* MAYBE: Rather than an option type, explicit: NoChange or ChangeTo (or even a "diff") *)
  module Backward = struct
    type 'r ar0 = 'r -> unit
    type ('a,'r) ar1 = 'a -> 'r -> ('a option)
    type ('a,'b,'r) ar2 = 'a -> 'b -> 'r -> ('a option * 'b option)
    type ('a,'b,'c,'r) ar3 = 'a -> 'b -> 'c -> 'r -> ('a option * 'b option * 'c option)
  end
end

module Forward_Arity = Pure_Arity.Forward;;
module Backward_Arity = Pure_Arity.Backward;;

(****************************************************************)
(* Standard abstraction for booleans. *)
module Quadrivalent = Quadrivalent_Lattice

(****************************************************************)
(* Forward transfer functions *)

module type With_Boolean_Forward = sig
  type boolean = Quadrivalent.t
  module Boolean_Forward:Transfer_functions.Boolean_Forward
    with module Arity := Forward_Arity and type boolean := boolean
  val truth_value: boolean -> Quadrivalent.t
end

module type With_Integer_Forward = sig
  type boolean = Quadrivalent.t
  type integer
  module Integer_Forward:Transfer_functions.Integer_Forward
    with module Arity := Forward_Arity
     and type boolean := boolean
     and type integer := integer
end


module type With_Binary_Forward = sig
  type boolean = Quadrivalent.t
  type binary
  module Binary_Forward:Transfer_functions.Binary_Forward
    with module Arity := Forward_Arity
    and type boolean := boolean
    and type binary := binary
end

module type With_Memory_Forward = sig
  type boolean = Quadrivalent.t
  type binary
  type memory
  module Memory_Forward:Transfer_functions.Memory_Forward
    with module Arity := Forward_Arity
    and type boolean := boolean
    and type binary := binary
    and type memory := memory
end

module type With_Memory_Binary_Boolean_Forward = sig
  include With_Memory_Forward;;
  include With_Binary_Forward with type boolean := boolean
                              and type binary := binary
  include With_Boolean_Forward with type boolean := boolean
end

module type With_Memory_Binary_Integer_Boolean_Forward = sig
  include With_Memory_Forward;;
  include With_Binary_Forward with type boolean := boolean
                               and type binary := binary
  include With_Integer_Forward with type boolean := boolean
  include With_Boolean_Forward with type boolean := boolean
end



module type With_Forward = sig
  include With_Memory_Binary_Integer_Boolean_Forward
end


(****************************************************************)
(* Backward transfer functions *)

module type With_Boolean_Backward = sig
  type boolean = Quadrivalent.t
  module Boolean_Backward:Transfer_functions.Boolean_Backward
    with module Arity := Backward_Arity and type boolean := boolean;;
end

module type With_Integer_Backward = sig
  type boolean = Quadrivalent.t
  type integer
  module Integer_Backward:Transfer_functions.Integer_Backward
    with module Arity := Backward_Arity
     and type boolean := boolean
     and type integer := integer
end


module type With_Binary_Backward = sig
  type boolean = Quadrivalent.t
  type binary
  module Binary_Backward:Transfer_functions.Binary_Backward
    with module Arity := Backward_Arity
    and type boolean := boolean
    and type binary := binary
end

module type With_Memory_Backward = sig
  type boolean = Quadrivalent.t
  type binary
  type memory
  module Memory_Backward:Transfer_functions.Memory_Backward
    with module Arity := Backward_Arity
    and type boolean := boolean
    and type binary := binary
    and type memory := memory
end

module type With_Memory_Binary_Boolean_Backward = sig
  include With_Memory_Backward;;
  include With_Binary_Backward with type boolean := boolean
                              and type binary := binary
  include With_Boolean_Backward with type boolean := boolean
end

(****************************************************************)
(* Both backward and forward. *)

module type With_Boolean_Forward_Backward = sig
  type boolean = Quadrivalent.t
  include With_Boolean_Forward with type boolean := boolean
  include With_Boolean_Backward with type boolean := boolean
end

module type With_Integer_Forward_Backward = sig
  type boolean = Quadrivalent.t
  type integer
  include With_Integer_Forward
    with type boolean := boolean
     and type integer := integer
  include With_Integer_Backward
    with type boolean := boolean
     and type integer := integer
end

module type With_Binary_Forward_Backward = sig
  type boolean = Quadrivalent.t
  type binary
  include With_Binary_Forward
    with type boolean := boolean
    and type binary := binary
  include With_Binary_Backward
    with type boolean := boolean
    and type binary := binary
end

module type With_Memory_Forward_Backward = sig
  type boolean = Quadrivalent.t
  type binary
  type memory
  include With_Memory_Forward
    with type boolean := boolean
    and type binary := binary
    and type memory := memory
  include With_Memory_Backward
    with type boolean := boolean
    and type binary := binary
    and type memory := memory
end

module type With_Binary_Boolean_Forward_Backward = sig
  include With_Binary_Forward_Backward
  include With_Boolean_Forward_Backward with type boolean := boolean
end


module type With_Memory_Binary_Boolean_Forward_Backward = sig
  include With_Memory_Forward_Backward;;
  include With_Binary_Boolean_Forward_Backward with type boolean := boolean
                                               and type binary := binary
end


module type With_Forward_Backward = sig
  include With_Memory_Binary_Boolean_Forward_Backward
end

(****************************************************************)
(* Queries and conversions to "standard" lattices. 


   TODO: Extend with other functions; in particular emptyness testing,
   constantness, should go here. *)

(* Note: Quadrivalent being small and the most precise basis, we
   require its use in queries, which avoids some conversions. *)
module type Boolean_Conversions = sig
  type boolean
  val convert_to_quadrivalent: boolean -> Quadrivalent.t
end

module type Integer_Conversions = sig
  type integer
  val convert_to_ival: integer -> Framac_ival.Ival.t
  val is_singleton_int: integer -> Z.t option
end

module type Binary_Conversions = sig
  type binary
  val binary_to_ival: signed:bool -> size:int -> binary -> Framac_ival.Ival.t

  val binary_is_singleton: size:int -> binary -> Z.t option

  (* True if the binary cannot be concretized into any value.  *)
  val binary_is_empty: size:int -> binary -> bool
  
  (* Fold on all integers contained in either the signed or unsigned
     representation of a binary. *)
  val binary_fold_crop: size:int -> binary -> inf:Z.t -> sup:Z.t -> 'a -> (Z.t -> 'a -> 'a) -> 'a

  val binary_to_known_bits: size:int -> binary -> Single_value_abstraction.known_bits
end

(* Conversions should be implemented by extending this module, so that
   the code is forward-compatible when new conversions are
   required. We use assert false instead of top, so that the loss of
   precision does not get unnoticed.  *)
module Dummy_Conversions = struct
  let convert_to_ival _ = assert false (* Ival.top *)
  let convert_to_cvalue _ = (* Cvalue.V.top *) assert false
  let is_singleton_int _ = assert false(* Quadrivalent.Top *)
  (* TODO: Memory, and queries about the reachability. *)
end

(****************************************************************)
(* Base versions *)



module type Boolean_Lattice = sig
  include Lattice_sig.Join_Semi_Lattice_With_Bottom
  include Lattice_sig.With_Inter with type t := t
  include Lattice_sig.With_Top with type t := t
  val singleton: bool -> t    
end
  
module type Integer_Lattice = sig
  include Lattice_sig.Join_Semi_Lattice_With_Bottom
  include Lattice_sig.With_Inter with type t := t
  include Lattice_sig.With_Top with type t := t
  val singleton: Z.t -> t
end

module type Binary_Lattice = sig
  include Datatype_sig.S
  val bottom: size:int -> t
  val is_bottom: size:int -> t -> bool
  val top: size:int -> t
  val inter: size: int -> t -> t -> t
  val join: size: int -> t -> t -> t    
  val pretty: size:int -> Format.formatter -> t -> unit
  val singleton: size:int -> Z.t -> t
  val widen: size:int -> previous:t -> t -> t
  val includes: size:int -> t -> t -> bool
  val includes_or_widen: size:int -> previous:t -> t -> (bool * t)    
end

module type Memory_Lattice = sig
  include Lattice_sig.Join_Semi_Lattice_With_Bottom
end

module type Boolean_Basis = sig
  val name: string
  include With_Boolean_Forward_Backward
  module Boolean_Lattice:Boolean_Lattice with type t = boolean
  include Boolean_Conversions with type boolean := boolean
end
;;

module type Integer_Basis = sig
  val name: string
  include With_Integer_Forward_Backward
  include With_Boolean_Forward_Backward with type boolean := boolean
  module Boolean_Lattice:Boolean_Lattice with type t = boolean
  module Integer_Lattice:Integer_Lattice with type t = integer
  include Boolean_Conversions with type boolean := boolean
  include Integer_Conversions with type integer := integer
end
;;

module type Binary_Basis = sig
  val name: string
  include With_Binary_Forward_Backward
  include With_Boolean_Forward_Backward with type boolean := boolean
  module Boolean_Lattice:Boolean_Lattice with type t = boolean
  module Binary_Lattice:Binary_Lattice with type t = binary
  include Boolean_Conversions with type boolean := boolean
  include Binary_Conversions with type binary := binary
end
;;


module type Binary_Integer_Basis = sig
  include Binary_Basis
  include Integer_Basis with type boolean := boolean
                         and module Boolean_Lattice := Boolean_Lattice
                         and module Boolean_Forward := Boolean_Forward
                         and module Boolean_Backward := Boolean_Backward                           
end
;;


(* Most complete version, with all datatypes. Used as a parameter for
   functors. *)
module type All_Basis = sig
  val name: string
  include With_Boolean_Forward_Backward 
  include With_Integer_Forward_Backward with type boolean := boolean
  include With_Binary_Forward_Backward with type boolean := boolean
  include With_Memory_Forward_Backward with type binary := binary
                                        and type boolean := boolean
  module Boolean_Lattice:Boolean_Lattice with type t = boolean
  module Integer_Lattice:Integer_Lattice with type t = integer
  module Binary_Lattice:Binary_Lattice with type t = binary
  module Memory_Lattice:Memory_Lattice with type t = memory
  include Boolean_Conversions with type boolean := boolean
  include Binary_Conversions with type binary := binary
  include Integer_Conversions with type integer := integer
                                              
end
