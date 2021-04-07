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

type counter_example = unit

type sat =
  | Sat of counter_example
  | Unsat
  | Unknown

module Quadrivalent = Basis_sig.Quadrivalent

(* Note: we use a module for Context, instead of a type, so that it
   can be used as a functor argument and replace the Arity. *)
module type Context = sig
  type t
  (* The level, i.e. depth of imbricated mu contexts. *)
  (* Note: currently, the context has no other use than this, 
     and could maybe be suppressed (replaced by the notion of scope 
     that is linked to the term)  *)
  val level:t -> int
end

module Context_Arity_Forward(Context:Context) = struct
  type 'r ar0 = Context.t -> 'r
  type ('a,'r) ar1 = Context.t -> 'a -> 'r
  type ('a,'b,'r) ar2 = Context.t -> 'a -> 'b -> 'r
  type ('a,'b,'c,'r) ar3 = Context.t -> 'a -> 'b -> 'c -> 'r
  type ('a,'r) variadic = Context.t -> 'a list -> 'r
end

(* General design notes:

   - Functors should state its minimal needs and provides the maximum
   features. But optional features of a functor, depending on an
   optional feature of an argument, should be done using "additional
   features functor". This sometimes make their use impractical, as is
   the multiplication of possible configurations. 

   For this reason, we tend to have the "base case" require more
   features than strictly necessary, when these extra features can be
   easily implemented using an approximate version (like the is_empty
   functions).

   TODO: This is the case for backward propagation functions, so they
   could always be required.

   - The types are always lifted, so that it is easy to add type
   equality/substitution in module constraints. Modules are only used
   for things that are not supposed to be changed, because it is
   difficult to "insert" additional values inside a submodule. That is
   why we do not use recursive submodules. 

   - There is a hierarchy of types: boolean is standalone, binary
   depends on boolean, and memory depends on both. 

   - When including modules with additional components (e.g. size_int
   for Ref_Addr), always put the component with largest fields at the
   beginning, else destructive substitution of modules does not work. 

   - The main purpose of this file is to provide a "standard" way to
   name the operations on domains. *)
(* Note: for simplicity, we now just require everybody to provide "Base";
   we have helper "Assert_False" modules to fill the holes when needed. *)


(****************************************************************)
(* Base module types describing operations on one or several types of terms. *)

(* Notes on the base operations:

   - Pretty is required everywhere, and used for debugging.

   - Equality refers to equality of the concretization. It can be
   approximate, i.e. it is ok to return false when we cannot detect
   that elements are equal; however when used as keys of
   datastructures, equality should probably at least return true for
   elements that are (==).

   - TODO: Do compare and hash have to respect equality? Map and Set
   do not need "equal", but Hashtbl does. So it seems that at least
   hash should respect equality, i.e. equal elements should have the
   same hash; which is not obvious when structurally different
   elements are detected as equal (e.g. different representations of
   empty). Or maybe it does not need, but in this case it is
   undefined whether different abstract values with same
   concretization represent different binding in the table (if by
   chance the hash is the same, they will share a binding; else they
   may have different bindings). 

   - compare and hash do not need to be implemented if the
   datastructures are not used.

*)

module type Boolean = sig
  type boolean
  module Context:Context
  module Boolean:Datatype_sig.S with type t = boolean

  
  (* TODO: is_empty should go into queries. *)
  (* Can return true if provably empty; false is always safe.  *)
  val boolean_is_empty: Context.t -> boolean -> bool
  val boolean_pretty: Context.t -> Format.formatter -> boolean -> unit
end

module type Integer = sig
  type boolean
  type integer
  module Context:Context
  module Integer:Datatype_sig.S with type t = integer

  (* TODO: An "integer_value", that returns a range (and congruence
     information) of the value. And perhaps another representation
     that uses multi-intervals. *)

  (* Can return true if provably empty; false is always safe.  *)
  val integer_is_empty: Context.t -> integer -> bool
  val integer_pretty: Context.t -> Format.formatter -> integer -> unit
end


module type Binary = sig
  type boolean
  type integer
  type binary
  module Context:Context
  module Binary:Datatype_sig.S with type t = binary

  (* Can return true if provably empty; false is always safe.  *)
  val binary_is_empty: size:int -> Context.t -> binary -> bool

  val binary_pretty: size:int -> Context.t -> Format.formatter -> binary -> unit

end

module type Binary_Integer_Boolean = sig
  include Boolean
  include Integer with type boolean := boolean
                   and module Context := Context
  include Binary with type boolean := boolean
                  and module Context := Context
                  and type integer := integer
end

module type Memory = sig
  type boolean
  type binary
  type memory
  module Context:Context
  (* Can return true if provably empty; false is always safe.  *)
  val memory_is_empty: Context.t -> memory -> bool

  val memory_pretty: Context.t -> Format.formatter -> memory -> unit
end

module type Memory_Binary_Integer_Boolean = sig
  include Binary_Integer_Boolean
  include Memory with type boolean := boolean
                 and type binary := binary
                 and module Context := Context
end

(****************************************************************)
(* Forward transfer functions *)

module type With_Boolean_Forward = sig
  type boolean
  module Context:Context
  module Boolean_Forward:Transfer_functions.Boolean_Forward
    with module Arity := Context_Arity_Forward(Context) and type boolean := boolean
end

module type With_Integer_Forward = sig
  type boolean
  type integer
  module Context:Context
  module Integer_Forward:Transfer_functions.Integer_Forward
    with module Arity := Context_Arity_Forward(Context)
    and type boolean := boolean
    and type integer := integer
end


module type With_Binary_Forward = sig
  type boolean
  type binary
  module Context:Context
  module Binary_Forward:Transfer_functions.Binary_Forward
    with module Arity := Context_Arity_Forward(Context)
    and type boolean := boolean
    and type binary := binary
end

module type With_Memory_Forward = sig
  type boolean
  type binary
  type memory
  module Context:Context
  module Memory_Forward:Transfer_functions.Memory_Forward
    with module Arity := Context_Arity_Forward(Context)
    and type boolean := boolean
    and type binary := binary
    and type memory := memory
end

module type With_Memory_Binary_Integer_Boolean_Forward = sig
  include With_Memory_Forward;;
  include With_Binary_Forward with type boolean := boolean
                              and type binary := binary
                              and module Context := Context
  include With_Integer_Forward with type boolean := boolean
                                and module Context := Context
  include With_Boolean_Forward with type boolean := boolean
                               and module Context := Context
end

module type With_Forward = sig
  include With_Memory_Binary_Integer_Boolean_Forward
end
  
(****************************************************************)

(* Empty denotes that the concretization has no value (or it is the
   concrete value representing the absence of value). Note that this
   does not necessarily imply that some error occured; for instance
   the offset of an uninitialized pointer can be represented with
   empty. Emptyness testing is a simple way of communicating between
   domains. *)
module type With_Empty = sig
  type binary
  type boolean
  type integer
  module Context:Context
  val binary_empty: size:int -> Context.t -> binary                               
  val boolean_empty: Context.t -> boolean
  val integer_empty: Context.t -> integer
end

(* A new value which is unknown, that is fresh at every level level. *)
module type With_Unknown = sig
  type binary
  type boolean
  type integer
  module Context:Context
  val binary_unknown:  size:int -> level:int -> Context.t -> binary
  val integer_unknown: level:int -> Context.t -> integer
  val boolean_unknown: level:int -> Context.t -> boolean
end


(****************************************************************)
(* Queries. *)

(* Queries allow to ask the domain an overapproximation of the set of
   concrete objects to which a dimension concretizes. This set of
   object must be finitely represented, but the choice of this
   representation is left to the domain. It is required that these
   objects can be converted to some standard representations.

   In addition, we require this set of object to be represented by a
   lattice, so that it is possible to test inclusion and perform
   approximation of union on these set of objects.
*)

(* Note: since Quadrivalent exactly represents the powerset of
   {true,false}, there is no point in using another type. *)
module type Boolean_Lattice = Basis_sig.Boolean_Lattice
module type Integer_Lattice = Basis_sig.Integer_Lattice
module type Binary_Lattice = Basis_sig.Binary_Lattice
module type Memory_Lattice = Basis_sig.Memory_Lattice                                                               

module type With_Queries = sig
  module Context:Context
  type boolean
  type integer
  type binary
  type memory

  module Query:sig
    module Boolean_Lattice:Boolean_Lattice
    module Binary_Lattice:Binary_Lattice
    module Integer_Lattice:Integer_Lattice
    include Basis_sig.Boolean_Conversions with type boolean := Boolean_Lattice.t
    include Basis_sig.Binary_Conversions with type binary := Binary_Lattice.t
    include Basis_sig.Integer_Conversions with type integer := Integer_Lattice.t
  (* TODO: This supersedes "truth_value" *)
  val boolean: Context.t -> boolean -> Boolean_Lattice.t
  val integer: Context.t -> integer -> Integer_Lattice.t
  val binary: size:int -> Context.t ->  binary -> Binary_Lattice.t

  (* Reachable means that the set of memory states is not empty. 
     - If we return {True,False}, then the memory may be reachable;
     - If we return {True}, then the memory is reachable;
     - If we return {False} or {}, the memory is not reachable. *)
  val reachable: Context.t -> memory -> Quadrivalent.t
  end

  (** [should_focus ~size ctx mem addr] asks the domain whether it is useful to
     "focus" (or "unfold", i.e. try to represent precisely the memory region
     pointed to by [addr], as long as aliasing info ensures that it is sound) a
     loaded value. [size] should be the size of [addr]. If the answer is yes,
     then the returned value should contain three things about the region to
     focus: its base, its size (in bits) and the offset of [addr] in it (in
     bits). *)
  val should_focus : size:int -> Context.t -> memory -> binary ->
    (binary * int * int) option

  (** [may_alias ~size ctx addr1 addr2] should return whether [addr1] and
     [addr2] can point to the same object. What is an "object" depends on the
     memory domain. It can be individual memory cells, or more complex memory
     structures. This function is used by focusing abstractions to discard a
     focused region when writing in a possibly aliased address. [size] is only
     the size of bath [addr1] and [addr2]. *)
  val may_alias : size:int -> Context.t -> binary -> binary -> bool
end

(****************************************************************)  
(* Other extensions. *)

module type With_Partitionning = sig
  type 'a decision
  type boolean
  (* The function goes from the strategy to the partitionning map;
     this requires the polymorphic argument. *)
  val boolean_split: ('a -> 'a -> 'a decision) -> boolean -> boolean
end

(****************************************************************)
(* Context *)

module type With_Context = sig
  module Context:Context
  type root_context

  (* Opens a new context, corresponding to the initial scope. *)
  val root_context: unit -> root_context
  val root_context_upcast: root_context -> Context.t
end

(****************************************************************)
(* Guards *)


module type With_Assume = sig
  type boolean
  type integer
  type binary
  type memory
  module Context:Context

  val assume_boolean: Context.t -> boolean -> boolean -> boolean  
  val assume_integer: Context.t -> boolean -> integer -> integer
  val assume_binary: size:int -> Context.t -> boolean -> binary -> binary
  val assume_memory: Context.t -> boolean -> memory -> memory
end


(****************************************************************)
(* Fixpoint iteration, and base for all abstract domains. *)

module type With_Nondet = sig
  module Context:Context

  type 'a in_tuple
  type 'a out_tuple

  (* "Joining" variables together. *)
  val typed_nondet2: Context.t -> 'a in_tuple -> 'a out_tuple

end

(* Note: all domains could have thee same interface; but some would
   have assert false and unit for the types they do not handle. We
   could even list the types that are handled by a domain, and check
   that everything is correct on functor instantiation. E.g. a functor
   translating memory to binary know how to handle binary and memory,
   and requires binary; it is able to pass the other types through.

   Other things could be checked, e.g. whether the domain is
   relational or not (possible optimizations), or if bottom is
   handled/necessary (evaluating domains do not require elements below
   to have a bottom.) *)

module type With_Fixpoint_Computation = sig
  module Context:Context
  type mu_context

  type 'a in_tuple
  type 'a out_tuple
  
  (* A mu_context is just a special case of context. *)
  val mu_context_upcast: mu_context -> Context.t

  (* TODO: Should rely on a previous context; or context should be
     something global on which we can open and close scopes
     imperatively.

     (MAYBE A context can be seen as a new branch in a derivation
     (proof) tree; leaves context have more variables introduced, (and
     possibly will know more fact), than the root.)

     Opening a new context can also be seen as opening a new scope:
     new variables can be introduced, but variables of the parent
     scope can still be seen.

     Contexts are used as follow. First, a new context is opened. Each
     context contains a tuple of the variables that have been
     introduced. *)
  val mu_context_open: Context.t -> mu_context

  (* Fixpoint step is a combination of inclusion checking +
     widening. It takes two arguments: a tuple of values which is the previous
     argument of the mu (the previous beginning of the loop), and a
     corresponding tuple which is the result of the evaluation of the body (the
     end of the loop).  Internally, it stores the arguments of the mu.

     It returns a boolean which says if the fixpoint is reached, and a
     function. If the fixpoint is reached, we can "close" the mu, and
     the function returns a tuple corresponding to the mu. We can
     always "restart" the mu, in which case the function returns a new
     arg. *)
  (* MAYBE: return other informations with the tuple, for instance if
     we detect that the function is finitely unrollable. *)
  val typed_fixpoint_step: mu_context -> (bool * 'a in_tuple) ->
    bool * (close:bool -> 'a out_tuple)
end

module Fresh_id:sig
  type t = private int
  val fresh: string -> t (* Arg: the name of the module; may be useful later. *)
end = struct
  type t = int
  let count = ref 0
  let fresh name =
    let v = incr count; !count in
    Codex_log.debug "Registering domain %s with id %d" name v;
    v
end


(* Identifying domains. *)
module type With_Id = sig
  val unique_id: Fresh_id.t
  val name: string
end

module type With_Serialization = sig
  type boolean
  type integer
  type binary
  type memory
  module Context:Context

  type 'a in_tuple
  (* Note: the is a (&&) of all the inclusion checks performed so far.   *)
  type 'a in_acc = bool * 'a in_tuple
  type 'a out_tuple
  type empty_tuple
  val empty_tuple: empty_tuple in_tuple
  
  (* We use a GADT because 'some is existentially quantified. The boolean
   * expresses whether the **second** operand of the serialization was included
   * in the **first** one. *)
  type ('a,'b) result = Result: bool * 'some in_tuple * ('some out_tuple -> 'a * 'b out_tuple) -> ('a,'b) result

  (* Serialization functions for base types. *)
  val serialize_integer: Context.t -> integer -> integer -> 'a in_acc -> (integer,'a) result
  val serialize_boolean: Context.t -> boolean -> boolean -> 'a in_acc -> (boolean,'a) result
  val serialize_binary: size:int -> Context.t -> binary -> binary -> 'a in_acc -> (binary,'a) result
  val serialize_memory: Context.t -> memory -> memory -> 'a in_acc -> (memory,'a) result                  
