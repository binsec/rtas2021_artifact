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

module Make(O:Term_smart_constructors_sig.S):Term_smart_constructors_sig.S = struct

  open Term_constructors;;
  
  module Boolean = struct
    let true_ = O.Boolean.true_
    let false_ = O.Boolean.false_

    let (&&) a b =
      match (a,b) with
      | {kind = True}, o | o, {kind = True} -> o
      | {kind = False}, _ | _, {kind = False} -> false_
      | _, _ when a == b -> a
      | _,_ -> O.Boolean.(&&) a b
    ;;

    let (||) a b =
      match (a,b) with
      | {kind = True}, _ | _, {kind = True} -> true_
      | {kind = False}, o | o, {kind = False} -> o
      | _, _ when a == b -> a
      | _,_ -> O.Boolean.(||) a b
    ;;

    let not t = match t.kind with
      | True -> false_
      | False -> true_
      | Not(nott) -> nott
      | _ -> O.Boolean.not t
    ;;
    
    let bottom = O.Boolean.bottom
    type boolean = O.Boolean.boolean

  end 


  module Tuple = struct
    include O.Tuple

    let assume cond x =
      match cond.kind with
      | True -> x
      | _ -> O.Tuple.assume cond x

    
    let nondet ~level types list = match list with
      | [x] -> x
      | _ -> nondet ~level types list

    let tuple_get tuple i =
      match tuple.kind with
      | Tuple_create(_,array) -> Immutable_dynamic_array.get array i
      | _ -> tuple_get tuple i

    
  end

  module Binary = struct

    include O.Binary

    let beq ~size a b = match a.kind,b.kind with
      | Biconst(_,a), Biconst(_,b) -> if Z.equal a b then O.Boolean.true_ else O.Boolean.false_
      | _ -> O.Binary.beq ~size a b

    let valid ~size access_type bin = match bin.kind with
      | Tuple_get({kind=Malloc _},0) -> O.Boolean.true_
      | _ -> O.Binary.valid ~size access_type bin
    ;;

    (* Note: Also happens a lot (TODO: Fix the source of these spurious extracts instead) *)
    let bextract ~size ~index ~oldsize x =
      assert(oldsize == Term_type_inference.size_of x);
      match oldsize with
      | s2 when s2 == size -> assert (index == 0); x
      | _ -> O.Binary.bextract ~size ~index ~oldsize x

  end

  module Integer = O.Integer

  module Memory = struct
    include O.Memory
    let assume cond x =
      match cond.kind, x.kind  with
      | True,_ -> x
      | _, Assume(c,_) when c == cond -> x
      | _ -> O.Memory.assume cond x

    let store ~size mem addr value =
      match mem.kind with
      | Store(oldsize,oldmem,oldaddress,oldvalue)
        when oldsize == size && oldaddress == addr ->
        O.Memory.store ~size oldmem addr value
      | _ -> O.Memory.store ~size mem addr value

  end

end
