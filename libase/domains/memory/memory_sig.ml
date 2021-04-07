(**************************************************************************)
(*                                                                        *)
(*  This file is part of the Codex plugin of Frama-C.                     *)
(*                                                                        *)
(*  Copyright (C) 2007-2017                                               *)
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

(* Signature for the parts of memory domain.

   The general idea is this. A memory maps adresses to
   values. Memories are built hierarchically: a part of a memory may
   be represented by another memory (e.g. whole memory can be
   parametrized by how memory is represented in each region, arrays
   can be parametrized by how memory is represented inside single
   cells, etc.). Ultimately, memories contain scalar /values/. 

   The fact that the value at the bottom of the hierarchy in the
   memory may contain parts of the adresses used at the top of the
   hierarchy requires a particular instantiation of functors. In
   general, a functor use the adresses of its parameter to build its
   own adresses. The adress of the top-level functor can be used as
   the value for the whole chain of functors, and is passed as an
   argument to build the memories.

   So memory "domains" contains two parts: a part used to build
   adresses and the final value, which is passed as an argument to the
   second part, to build memories. *)



(* Used to distinguish cases for weak updates, strong updates, or
   completely invalid destinations. *)
type 'a precision =
  | Empty
  | Singleton of 'a
  | Imprecise
;;

(* Raised e.g. when storing to an invalid location. *)
exception Memory_Empty;;

(* Value represent what is stored in a memory region. 


   MAYBE: It can represent a region itself? Not for now.

   Note: (at least currently), Value are required to have a fixed size
   (in bits). 
*)
module type Value = sig
  module Scalar:Domain_sig.Base
  type binary                   (* Generally different from Scalar.binary! *)
  val binary_pretty: size:int -> Scalar.Context.t -> Format.formatter -> binary -> unit
  val binary_empty: size:int -> Scalar.Context.t -> binary

  (* Serialize functions also return their de-serializer. *)
  val serialize: size:int -> Scalar.Context.t ->
    binary -> binary -> 'a Scalar.in_acc -> (binary,'a) Scalar.result
  ;;
    
  module Binary_Forward:sig
    val bextract: size:int -> index:int -> oldsize:int ->
      Scalar.Context.t -> binary -> binary
    val bconcat: size1:int -> size2:int -> Scalar.Context.t -> binary -> binary -> binary
    val bchoose: size:int -> Transfer_functions.Choice.t -> Scalar.Context.t -> binary -> binary
  end

end

(* This type can be used as a Value in memory, but can also be
   operated using the classical transfer functions. This is used to
   build values that contains the adresses to a memory (memory
   functors should allow building an Operable_Value from an
   Operable_Value that can maps the adresses of the memory lower in
   the hierarchy). *)
module type Operable_Value = sig

  type binary

  module Binary:Datatype_sig.S with type t = binary
  module Scalar:Domain_sig.Base
  include Value with type binary := binary
                 and module Scalar := Scalar    

  module Query:sig
    module Binary_Lattice:Domain_sig.Binary_Lattice
    val binary: size:int -> Scalar.Context.t -> binary -> Binary_Lattice.t
    include Basis_sig.Binary_Conversions with type binary := Binary_Lattice.t
  end

  
end


(* Operable value representing values and pointers to the whole memory. *)
module type Operable_Value_Whole = sig

  module Scalar:Domain_sig.Base

  type binary
  module Binary_Forward:Transfer_functions.Binary_Forward
    with module Arity := Domain_sig.Context_Arity_Forward(Scalar.Context)
     and type boolean := Scalar.boolean
     and type binary := binary  

  include Operable_Value
    with module Scalar := Scalar
     and type binary := binary
     and module Binary_Forward := Binary_Forward

  (* Applying guards on a pointer. *)
  val assume: size:int -> Scalar.Context.t -> Scalar.boolean -> binary -> binary
  val binary_unknown: size:int -> level:int -> Scalar.Context.t -> binary
end

(* Representation of pointers inside some memory regions. Maybe
   address or pointer would be a better name here. *)
