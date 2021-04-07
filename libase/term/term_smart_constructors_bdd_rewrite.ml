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

  (* TODO: Make it so that oldest terms sink to the bottom, are they
     are more likely not to change (and thus this should increase
     sharing). *)
  module BDD_ = Bdd.Make(struct
      open Term_constructors
      include Term_type.Term_datatype
      let compare a b = Pervasives.compare a.id b.id
    end
    )
  module BDDTerm = struct
    include BDD_.BDD;;
    include WithCache(struct let cache_default_size = 199 end)
  end
  module BDDTermHash = Hashtbl.Make(BDDTerm);;

  module Term_weakhashtbl = Ephemeron.K1.Make(Term_type.Term_datatype)
  let term_to_bdd_cache: BDDTerm.t Term_weakhashtbl.t = Term_weakhashtbl.create 17;;
  let bdd_to_term_cache (*: Term.t BDDTermHash.t = *) = BDDTermHash.create 17;;

  let rec term_to_bdd_ term =
    (* Kernel.feedback "Term to bdd %a" Term.pretty term; *)
    let open Term_constructors in
    match term.kind with
    | False -> BDDTerm.zero
    | True -> BDDTerm.one
    | Not t -> BDDTerm.(!~) @@ term_to_bdd t
    | And(t1,t2) -> BDDTerm.(&&~) (term_to_bdd t1) (term_to_bdd t2)
    | Or(t1,t2) ->  BDDTerm.(||~) (term_to_bdd t1) (term_to_bdd t2)
    | _ -> BDDTerm.var term
  and term_to_bdd term =
    try Term_weakhashtbl.find term_to_bdd_cache term
    with Not_found ->
      let res = term_to_bdd_ term in
      Term_weakhashtbl.replace term_to_bdd_cache term res;
      assert ((bdd_to_term res) == term);
      res

  and bdd_to_term_ bdd =
    match bdd with
    | BDDTerm.Zero -> O.Boolean.false_
    | BDDTerm.One -> O.Boolean.true_
    | BDDTerm.If(_,term,then_,else_) ->
      O.Boolean.(||)
        (O.Boolean.(&&) term @@ bdd_to_term then_)
        (O.Boolean.(&&) (O.Boolean.not term) @@ bdd_to_term else_)
  and bdd_to_term bdd =
    try BDDTermHash.find bdd_to_term_cache bdd
    with Not_found ->
      let res = bdd_to_term_ bdd in
      BDDTermHash.replace bdd_to_term_cache bdd res;
      assert ((term_to_bdd res) == bdd);
      res
  ;;

  let and_terms t1 t2 = bdd_to_term @@ BDDTerm.(&&~) (term_to_bdd t1) (term_to_bdd t2);;
  let or_terms t1 t2 = bdd_to_term @@ BDDTerm.(||~) (term_to_bdd t1) (term_to_bdd t2);;
  
  module Boolean = struct
    let true_ = O.Boolean.true_
    let false_ = O.Boolean.false_

    let (&&) t1 t2 = bdd_to_term @@ BDDTerm.(&&~) (term_to_bdd t1) (term_to_bdd t2);;
    let (||) t1 t2 = bdd_to_term @@ BDDTerm.(||~) (term_to_bdd t1) (term_to_bdd t2);;

    let not t = bdd_to_term @@ BDDTerm.(!~) (term_to_bdd t);;

    let bottom = O.Boolean.bottom
    type boolean = O.Boolean.boolean
    
  end 

  module Tuple = O.Tuple
  module Binary = O.Binary
  module Integer = O.Integer
  module Memory = O.Memory

end
