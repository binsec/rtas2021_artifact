(**************************************************************************)
(*                                                                        *)
(*  This file is part of the Codex plugin of Frama-C.                     *)
(*                                                                        *)
(*  Copyright (C) 2007-2016                                               *)
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


open Term_type


module type S = sig
  module Boolean:sig
    type boolean = term
    include Transfer_functions.Boolean_Forward with module Arity := Basis_sig.Pure_Arity.Forward
                                                and type boolean := term
    val bottom: boolean
  end

  module Integer:sig
    type boolean = term
    type integer = term
    include Transfer_functions.Integer_Forward with module Arity := Basis_sig.Pure_Arity.Forward
                                                and type integer := term
                                                and type boolean := term
    val iunknown_explicit: level:int -> integer
    val bottom: integer
  end


  module Binary:sig
    type boolean = term
    type binary = term
    include Transfer_functions.Binary_Forward with module Arity := Basis_sig.Pure_Arity.Forward
                                               and type binary := term
                                               and type boolean := term
    val bunknown_explicit: size:int -> level:int -> binary
    val bottom: size:int -> binary
    val assume: size:int -> boolean -> binary -> binary
  end 

  module Memory:sig
    type boolean = term
    type binary = term
    type memory = term
    include Transfer_functions.Memory_Forward with module Arity := Basis_sig.Pure_Arity.Forward
                                               and type memory := term
                                               and type binary := term
                                               and type boolean := term
    val assume: (boolean,memory,memory) Basis_sig.Pure_Arity.Forward.ar2    
    val bottom: memory
  end

  module Tuple:sig
    (* Create a new variable of type tuple, initially with a 0-size tuple. *)
    val var: level:int -> term

    (* Append a type to the tuple type of a variable.  *)
    val append_type: term -> Term_constructors.term_type -> unit

    (* [except tuple i a] creates a new tuple where the ith element
       was replaced by a. *)
    val except: term -> int -> term -> term

    val tuple_create: term Immutable_dynamic_array.t -> term
    val tuple_get: term -> int -> term

    val assume: term -> term -> term
    val nondet: level:int -> Term_constructors.term_type Immutable_dynamic_array.t -> term list -> term
    val builtin: Builtin.t -> term -> term

    val mu_explicit: var:term -> body:term -> level:int -> arg:term -> term      
  end

end