module type Operable_Value_Region = sig

  include Operable_Value

  (* MAYBE: send and receive integers instead of binaries. *)

  val bchoose: size:int -> Transfer_functions.Choice.t -> Scalar.Context.t -> binary -> binary
  
  (* address := address + k * expr *)
  val bindex: size:int -> int -> Scalar.Context.t -> binary -> Scalar.binary -> binary

  (* pointer substraction. *)
  val bisub: size:int -> Scalar.Context.t -> binary -> binary -> Scalar.binary

  (* address := address + k *)
  val bshift: size:int -> offset:int -> max:int option -> Scalar.Context.t -> binary -> binary

  (* Beginning of a region (offset 0) of size max (in bytes, if known and/or useful). *)
  val zero_offset: size:int -> max:int option -> Scalar.Context.t -> binary

  (* Pointer comparison. *)
  val ble: size:int -> Scalar.Context.t -> binary -> binary -> Scalar.boolean
  val beq: size:int -> Scalar.Context.t -> binary -> binary -> Scalar.boolean

  (* Applying guards on a pointer. *)
  val assume: size:int -> Scalar.Context.t -> Scalar.boolean -> binary -> binary

  val binary_unknown : size:int -> level:int -> Scalar.Context.t -> binary

  (* Check that all array indices operation are within bound. *)
  val within_bounds: size:int -> Scalar.Context.t -> binary -> Scalar.boolean
  
end

(* Memory map adresses to values. These types may differ (for
   instance, an array functor can takes integer as adresses, but can
   contain any value). *)
module type Memory = sig
  
  type address 
  module Scalar:Domain_sig.Base
  module Value:Value
  
  type memory
  val pretty: Scalar.Context.t -> Format.formatter -> memory -> unit

  (* MAYBE: assume too? This could help when serializing. Or: always 
     put the assume on the load, on the top-level domain. *)
  val load: size:int -> Scalar.Context.t -> memory -> address -> Value.binary
  val store: size:int -> Scalar.Context.t -> memory -> address -> Value.binary -> memory

  val serialize: Scalar.Context.t -> memory -> memory -> 'a Scalar.in_acc ->
    (memory,'a) Scalar.result
end



(* The signatures for regions and whole memory are different, as we do
   not perform the same actions on them. *)

module type Memory_Region_Functions = sig
  module Scalar:Domain_sig.Base
  type memory
  
  (* Create an initial region of size int (in bytes). *)
  val initial: Scalar.Context.t -> int -> memory

  (* Temporary; get size (in bytes). *)
  val get_size: memory -> int


end

module type Memory_Region = sig
  include Memory

  include Memory_Region_Functions
    with module Scalar := Scalar
     and type memory := memory

end

module type Whole_Memory_Functions = sig
  module Scalar:Domain_sig.Base
  type memory
  type address
  
  (* Allocates a memory region in m. *)
  val malloc: id:Transfer_functions.Malloc_id.t -> malloc_size:int ->
    Scalar.Context.t -> memory -> address * memory

  (* Free a memory region. *)
  val free: Scalar.Context.t -> memory -> address -> memory

  val unknown: level:int -> Scalar.Context.t -> memory

  val memory_empty: Scalar.Context.t -> memory

end

module type Whole_Memory_Queries = sig
  module Scalar : Domain_sig.Base
  type memory
  type address

  (** [should_focus ~size ctx mem addr] asks the domain whether it is useful to
     "focus" (or "unfold", i.e. try to represent precisely the memory region
     pointed to by [addr], as long as aliasing info ensures that it is sound) a
     loaded value. If the answer is yes, then the returned value should contain
     three things about the region to focus: its base, its size and the offset
     of [addr] in it. *)
  val should_focus : size:int -> Scalar.Context.t -> memory -> address ->
    (address * int * int) option

  (** [may_alias ~size ctx addr1 addr2] should return whether [addr1] and
     [addr2] can point to the same object. What is an "object" depends on the
     memory domain. It can be individual memory cells, or more complex memory
     structures. This function is used by focusing abstractions to discard a
     focused region when writing in a possibly aliased address. *)
  val may_alias : size:int -> Scalar.Context.t -> address -> address -> bool
end


module type Whole_Memory = sig
  include Memory

  include Whole_Memory_Functions
    with module Scalar := Scalar
     and type memory := memory
     and type address := address

  include Whole_Memory_Queries
    with module Scalar := Scalar
     and type memory := memory
     and type address := address
end