end

(* Note: We could have store a "current_context" context inside the
   domain, which would avoids the need to pass context,mu_context
   etc. everytime. *)
(* Note: we could now organize this differently, using different types. *)
module type Base = sig
  include With_Id

  type integer
  type binary
  type memory
  type boolean

  include With_Context
  include With_Queries with module Context := Context
                             and type integer := integer
                             and type binary := binary
                             and type memory := memory
                             and type boolean := boolean

  (* Used to select which variables to join pairwise. *)
  include With_Serialization with module Context := Context
                             and type integer := integer
                             and type binary := binary
                             and type memory := memory
                             and type boolean := boolean

  (* Guards *)
  include With_Assume with module Context := Context
                       and type integer := integer
                       and type binary := binary
                       and type memory := memory
                       and type boolean := boolean

  (* Joining variables together. *)
  include With_Nondet with module Context := Context
                       and type 'a in_tuple := 'a in_tuple
                       and type 'a out_tuple := 'a out_tuple                                         

  
  (* Fixpoint computation. *)
  include With_Fixpoint_Computation with module Context := Context
                                     and type 'a in_tuple := 'a in_tuple
                                     and type 'a out_tuple := 'a out_tuple                                         
  (* Builtin functions. *)
  include Builtin.With_Builtin with type binary := binary
                                and type integer := integer
                                and type boolean := boolean
                                and type memory := memory
                                and module Context := Context

  (* Transfer functions. *)
  include Memory_Binary_Integer_Boolean with type boolean := boolean
                                         and type integer := integer
                                         and type binary := binary
                                         and type memory := memory
                                         and module Context := Context

  include With_Empty with type boolean := boolean
                      and type integer := integer
                      and type binary := binary
                      and module Context := Context

  include With_Unknown with type boolean := boolean
                        and type integer := integer
                        and type binary := binary
                        and module Context := Context
  

  include With_Forward with type boolean := boolean
                        and type binary := binary
                        and type integer := integer
                        and type memory := memory
                        and module Context := Context

  (* Set operations. Note that we do not distinguish binary from binary sets. *)
  val union: Transfer_functions.Condition.t -> Context.t -> 'a in_tuple -> 'a out_tuple

  (* Check if an assertion is satisfiable (i.e. a trace exists that makes it true). *)
  val satisfiable: Context.t -> boolean -> sat

  (* Check if the memory is reachable. sat means reachable. *)
  val reachable: Context.t -> memory -> sat
  
