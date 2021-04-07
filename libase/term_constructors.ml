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

(* Constructors for the term. They are written here once for all, in
   this ml file that does not have a mli, do avoid the need to write them
   several times. *)

type level = int                (* De Bruijn level; i.e. we count indices from the outside *)
type time = level * int   (* A unique identifier for terms that need it.
                                    We suppose that to each binder is associated a unique time;
                                    the second int uniquely identifies all the terms that use 
                                    that unique time. *)

type size = Term_types.size
type term_type = Term_types.t


(* The term language. (See description below) *)



type ('var,'t,'tset, 'intmap) _kind = 

(* Generic operators. *)
| Var of term_type Immutable_dynamic_array.t ref * 'var * level
| Assume of 't * 't		    (* bool * 'a -> 'a *)
| If_not_bottom of 'tset * 't       (* 'a * 'b -> 'b *)  
| Nondet of time * term_type Immutable_dynamic_array.t * 'tset (* 'a set -> 'a *)
| Mu of time * 't * 't * 't              (* (tuple -> tuple) * tuple -> tuple *)

(* Tuples. Note: these are different from arrays; the selector is
   always a constant int, and one can freely mix types. *)
| Tuple_get of 't * int
| Tuple_create of term_type Immutable_dynamic_array.t Lazy.t * 't Immutable_dynamic_array.t

(* Boolean constants and operations (excluding predicates) *)
| True                              (* bool *)
| False                             (* bool *)
(* MAYBE: Replace by an if-then-else/BDD-like construct? *)
| And of 't * 't                    (* bool * bool -> bool *)
| Or of 't * 't                     (* bool * bool -> bool *)
| Not of 't                         (* bool -> bool *)

(* Operation on binary objects (i.e. representations of objects). *)
(* TODO: 
   - Use an integer term instead of an integer.t *)
| Bunknown of size * time           (* binary<size> *) (* like havoc in boogie. *)
| Buninit of size                   (* binary<size> *)    
| Biconst of size * Z.t             (* binary<size> *)
| Biadd of size * bool * bool * 't * 't           (* binary<size> * binary<size> -> binary<size> *)
| Bisub of size * bool * bool * 't * 't           (* binary<size> * binary<size> -> binary<size> *)
| Bimul of size * bool * bool * 't * 't           (* binary<size> * binary<size> -> binary<size> *)        
| Biudiv of size * 't * 't          (* binary<size> * binary<size> ->binary<size> *)
| Bisdiv of size * 't * 't          (* binary<size> * binary<size> ->binary<size> *)
| Biumod of size * 't * 't          (* binary<size> * binary<size> ->binary<size> *)
| Bismod of size * 't * 't          (* binary<size> * binary<size> ->binary<size> *)
(* MAYBE: the second argument should be an integer, rather than a binary. *)
(* Arithmetic (signed) and logical (unsigned) shift left and right. *)
| Bshl of size * bool * bool * 't * 't            (* binary<size> * binary<size> -> binary<size>*)
| Bashr of size * 't * 't           (* binary<size> * binary<size> -> binary<size> *)
| Blshr of size * 't * 't           (* binary<size> * binary<size> -> binary<size> *)
| Band of size * 't * 't            (* binary<size> list -> binary<size> *)
| Bor of size * 't  * 't             (* binary<size> list -> binary<size> *)
| Bxor of size * 't * 't            (* binary<size> list -> binary<size> *)
(* TODO: Binary floating-point operations. *)
    
(* Binary predicates: =, integer signed and unsigned <= and <  *)
| Beq of size * 't * 't             (* binary<size> * binary<size> -> bool *)
| Bisle of size * 't * 't           (* binary<size> * binary<size> -> bool *)
| Biule of size * 't * 't           (* binary<size> * binary<size> -> bool *)
(* TODO: Bslt, Bsle, Bult, Bule. *)
| Bconcat of size * 't * 't         (* binary<size_i> * binary<size_j> -> binary<size_i+size_j>; first argument is most significant. *)
| Bextract of size * int * 't        (* newsize:int * index:int * binary<size> -> binary<newsize> *)
| Bsext of size * 't                 (* binary<size> -> binary<i>, with i > size *)
| Buext of size * 't                 (* binary<size> -> binary<i>, with i > size *)
| Bofbool of size * 't               (* boolean -> binary<size> *)         

| Valid of size * Transfer_functions_sig.access_type * 't                 (* binary<size> -> bool *)
| Valid_ptr_arith of size * 't * 't
| Bshift of size * int * int option * 't    (* binary<size> -> bool + limitation of pointer arithmetics*)
| Bindex of size * 't * int * 't     (* binary<size> -> bool *)

(* TODO: Arguments to nondet. *)

(* Integer operations. Currently not used. *)
(* MAYBE: Should possibly be "numeric operations", on reals + integers. *)
| Iconst of Z.t
| Iadd of 't * 't
| Isub of 't * 't
| Imul of 't * 't                   (* integer list -> integer *)
| Idiv of 't * 't                   (* integer * integer -> integer *)
| Imod of 't * 't                   (* integer * integer -> integer *)
| Itimes of Z.t * 't          (* const integer * integer -> integer *)
| Ieq of 't * 't
| Ile of 't * 't
| Ishl of 't * 't                   (* integer * integer -> integer *)
| Ishr of 't * 't                   (* integer * integer -> integer *)
| Iand of 't * 't                   (* integer * integer -> integer *)
| Ior of 't * 't                    (* integer * integer -> integer *)
| Ixor of 't * 't                   (* integer * integer -> integer *)
| Iunknown of time                  (* integer *)
    
(* Maybe: Ishiftl/Ishiftr, or Ishift. The logical shift is
   meaningless, as integers have an infinite number of leading zero
   or ones. *)
    
(* TODO: Operations to obtain a base and an offset from a binary (that
   contains a pointer); to shift an offset. *)

(* Memory operations. *)
| Store of size * 't * 't * 't      (* size,store,address,value *)
| Load of size * 't * 't            (* size,store,address *)
| Memcpy of size * 't * 't * 't     (* size,store,from_address,to_address *)          
| Malloc of Transfer_functions.Malloc_id.t * int * 't  (* malloc_size,store -> store * address *)
| Free of 't * 't                   (* store,address -> store *)
| Builtin of Builtin.t * 't         (* tuple -> tuple *)
| Memunknown of time
(* TODO: Alloc and free, that creates and remove new bases of a given
   (variable) size. Addr should be replaced by such a base. *)

(* Other binders. Currently not used *)
| Lambda of 'var * 't                (* For functions. *)
(* Forall *)

(* These terms are used only in the logic. Currently not used. *)
| Separated of 't * int * 't * int	(* addra,sizea,addrb,sizeb *)
| Offset of 't                      (* binary<size> -> integer *)
;;

(* Some explanations on the terms:

   - The _kind and _term have a _ prefix so that they are not confused
   with the type in Term_type (in particular when both modules are
   [open]ed).

   - Polymorphic operators have their type explicited when it cannot
   always be known (it is the case for Nondet{})

   - When arguments are commutative and associative, they are put
   together using a map or a set. This allows to get a canonical
   representation (using affine combinations), and triggers some
   simplifications.

   MAYBE this should be changed, because maps and sets cannot be
   associated to abstract values, which is inconvenient for backward
   propagation. So terms should instead be sorted using a binary
   search tree, i.e. with interior nodes also being binary search
   trees. This would also simplify interpretation. Also, it would
   allow to put flags on some constructors, such as "no signed wrap"
   on addition.
   
   - <,<=, for binaries (modulo arithmetic) are necessary
   predicates. But for non-modulo arithmetic (e.g integers), we can
   use unary predicates that compare to 0 instead (and substract both
   sides of the operand). Unary predicates are simpler to handle
   (e.g. in DBMs). 

   - If_not_bottom allows conditional rewrites: if one of the term in the
     set is bottom, then the value is bottom; else it is the value
     TODO: Replace par des "assume(unblocked(...),x)"

   - Assumes can be placed at different places: assume(c,x1) +
     assume(c,x2) is equal to assume(c,x1+x2). The former is better in
     forward abstract interpretation, as it avoids the need to 
     compute x1 + x2 in a condition other than c. The latter is better
     in backward abstract interpretation for the same reason; it is
     also less repetitive. Thus, placement of the assume conditions allows
     to tune how the domains will perform wrt precision and efficiency.

   - De Bruijn levels have the advantage of being nameless, and that a
     reference to a variable has the same name whatever the binding
     depth. Also, they correspond to the loop nesting. This make them
     more convenient and readable than, for instance, De Bruijn
     indices. 

     Note however that we still need global names. A our focus is
     context-dependent abstract interpretation, the value associated
     with a term depends on the context where is the term (i.e. the
     value of the variable "2" depends on the loop its'in, so we want
     a unique name for it). If we had a fully modular abstract
     interpreter, then using De Bruijn indices would be the best idea. *)

type ('var, 'set, 'intmap, 'parents) _term = {
    id: int;
    kind: ('var, ('var, 'set, 'intmap, 'parents) _term, 'set, 'intmap) _kind;
    parents: 'parents
  }
