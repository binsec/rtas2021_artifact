(**************************************************************************)
(*                                                                        *)
(*  This file is part of Frama-C.                                         *)
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

(* Note that we do not combine the information coming from both
   domains; there is no generic way to do it. Combination should be
   done in the caller of this functor.

   XXX: This is because we are not really making a product of domains,
   but merely calling two domains with the same input. Hmm, still
   we should be able to intersect the queries; weird.

*)


(* Make every operation twice. We need a pair of boolean identifiers,
   a pair of binary identifiers, etc. Only truth_value benefits from
   the interaction. 

   TODO: call eval + inject on all boolean operations, then on all operations. *)
open Domain_sig;;
module type S =  Base


module Make
    (P1:S)
    (P2:S
     with type Query.Boolean_Lattice.t = Lattices.Quadrivalent.t):S
  with type Context.t = P1.Context.t * P2.Context.t
  and type binary = P1.binary * P2.binary
  and type integer = P1.integer * P2.integer
  and type boolean = P1.boolean * P2.boolean
  and type Query.Boolean_Lattice.t = P1.Query.Boolean_Lattice.t * P2.Query.Boolean_Lattice.t                                    
  and type Query.Binary_Lattice.t = P1.Query.Binary_Lattice.t * P2.Query.Binary_Lattice.t
  and type Query.Integer_Lattice.t = P1.Query.Integer_Lattice.t * P2.Query.Integer_Lattice.t                                                                  