end

(****************************************************************)
(* Context conversion procedures: pass through the values by just
   changing the context. *)
module type Convert_Contexts = sig
  module From:Context
  module To:Context
  val convert: From.t -> To.t
end

module Make_Convert(C:Convert_Contexts) = struct
  (* Note: context conversion goes in the opposite direction than
     transfer function conversion.*)
  module From_Arity = Context_Arity_Forward(C.To)
  module To_Arity = Context_Arity_Forward(C.From)
  let ar0 f ctx = f (C.convert ctx)
  let ar1 = ar0
  let ar2 = ar0
  let ar3 = ar0
  let variadic = ar0
end

module Convert_Boolean_Forward
  (C:Convert_Contexts)
  (D:With_Boolean_Forward with module Context = C.To) =
struct
  module C = Make_Convert(C)
  module F = struct include D;; include D.Boolean_Forward end
  include Transfer_functions_conversions.Convert_Boolean_Forward(C)(F)
end

module Convert_Integer_Forward
  (C:Convert_Contexts)
  (D:With_Integer_Forward with module Context = C.To) =
struct
  module C = Make_Convert(C)
  module F = struct include D;; include D.Integer_Forward end
  include Transfer_functions_conversions.Convert_Integer_Forward(C)(F)
end


module Convert_Binary_Forward
  (C:Convert_Contexts)
  (D:With_Binary_Forward with module Context = C.To) =
struct
  module C = Make_Convert(C)
  module F = struct include D;; include D.Binary_Forward end
  include Transfer_functions_conversions.Convert_Binary_Forward(C)(F)
end

module Convert_Memory_Forward
  (C:Convert_Contexts)
  (D:With_Memory_Forward with module Context = C.To) =
struct
  module C = Make_Convert(C)
  module F = struct include D;; include D.Memory_Forward end
  include Transfer_functions_conversions.Convert_Memory_Forward(C)(F)
end