=
struct

  let name = P1.name ^ "*" ^ P2.name
  let unique_id = Domain_sig.Fresh_id.fresh name;;

  module Types = struct
    type boolean = P1.boolean * P2.boolean
    type integer = P1.integer * P2.integer                                  
    type binary = P1.binary * P2.binary
    type memory = P1.memory * P2.memory
  end
  include Types

  type 'a in_tuple = In: 'a P1.in_tuple * 'b P2.in_tuple -> ('a * 'b) in_tuple
  type 'a in_acc = bool * 'a in_tuple
  type 'b out_tuple = Out: 'a P1.out_tuple * 'b P2.out_tuple -> ('a * 'b) out_tuple
  type empty_tuple = (P1.empty_tuple * P2.empty_tuple)
  let empty_tuple = In(P1.empty_tuple,P2.empty_tuple)

  type ('a,'b) result =
      Result: bool * 'some in_tuple * ('some out_tuple -> 'a * 'b out_tuple) -> ('a,'b) result

  type 'elt higher1 = {subf1: 'tl. P1.Context.t -> 'elt -> 'elt -> 'tl P1.in_acc -> ('elt,'tl) P1.result  } [@@unboxed]
  type 'elt higher2 = {subf2: 'tl. P2.Context.t -> 'elt -> 'elt -> 'tl P2.in_acc -> ('elt,'tl) P2.result  } [@@unboxed]

  let serialize (type c) {subf1} {subf2} (ctx1,ctx2) (a1,a2) (b1,b2) (acc:c in_acc): (_,c) result =
    let (included, In (acc1,acc2)) = acc in
    let P1.Result(inc1,acc1,d1) = subf1 ctx1 a1 b1 (included,acc1) in
    let P2.Result(inc2,acc2,d2) = subf2 ctx2 a2 b2 (included,acc2) in
    Result(inc1 && inc2, In(acc1,acc2), fun (Out(tup1,tup2)) ->
      let r1,tup1 = d1 tup1 in
      let r2,tup2 = d2 tup2 in
      let tup = Out(tup1,tup2) in
      (r1,r2),tup)
  ;;
  
  let serialize_memory ctx a b acc =
    serialize {subf1=P1.serialize_memory} {subf2=P2.serialize_memory} ctx a b acc
  let serialize_integer ctx a b acc =
    serialize {subf1=P1.serialize_integer} {subf2=P2.serialize_integer} ctx a b acc
  let serialize_boolean ctx a b acc =
    serialize {subf1=P1.serialize_boolean} {subf2=P2.serialize_boolean} ctx a b acc      
  let serialize_binary ~size ctx a b acc =
    serialize
      {subf1=fun ctx a b acc -> P1.serialize_binary ~size ctx a b acc}
      {subf2=fun ctx a b acc -> P2.serialize_binary ~size ctx a b acc}
      ctx a b acc

  let typed_nondet2 (type c) (ctx1,ctx2) (In(tup1,tup2):c in_tuple): c out_tuple =
    Out(P1.typed_nondet2 ctx1 tup1,
        P2.typed_nondet2 ctx2 tup2)
  ;;

  let typed_fixpoint_step (type c) (ctx1,ctx2) (included,In(tup1,tup2):c in_acc):
    bool * (close:bool -> c out_tuple) =
    let bool1,k1 = P1.typed_fixpoint_step ctx1 (included, tup1) in
    let bool2,k2 = P2.typed_fixpoint_step ctx2 (bool1, tup2) in    
    bool2, (fun ~close ->
        let tup1 = k1 ~close in
        let tup2 = k2 ~close in
        Out(tup1,tup2)
      )
  ;;

  module Binary = Datatype_sig.Prod2(P1.Binary)(P2.Binary)
  module Integer = Datatype_sig.Prod2(P1.Integer)(P2.Integer)
  module Boolean = Datatype_sig.Prod2(P1.Boolean)(P2.Boolean)            

  (**************** Transfer functions ****************)

  (* TODO: We could probably factor using arity. *)
  module Boolean_Forward = struct
    let not (ctx1,ctx2) (v1,v2) = (P1.Boolean_Forward.not ctx1 v1,
                                   P2.Boolean_Forward.not ctx2 v2)
    let (&&) (ctx1,ctx2) (a1,a2) (b1,b2) = (P1.Boolean_Forward.(&&) ctx1 a1 b1,
                                            P2.Boolean_Forward.(&&) ctx2 a2 b2);;
    let (||) (ctx1,ctx2) (a1,a2) (b1,b2) = (P1.Boolean_Forward.(||) ctx1 a1 b1,
                                            P2.Boolean_Forward.(||) ctx2 a2 b2);;
    let true_ (ctx1,ctx2) = P1.Boolean_Forward.true_ ctx1,
                            P2.Boolean_Forward.true_ ctx2;;
    let false_ (ctx1,ctx2) = P1.Boolean_Forward.false_ ctx1,
                             P2.Boolean_Forward.false_ ctx2;;
  end

  module Integer_Forward = struct
    module IF1 = P1.Integer_Forward
    module IF2 = P2.Integer_Forward

    let itimes k (ctx1,ctx2) (v1,v2) = IF1.itimes k ctx1 v1, IF2.itimes k ctx2 v2;;
    let iconst k (ctx1,ctx2)  = IF1.iconst k ctx1, IF2.iconst k ctx2;;    
    let iadd (ctx1,ctx2) (v1,v2) (w1,w2) = IF1.iadd ctx1 v1 w1, IF2.iadd ctx2 v2 w2;;
    let imul (ctx1,ctx2) (v1,v2) (w1,w2) = IF1.imul ctx1 v1 w1, IF2.imul ctx2 v2 w2;;
    let idiv (ctx1,ctx2) (v1,v2) (w1,w2) = IF1.idiv ctx1 v1 w1, IF2.idiv ctx2 v2 w2;;
    let imod (ctx1,ctx2) (v1,v2) (w1,w2) = IF1.imod ctx1 v1 w1, IF2.imod ctx2 v2 w2;;
    let ishl (ctx1,ctx2) (v1,v2) (w1,w2) = IF1.ishl ctx1 v1 w1, IF2.ishl ctx2 v2 w2;;
    let ishr (ctx1,ctx2) (v1,v2) (w1,w2) = IF1.ishr ctx1 v1 w1, IF2.ishr ctx2 v2 w2;;
    let ior (ctx1,ctx2) (v1,v2) (w1,w2) = IF1.ior ctx1 v1 w1, IF2.ior ctx2 v2 w2;;
    let ixor (ctx1,ctx2) (v1,v2) (w1,w2) = IF1.ixor ctx1 v1 w1, IF2.ixor ctx2 v2 w2;;
    let iand (ctx1,ctx2) (v1,v2) (w1,w2) = IF1.iand ctx1 v1 w1, IF2.iand ctx2 v2 w2;;
    
    (* Derived constructors. *)
    let isub (ctx1,ctx2) (v1,v2) (w1,w2) = IF1.isub ctx1 v1 w1, IF2.isub ctx2 v2 w2;;
    let ieq (ctx1,ctx2) (v1,v2) (w1,w2) = IF1.ieq ctx1 v1 w1, IF2.ieq ctx2 v2 w2;;
    let ile (ctx1,ctx2) (v1,v2) (w1,w2) = IF1.ile ctx1 v1 w1, IF2.ile ctx2 v2 w2;;
    let zero (ctx1,ctx2) = IF1.zero ctx1, IF2.zero ctx2
    let one (ctx1,ctx2) = IF1.one ctx1, IF2.one ctx2                             
  end
  
  module Binary_Forward = struct
    module BF1 = P1.Binary_Forward
    module BF2 = P2.Binary_Forward

    let buninit ~size (ctx1,ctx2) = BF1.buninit ~size ctx1, BF2.buninit ~size ctx2;;
    let biconst ~size k (ctx1,ctx2) =
      BF1.biconst ~size k ctx1, BF2.biconst ~size k ctx2;;

    let bop1 ~size op1 op2 (ctx1,ctx2) (v1,v2) =
      op1 ~size ctx1 v1, op2 ~size ctx2 v2
    ;;
    let valid ~size acc_typ = bop1 (BF1.valid acc_typ) (BF2.valid acc_typ) ~size
    let bsext ~size ~oldsize = bop1 (BF1.bsext ~oldsize) (BF2.bsext ~oldsize) ~size
    let buext ~size ~oldsize = bop1 (BF1.buext ~oldsize) (BF2.buext ~oldsize) ~size
    let bofbool ~size = bop1 (BF1.bofbool) (BF2.bofbool) ~size
    let bchoose ~size cond = bop1 (BF1.bchoose cond) (BF2.bchoose cond) ~size
    
    let bextract ~size ~index ~oldsize (ctx1,ctx2) (v1,v2) =
      BF1.bextract ~size ~index ~oldsize ctx1 v1, BF2.bextract ~size ~index ~oldsize ctx2 v2;;

    let bop2_no_size op1 op2 (ctx1,ctx2) (v1,v2) (w1,w2) =
      op1 ctx1 v1 w1, op2 ctx2 v2 w2
    
    let bpred = bop2_no_size

    (* Note: We cannot cross the results with inject_boolean in
       general: if domains abstracts unrelated pieces of information,
       the fact that one is true does not imply that the other
       is. E.g.  if one domain abstracts the set of memory regions to
       which a pointer may points, and the other the possible offsets
       of the pointers, we cannot deduce from a[0] != b[0] that 0 != 0. 

       So, crossing informations like this should take place in the
       caller of this functor. *)

    let bisle ~size = bpred (BF1.bisle ~size) (BF2.bisle ~size);;
    let biule ~size = bpred (BF1.biule ~size) (BF2.biule ~size);;
    let beq   ~size = bpred (BF1.beq   ~size) (BF2.beq ~size);;


    let bop2_flags ~size ~nsw ~nuw bop1 bop2 = bop2_no_size (bop1 ~size ~nsw ~nuw) (bop2 ~size ~nsw ~nuw)    
    let bop2 ~size bop1 bop2 = bop2_no_size (bop1 ~size) (bop2 ~size)
    
    let valid_ptr_arith ~size = bop2 BF1.valid_ptr_arith BF2.valid_ptr_arith ~size

    let biadd = bop2_flags BF1.biadd BF2.biadd
    let bisub = bop2_flags BF1.bisub BF2.bisub
    let bimul = bop2_flags BF1.bimul BF2.bimul
    let bshl =  bop2_flags BF1.bshl BF2.bshl
        
    let blshr = bop2 BF1.blshr BF2.blshr
    let bashr = bop2 BF1.bashr BF2.bashr

    let biumod = bop2 BF1.biumod BF2.biumod
    let biudiv = bop2 BF1.biudiv BF2.biudiv
    let bismod = bop2 BF1.bismod BF2.bismod
    let bisdiv = bop2 BF1.bisdiv BF2.bisdiv

    let bxor = bop2 BF1.bxor BF2.bxor
    let bor = bop2 BF1.bor BF2.bor
    let band = bop2 BF1.band BF2.band

    let bconcat ~size1 ~size2 = bop2_no_size (BF1.bconcat ~size1 ~size2) (BF2.bconcat ~size1 ~size2)
    
    let bshift ~size ~offset ~max _ = assert false
    let bindex ~size _ = assert false    

    
  end
  module Memory_Forward = Assert_False_Transfer_Functions.Memory.Memory_Forward

  (**************** Context ****************)
  module Context = struct
    type t = P1.Context.t * P2.Context.t
    let level (x1,x2) =
      let res = P1.Context.level x1 in
      assert(res == P2.Context.level x2);
      res
  end
  
  type root_context = (P1.root_context * P2.root_context)
  let root_context_upcast (root1,root2) = (P1.root_context_upcast root1, P2.root_context_upcast root2)

  let root_context () = (P1.root_context(), P2.root_context())
  
  type mu_context = (P1.mu_context * P2.mu_context)                      

  let mu_context_open (ctx1,ctx2) = (P1.mu_context_open ctx1, P2.mu_context_open ctx2)
  let mu_context_upcast (mu1,mu2) = (P1.mu_context_upcast mu1, P2.mu_context_upcast mu2)

  (**************** Tuple ****************)

  include Builtin.Make(Types)(Context)
  
  (**************** Requests ****************)
  let memory_pretty (ctx1,ctx2) fmt (mem1,mem2) = assert false
  let memory_is_empty (mem1,mem2) = assert false
  let boolean_pretty (ctx1,ctx2) fmt (mem1,mem2) =
    Format.fprintf fmt "(%a,%a)" (P1.boolean_pretty ctx1) mem1 (P2.boolean_pretty ctx2) mem2;;
  let integer_pretty (ctx1,ctx2) fmt (mem1,mem2) =
    Format.fprintf fmt "(%a,%a)" (P1.integer_pretty ctx1) mem1 (P2.integer_pretty ctx2) mem2;;
  let boolean_is_empty (mem1,mem2) = assert false
  let binary_pretty ~size (ctx1,ctx2) fmt (mem1,mem2) =
    Format.fprintf fmt "(%a,%a)"
      (P1.binary_pretty ~size ctx1) mem1
      (P2.binary_pretty ~size ctx2) mem2;;
  let integer_is_empty (int1,int2) = assert false
  let binary_is_empty ~size (mem1,mem2) = assert false
  
  module Query = struct
    module Boolean_Lattice = struct
      include Lattices.Prod.Prod2_With_Inter_Bottom(P1.Query.Boolean_Lattice)(P2.Query.Boolean_Lattice)
      let top = P1.Query.Boolean_Lattice.top, P2.Query.Boolean_Lattice.top
      let singleton x = P1.Query.Boolean_Lattice.singleton x, P2.Query.Boolean_Lattice.singleton x
    end
    (* module Binary = Lattices.Prod.Prod2_With_Inter_Bottom(P1.Query.Binary)(P2.Query.Binary) *)
    module Binary_Lattice = struct
      (* include Lattices.Dummy *)
      (* include Datatype_sig.Prod2(P1.Query.Binary_Lattice)(P2.Query.Binary_Lattice) *)

      module A = P1.Query
      module B = P2.Query
      type t = A.Binary_Lattice.t * B.Binary_Lattice.t
      let equal (a1,b1) (a2,b2) = A.Binary_Lattice.equal a1 a2 && B.Binary_Lattice.equal b1 b2
      let compare (a1,b1) (a2,b2) =
        let ra = A.Binary_Lattice.compare a1 a2 in
        if ra != 0 then ra
        else B.Binary_Lattice.compare b1 b2
      let sdbm x y = y + (x lsl 16) + (x lsl 6) - x;;          
      let hash (a,b) = sdbm (A.Binary_Lattice.hash a) (B.Binary_Lattice.hash b)
      let pretty ~size fmt (a,b) = Format.fprintf fmt "(%a,%a)" (A.Binary_Lattice.pretty ~size) a (B.Binary_Lattice.pretty ~size) b

      let top ~size = P1.Query.Binary_Lattice.top ~size, P2.Query.Binary_Lattice.top ~size
      
      let inter ~size (a1,a2) (b1,b2) =
        P1.Query.Binary_Lattice.inter ~size a1 b1, P2.Query.Binary_Lattice.inter ~size a2 b2
      let is_bottom ~size _ = assert false
      let includes ~size _ = assert false
      let join ~size _ = assert false
      let bottom ~size = P1.Query.Binary_Lattice.bottom ~size, P2.Query.Binary_Lattice.bottom ~size
      let widen ~size ~previous _ = assert false
      let includes_or_widen ~size ~previous _ = assert false
      let singleton ~size k = P1.Query.Binary_Lattice.singleton ~size k,
                              P2.Query.Binary_Lattice.singleton ~size k
    end

    
    (* let convert_to_cvalue (lat1,lat2) = *)
    (*     Cvalue.V.narrow *)
    (*       (P1.Query.convert_to_cvalue lat1) *)
    (*       (P2.Query.convert_to_cvalue lat2) *)
    module Integer_Lattice = struct
      include Lattices.Prod.Prod2_With_Inter_Bottom(P1.Query.Integer_Lattice)(P2.Query.Integer_Lattice)
      let top = P1.Query.Integer_Lattice.top, P2.Query.Integer_Lattice.top
      let singleton x = P1.Query.Integer_Lattice.singleton x, P2.Query.Integer_Lattice.singleton x
    end

    (* To be filled by the caller of this functor. *)
    let convert_to_ival (lat1,lat2) = assert false
    let is_singleton_int (lat1,lat2) = assert false

    let boolean (ctx1,ctx2) (id1,id2) =
        (P1.Query.boolean ctx1 id1),
        (P2.Query.boolean ctx2 id2)


    (* To be filled by the caller of this functor. *)
    let convert_to_quadrivalent _ = assert false
    let binary_to_ival ~signed ~size _ = assert false

    let binary_to_known_bits ~size (a,b) =
      Single_value_abstraction.Known_bits.inter ~size
        (P1.Query.binary_to_known_bits ~size a) (P2.Query.binary_to_known_bits ~size b)
    ;;

    
    let binary_fold_crop ~size bin ~inf ~sup f acc = assert false
    let binary_is_empty ~size (x1,x2) =
       P1.Query.binary_is_empty ~size x1
    || P2.Query.binary_is_empty ~size x2
    
    let integer (ctx1,ctx2) (id1,id2) =
      (P1.Query.integer ctx1 id1, P2.Query.integer ctx2 id2)
    let binary ~size (ctx1,ctx2) (id1,id2) =
      (P1.Query.binary ~size ctx1 id1,
       P2.Query.binary ~size ctx2 id2)

    let reachable (ctx1,ctx2) (mem1,mem2) =
      Quadrivalent.inter
        (P1.Query.reachable ctx1 mem1)
        (P2.Query.reachable ctx2 mem2)

    let binary_is_singleton ~size (a,b) =
      match P1.Query.binary_is_singleton ~size a with
      | None -> P2.Query.binary_is_singleton ~size b
      | x -> x
    ;;

  end

  let satisfiable (ctx1,ctx2) (b1,b2) =
    match P1.satisfiable ctx1 b1 with
    | (Domain_sig.Unsat | Domain_sig.Sat _ ) as r -> r
    | _ -> P2.satisfiable ctx2 b2
  ;;

  let reachable (ctx1,ctx2) (b1,b2) =
    match P1.reachable ctx1 b1 with
    | (Domain_sig.Unsat | Domain_sig.Sat _ ) as r -> r
    | _ -> P2.reachable ctx2 b2
  ;;

  let binary_empty ~size (ctx1,ctx2) = P1.binary_empty ctx1 ~size, P2.binary_empty ctx2 ~size
  let integer_empty (ctx1,ctx2) = P1.integer_empty ctx1, P2.integer_empty ctx2
  let boolean_empty (ctx1,ctx2) = P1.boolean_empty ctx1, P2.boolean_empty ctx2

  let binary_unknown ~size ~level (ctx1,ctx2) = P1.binary_unknown ctx1 ~size ~level, P2.binary_unknown ctx2 ~size ~level
  let integer_unknown ~level (ctx1,ctx2) = P1.integer_unknown ctx1 ~level, P2.integer_unknown ctx2 ~level
  let boolean_unknown ~level (ctx1,ctx2) = P1.boolean_unknown ctx1 ~level, P2.boolean_unknown ctx2 ~level

  
  let assume_boolean (ctx1,ctx2) (cond1,cond2) (v1,v2) =
      P1.assume_boolean ctx1 cond1 v1, P2.assume_boolean ctx2 cond2 v2

  let assume_integer (ctx1,ctx2) (cond1,cond2) (v1,v2) =
      P1.assume_integer ctx1 cond1 v1, P2.assume_integer ctx2 cond2 v2

  let assume_memory (ctx1,ctx2) (cond1,cond2) (v1,v2) =
      P1.assume_memory ctx1 cond1 v1, P2.assume_memory ctx2 cond2 v2

    let assume_binary ~size (ctx1,ctx2) (cond1,cond2) (v1,v2) =
      P1.assume_binary ~size ctx1 cond1 v1, P2.assume_binary ~size ctx2 cond2 v2

  
  let union _ = assert false

  let should_focus ~size:_ _ = assert false
  let may_alias ~size:_ _ = assert false

end

