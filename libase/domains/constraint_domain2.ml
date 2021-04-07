(**************************************************************************)
(*                                                                        *)
(*  This file is part of the Codex plugin of Frama-C.                     *)
(*                                                                        *)
(*  Copyright (C) 2007-2018                                               *)
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

[@@@warning "-33"]              (* Constraints.() gives spurious warning in 4.04.2 *)

(* Should we use first-order formula (over-approximation) or horn
   clauses (exact) to the SMT solver. *)
let option_translate: [ `First_order | `Horn ] = `Horn;;

(* How should we pretty-print constraints: only their value (easier to read),
   symbolically (better when there are issues with evaluation), or print both
   (more complete debugging info).  *)
let option_pretty_constraints: [`Symbolic | `Value | `Both ] = `Value

(* Option to introduce a fresh variable at every fixpoint
   step. Mandatory if we do not push/pop.

   Results: faster on almost all benchmarks, except papabench;
   probably the variable order is better that way for bdd
   construction. It also fixes a precision issues in t072. *)
let option_fresh_variable_every_time =
  (not Domains_constraints_constraint_propagation.option_push_and_pop) ||
  (not Domains_constraints_constraint_propagation.option_push_and_pop_on_restart) ||
  true;;

(* If true, whenever the result of a computation is a constant, we
   simplify the constraint using the constant. This allows to detect
   equality more often, and to have fewer variables.
   It shows a small gain on the benchmarks. *)
let option_semantic_simplification_constraints = true

module TC = Term_constructors_gadt

module Make
    (Constraints:Constraints_constraints_sig.Constraints)
    (Domain:Constraint_domains_sig.Domain_S with module Constraints = Constraints)
  :Domain_sig.Base = struct

  let name = "Constraint_Domain2(" ^ Domain.name ^ ")";;
  let unique_id = Domain_sig.Fresh_id.fresh name;;

  (* Domain also represents the condition for which the identifier is
     defined, and constrain its term. If the condition is false, we
     use bottom as the term is useless. *)
  (* MAYBE: Not sure it's worth the trouble here (it is in
     With_Bottom), or for evaluation.  I could just have a
     Domain.bottom, and use Constrain.empty with dommain.bottom as the
     bottom value.  But it is also nice not to have to handle bottom
     in every abstract domain...  *)
  type 'a identifier =
    | Bottom 
    | NonBottom of {
        constrain: 'a Constraints.t;
        domain: Domain.t
      };;

  module Types = struct
    type binary = TC.binary identifier
    type memory = unit identifier
    type integer = TC.integer identifier
    type boolean = TC.boolean identifier
  end

  include Types

  module Identifier = struct

    let pretty fmt = function
      | Bottom -> Format.fprintf fmt "<bottom>"
      | NonBottom x ->
        Format.fprintf fmt "{constrain %a domain %a}"
          Constraints.pretty x.constrain
          Domain.pretty x.domain

    (* This is used by the above domains to decide whether the values
       are the same, and if they should be serialized. It is important
       to serialize if the constraints are equal and the domains are
       different, else we loose information about conditions. *)
    let _equal a b =
      let res = match a,b with
        | Bottom, Bottom -> true
        | NonBottom a, NonBottom b ->
          Constraints.equal a.constrain b.constrain
          && Domain.equal a.domain b.domain
        | Bottom, NonBottom _ | NonBottom _, Bottom -> false
      in
      res
    ;;

    (* Actually, this check is done really often, and this
       approximation yields the correct result most of the
       time. Moreover, if we do not detect equality here, we will
       probably do it in fixpoint_step or nondet. Using this
       implementation gave me a 10-20% speedup in execution time. *)
    (*let equal a b = a == b;;*)
    let equal = _equal

    (* This does not check for equality of the [domain] fields. It is not
       necessary for the only current use case: a map indexed by [t]s to
       implement focusing. *)
    let compare a b =
      match a,b with
      | Bottom, Bottom -> 0
      | Bottom, NonBottom _ -> -1
      | NonBottom _, Bottom -> 1
      | NonBottom a, NonBottom b -> Constraints.compare a.constrain b.constrain

    let hash _ = assert false
  end

  module Binary = struct
    type t = binary
    include Identifier
  end

  module Integer = struct
    type t = integer
    include Identifier
  end

  module Boolean = struct
    type t = boolean
    include Identifier
  end

  (**************** Context ****************)

  type mu_context = {
    mu_ctx: context;              (* Context corresponding to this mu_context *)
    mu_parent_ctx: context;       (* Parent of the current context. *)
  }

  and root_context = {
    root_ctx: context;            (* Context corresponding to this root_context *)
  }

  and context_ =
    | Mu_context of mu_context
    | Root_context of root_context

  and context =
    { ctx:context_;
      level:int;
    }
  ;;

  module Context = struct
    type t = context
    let level ctx = ctx.level
  end

  let root_context_upcast ctx = ctx.root_ctx

  let root_context() =
    let rec root_ctx = {
      root_ctx = ctx;
    }
    and ctx = {
      ctx = Root_context root_ctx;
      level = 0;
    }
    in root_ctx
  ;;

  (**************** Transfer functions ****************)

  include Builtin.Make(Types)(Context)

  (* This optimization is very important, else in particular we build
     complex formula that are actually empty. *)
  (* Note: We can just build the constrain here, because the value for
     the boolean constants is already evaluated. *)
  let opt_boolean x =
    match x with
    | Bottom -> Bottom
    | NonBottom {constrain;domain} as x ->
      match Domain.Query.boolean domain constrain with
      | Quadrivalent_Lattice.True -> NonBottom {constrain=Constraints.Build.Boolean.true_;domain}
      | Quadrivalent_Lattice.False -> NonBottom {constrain=Constraints.Build.Boolean.false_;domain}
      | Quadrivalent_Lattice.Bottom -> NonBottom {constrain=Constraints.Build.Boolean.empty;domain}
      | _ -> x
  ;;

  let ar0 constrain fdomain =
    NonBottom{ constrain; domain = fdomain Domain.top constrain }
  ;;

  let ar0_boolean = ar0
  let ar0_integer = ar0
  let ar0_binary = ar0        

  let iconst k = ar0 (Constraints.Build.Integer.iconst k) (Domain.Integer_Forward.iconst k);;
  let biconst ~size k = ar0 (Constraints.Build.Binary.biconst ~size k) (Domain.Binary_Forward.biconst ~size k);;  

  let opt_integer x =
    if option_semantic_simplification_constraints
    then match x with
      | Bottom -> Bottom
      | NonBottom {constrain;domain} as x ->
        let value = Domain.Query.integer domain constrain in
        match Domain.Query.is_singleton_int value with
        | None -> x
        | Some k -> iconst k
    else x
  ;;

  let opt_binary ~size x = 
    if option_semantic_simplification_constraints
    then match x with
      | Bottom -> Bottom
      | NonBottom {constrain;domain} as x ->
        let value = Domain.Query.binary ~size domain constrain in
        match Domain.Query.binary_is_singleton ~size value with
        | None -> x
        | Some k -> biconst ~size k
    else x
  ;;

  
  let ar1 fconstrain fdomain a =
    match a with
    | Bottom -> Bottom
    | NonBottom a ->
      let res = fconstrain a.constrain in
      let dom = fdomain a.domain a.constrain res in
      NonBottom{ 
        constrain = res;
        domain = dom
      }
  ;;

  let _ar1_integer_integer = ar1
  let ar1_integer_integer fconstrain fdomain a  =
    opt_integer @@ ar1 fconstrain fdomain a
  ;;

  let _ar1_binary_binary ~size = ar1
  let ar1_binary_binary ~size fconstrain fdomain a  =
    opt_binary ~size @@ ar1 fconstrain fdomain a
  ;;

  let ar1_boolean_binary ~size fconstrain fdomain a  =
    opt_binary ~size @@ ar1 fconstrain fdomain a
  ;;

  
  

  let ar1_boolean_boolean fconstrain fdomain a  =
    opt_boolean @@ ar1 fconstrain fdomain a
  ;;


  let ar2 fconstrain fdomain a b =
    match a,b with
    | Bottom, _ | _, Bottom -> Bottom
    | NonBottom a, NonBottom b ->
      let dom = Domain.inter a.domain b.domain in
      let res = fconstrain a.constrain b.constrain in
      let dom = fdomain dom a.constrain b.constrain res in
      NonBottom{ 
        constrain = res;
        domain = dom;
      }
  ;;

  let ar2' f a b =
    match a,b with
    | Bottom, _ | _, Bottom -> Bottom
    | NonBottom a, NonBottom b ->
      let dom = Domain.inter a.domain b.domain in
      let (constrain,domain) = f dom a.constrain b.constrain in
      NonBottom{ constrain; domain }
  ;;

  let ar2_binary_binary_binary' ~size f a b =
    opt_binary ~size @@ ar2' f a b;;

  let ar2_binary_binary_binary ~size fconstrain fdomain a b =
    opt_binary ~size @@ ar2 fconstrain fdomain a b;;
  
  let ar2_integer_integer_integer fconstrain fdomain a b =
    opt_integer @@ ar2 fconstrain fdomain a b;;

  let ar2_boolean_boolean_boolean fconstrain fdomain a b =
    opt_boolean @@ ar2 fconstrain fdomain a b 
  ;;

  let ar2_integer_integer_boolean =
    if option_semantic_simplification_constraints
    then ar2_boolean_boolean_boolean
    else ar2
  ;;

  let ar2_binary_binary_boolean ~size =
    if option_semantic_simplification_constraints
    then ar2_boolean_boolean_boolean
    else ar2
  ;;

  (* It is more precise to assume individual simple constraints; this function does that. *)
  let rec assume ctx cond pos domain =
    (* Codex_log.feedback "rec assuming %b %a %a" pos Constraints.pretty cond Domain.pretty domain; *)
    let join a b pos domain =
       Codex_log.warning "This was never tested";
       let dom1 = assume ctx a pos domain in
       let dom2 = assume ctx b pos domain in
       let tup = Immutable_array.empty in
       match dom1,dom2 with
       | None, dom | dom, None -> dom
       | Some doma, Some domb -> Some(Domain.nondet ~doma ~tupa:tup ~domb ~tupb:tup ~tupres:tup)
    in
    let inter a b pos domain = match assume ctx a pos domain with
       | None -> None
       | Some domain -> assume ctx b pos domain
    in
    match pos, cond with
    | _, Constraints.(Bool{term=T1{tag=TC.Not;a=Constraints.Bool _ as a}}) ->
       assume ctx a (not pos) domain
    | true,Constraints.(Bool{term=T2{tag=TC.And; a=Constraints.Bool _ as a; b=Constraints.Bool _ as b}}) ->
       inter a b pos domain
    (* Same, but we cannot factorize with OCaml 4.05 *)
    | false,Constraints.(Bool{term=T2{tag=TC.Or; a=Constraints.Bool _ as a; b=Constraints.Bool _ as b}}) ->
       inter a b pos domain
    | true, Constraints.(Bool{term=T2{tag=TC.Or; a=Constraints.Bool _ as a; b=Constraints.Bool _ as b}}) ->
       join a b pos domain
    | false, Constraints.(Bool{term=T2{tag=TC.And; a=Constraints.Bool _ as a; b=Constraints.Bool _ as b}}) ->
       join a b pos domain
    | _ ->
       match pos, Domain.Query.convert_to_quadrivalent @@ Domain.Query.boolean domain cond with
       | (_, Quadrivalent_Lattice.Bottom)
         | true, Quadrivalent_Lattice.False
         | false, Quadrivalent_Lattice.True -> None
       | true, Quadrivalent_Lattice.True
         | false,Quadrivalent_Lattice.False -> Some domain
       | (true, Quadrivalent_Lattice.Top) -> Domain.assume domain cond
       | (false,Quadrivalent_Lattice.Top) ->
          (* Need to evaluate the condition before propagation. *)
          let ncond = Constraints.Build.Boolean.not cond in
          let domain = Domain.Boolean_Forward.not domain cond ncond in
          Domain.assume domain ncond
          

  let assume ctx cond x =
    match cond, x with
    | Bottom, _ | _, Bottom -> Bottom
    | NonBottom{domain=domaincond;constrain=constraincond},
      NonBottom{domain=domainx;constrain=constrainx} ->
       let domain = Domain.inter domaincond domainx in
       (* Codex_log.feedback "nonrec assuming %a" Constraints.pretty constraincond; *)
       match assume ctx constraincond true domain with
       | None -> Bottom
       | Some domain -> NonBottom{constrain=constrainx;domain}   
  ;;    

  
  module Boolean_Forward = struct
    let (||) ctx = ar2_boolean_boolean_boolean Constraints.Build.Boolean.(||) Domain.Boolean_Forward.(||)
    let (&&) ctx = ar2_boolean_boolean_boolean Constraints.Build.Boolean.(&&) Domain.Boolean_Forward.(&&)        
    let not ctx = ar1_boolean_boolean Constraints.Build.Boolean.not Domain.Boolean_Forward.not
    (* Note: we avoid creating those every time. *)
    let true_ = let res = ar0_boolean Constraints.Build.Boolean.true_ Domain.Boolean_Forward.true_ in fun ctx -> res
    let false_ = let res = ar0_boolean Constraints.Build.Boolean.false_ Domain.Boolean_Forward.false_ in fun ctx -> res
  end

  module Integer_Forward' = struct
    let ile ctx = ar2_integer_integer_boolean Constraints.Build.Integer.ile Domain.Integer_Forward.ile      
    let ieq ctx = ar2_integer_integer_boolean Constraints.Build.Integer.ieq Domain.Integer_Forward.ieq

    let iconst k = ar0_integer (Constraints.Build.Integer.iconst k) (Domain.Integer_Forward.iconst k)
    let one = let res = iconst Z.one in fun ctx -> res
    let zero = let res = iconst Z.zero in fun ctx -> res
    let iconst k ctx = iconst k

    let ixor ctx = ar2_integer_integer_integer Constraints.Build.Integer.ixor Domain.Integer_Forward.ixor
    let ior  ctx = ar2_integer_integer_integer Constraints.Build.Integer.ior  Domain.Integer_Forward.ior 
    let iand ctx = ar2_integer_integer_integer Constraints.Build.Integer.iand Domain.Integer_Forward.iand
    let ishr ctx = ar2_integer_integer_integer Constraints.Build.Integer.ishr Domain.Integer_Forward.ishr
    let ishl ctx = ar2_integer_integer_integer Constraints.Build.Integer.ishl Domain.Integer_Forward.ishl
    let imod ctx = ar2_integer_integer_integer Constraints.Build.Integer.imod Domain.Integer_Forward.imod
    let idiv ctx = ar2_integer_integer_integer Constraints.Build.Integer.idiv Domain.Integer_Forward.idiv
    let imul ctx = ar2_integer_integer_integer Constraints.Build.Integer.imul Domain.Integer_Forward.imul
    let iadd ctx = ar2_integer_integer_integer Constraints.Build.Integer.iadd Domain.Integer_Forward.iadd
    let isub ctx = ar2_integer_integer_integer Constraints.Build.Integer.isub Domain.Integer_Forward.isub
    let itimes k ctx = ar1_integer_integer (Constraints.Build.Integer.itimes k) (Domain.Integer_Forward.itimes k)
  end

  module Integer_Forward = struct
    include Integer_Forward'

    (* Rewrite x + x as 2 * x. *)
    let iadd ctx a' b' = match a',b' with
      | Bottom, _ | _,Bottom -> Bottom
      | NonBottom a, NonBottom b ->
        if a.constrain == b.constrain
        then
          let two = Z.of_int 2 in
          let two = (Constraints.Build.Integer.iconst two) in          
          let constrain = Constraints.Build.Integer.imul two a.constrain in
          let domain = Domain.inter a.domain b.domain in
          let domain = Domain.Integer_Forward.imul domain two a.constrain constrain in
          NonBottom { constrain; domain }
        else iadd ctx a' b'
    ;;

  end

  module Binary_Forward = struct

    module No_Rewrite = struct
      let default fconstrain fdomain dom a b =
        let constrain = fconstrain a b in
        let domain = fdomain dom a b constrain in
        constrain,domain

      let biadd ~size ~nsw ~nuw = default (Constraints.Build.Binary.biadd ~size ~nsw ~nuw) (Domain.Binary_Forward.biadd ~size ~nsw ~nuw)
      let bisub ~size ~nsw ~nuw = default (Constraints.Build.Binary.bisub ~size ~nsw ~nuw) (Domain.Binary_Forward.bisub ~size ~nsw ~nuw)        
      let bimul ~size ~nsw ~nuw = default (Constraints.Build.Binary.bimul ~size ~nsw ~nuw) (Domain.Binary_Forward.bimul ~size ~nsw ~nuw)

      let bxor ~size = default (Constraints.Build.Binary.bxor ~size) (Domain.Binary_Forward.bxor ~size)
      let bor ~size = default (Constraints.Build.Binary.bor ~size) (Domain.Binary_Forward.bor ~size)
      let band ~size = default (Constraints.Build.Binary.band ~size) (Domain.Binary_Forward.band ~size)

      let bashr ~size = default (Constraints.Build.Binary.bashr ~size) (Domain.Binary_Forward.bashr ~size)
      let blshr ~size = default (Constraints.Build.Binary.blshr ~size) (Domain.Binary_Forward.blshr ~size)                                        
      let bshl ~size ~nsw ~nuw = default (Constraints.Build.Binary.bshl ~size ~nsw ~nuw) (Domain.Binary_Forward.bshl ~size ~nsw ~nuw)

      let bisdiv ~size = default (Constraints.Build.Binary.bisdiv ~size) (Domain.Binary_Forward.bisdiv ~size)
      let biudiv ~size = default (Constraints.Build.Binary.biudiv ~size) (Domain.Binary_Forward.biudiv ~size)        
      let bismod ~size = default (Constraints.Build.Binary.bismod ~size) (Domain.Binary_Forward.bismod ~size)
      let biumod ~size = default (Constraints.Build.Binary.biumod ~size) (Domain.Binary_Forward.biumod ~size)        
    end

    (* Rewrite shifts + additions/soustractions into multiplication by a
       constant. This is a common pattern in binary analysis. *)
    module Rewrite = struct
      include No_Rewrite

      let extract = function
        | Constraints.(Binary{term=T2{tag=TC.Bimul _size;
                                      a=Binary{term=T0{tag=TC.Biconst(_size2,k)}};
                                      b=Binary _ as b}}) -> (k,b)
        | Constraints.(Binary{term=T2{tag=TC.Bimul _size;
                                      a=Binary _ as a;
                                      b=Binary{term=T0{tag=TC.Biconst(_size2,k)}}}}) -> (k,a)
        | x-> (Z.one, x)
      ;;

      let extract_sum = function
        | Constraints.(Binary{term=T2{tag=TC.Biadd _size;
                                      a=Binary _ as a;
                                      b=Binary{term=T0{tag=TC.Biconst(_size2,k)};}}}) ->
            (a,k)
        | Constraints.(Binary{term=T2{tag=TC.Biadd _size;
                                      a=Binary{term=T0{tag=TC.Biconst(_size2,k)}};
                                      b=Binary _ as b;}}) ->
            (b,k)
        | x -> (x, Z.zero)

      let rec bimul ~size ~nsw ~nuw domain a b =
        match a,b with
        | Constraints.(Binary{term=T0{tag=TC.Biconst(_size1,k1)}}),
          Constraints.(Binary{term=T2{tag=TC.Bimul _size;a=Binary{term=T0{tag=TC.Biconst(_size2,k2)}};b=Binary _ as x}}) ->
          bitimes ~size ~nsw ~nuw domain (Z.mul k1 k2) x
        | Constraints.(Binary{term=T2{tag=TC.Bimul _size;a=Binary{term=T0{tag=TC.Biconst(_size2,k2)}};b=Binary _ as x}}),
          Constraints.(Binary{term=T0{tag=TC.Biconst(_size1,k1)}}) ->
          bitimes ~size ~nsw ~nuw domain (Z.mul k1 k2) x
        | _ -> No_Rewrite.bimul ~size ~nsw ~nuw domain a b

      and bitimes ~size ~nsw ~nuw domain k x =
        if Z.equal k Z.zero then (Constraints.Build.Binary.biconst ~size Z.zero), domain
        else if Z.equal k Z.one then (x,domain)
        else
          let default () =
            let constr = (Constraints.Build.Binary.biconst ~size k) in
            (* optional, as domains should handle evaluation of constants themselves *)
            let domain = Domain.Binary_Forward.biconst ~size k domain constr in
            No_Rewrite.bimul ~size ~nsw ~nuw domain constr x
          in
          (* Simplify k*(x' + c) into k*(x' - d), with 0<d<c, when possible.
             Useful when the compiler does e.g.  16*(N+(2^28-1)) instead of
             straightforwardly 16*(N-1), for optimization reasons. When that happens,
             when dividing by 16 to get the index in an array, we have another
             rewrite that returns N+(2^28-1) and that's actually incorrect, so
             we want to return N-1 instead. It looks like bad engineering that
             the other rewrite (in [bisdiv]) may be unsound, and that we have
             to fix it here, though. *)
          (* The rewrite is based on the following theorem:
             Let k,x,c be in [0,2^size - 1]. Assume k*c < 2^size. Let r be the
             remainer in the Euclidean division of kc by 2^size. Then:
             - If k divides (2^size-r) and r > 2^size - k*c, then by defining
               d = (2^size-r) / k, we have
               * k(x+c) congruent to k(x-d) modulo 2^size, and
               * 0 < d < c.
             - Otherwise, then there exists no d that satisfies the two
               constraints above.
          *)
          let x',c = extract_sum x in
          let two_pow_size = Z.shift_left Z.one size in
          let kc = Z.mul k c in
          let q,r = Z.ediv_rem kc two_pow_size in
          let cond1 = Z.equal q Z.zero in
          let cond2 = Z.equal Z.zero @@ Z.erem (Z.sub two_pow_size r) k in
          let cond3 = Z.gt r (Z.sub two_pow_size kc) in
          if cond1 && cond2 && cond3 then
            let d = Z.divexact (Z.sub two_pow_size r) k in
            let constr_d = Constraints.Build.Binary.biconst ~size d in
            let domain = Domain.Binary_Forward.biconst ~size d domain constr_d in
            let interm,domain = No_Rewrite.bisub ~size ~nsw ~nuw domain x' constr_d in
            let constr_k = Constraints.Build.Binary.biconst ~size k in
            let domain = Domain.Binary_Forward.biconst ~size k domain constr_k in
            No_Rewrite.bimul ~size ~nsw ~nuw domain constr_k interm
          else
            default ()

      (* XXX: need to check for signed overflow, otherwise unsound *)
      and bisdiv ~size domain a b =
      match b with
      | Constraints.(Binary{term=T0{tag=TC.Biconst(_size,k)}}) when not @@ Z.equal k Z.zero ->
          let ka,va = extract a in
          if Z.equal Z.zero (Z.rem ka k) then
            bitimes ~size ~nsw:false ~nuw:false domain (Z.divexact ka k) va
          else
            No_Rewrite.bisdiv ~size domain a b
      | _ -> No_Rewrite.bisdiv ~size domain a b

      and biadd ~size ~nsw ~nuw domain a b =
        match a,b with
        | Constraints.(Binary{term=T0{tag=TC.Biconst(_size,k)}}), _
              when Z.equal k Z.zero ->
            b, domain
        | _, Constraints.(Binary{term=T0{tag=TC.Biconst(_size,k)}})
              when Z.equal k Z.zero ->
            a, domain
        | Constraints.(Binary{term=T2{tag=TC.Bisub _size';
                                      a=Binary _ as x;
                                      b=Binary{term=T0{tag=TC.Biconst(_size1,k1)}}}},
                       Binary{term=T0{tag=TC.Biconst (_size2, k2)}})
              when Z.equal k1 k2 ->
            (* (x-k)+k -> x *)
            x, domain
        | Constraints.(Binary{term=T0{tag=TC.Biconst (_size2, k2)}},
                       Binary{term=T2{tag=TC.Bisub _size';
                                      a=Binary _ as x;
                                      b=Binary{term=T0{tag=TC.Biconst(_size1,k1)}}}})
              when Z.equal k1 k2 ->
            (* k+(x-k) -> x *)
            x, domain
        | _ ->
            let (ka,va) = extract a and (kb,vb) = extract b in
            if Constraints.equal va vb
            then bitimes ~size ~nsw ~nuw domain (Z.add ka kb) va
            else No_Rewrite.biadd ~size ~nsw ~nuw domain a b

      and bisub ~size ~nsw ~nuw domain a b =
        match a,b with
        | _, Constraints.(Binary{term=T0{tag=TC.Biconst(_size,k)}})
              when Z.equal k Z.zero ->
            a, domain
        | _ ->
            let (ka,va) = extract a and (kb,vb) = extract b in
            if Constraints.equal va vb
            (* k_a*x - k_b*x -> (k_a - k_b)*x *)
            then bitimes ~size ~nsw ~nuw domain (Z.sub ka kb) va
            else if Constraints.equal va b
            (* k_a*x - x -> (k_a - 1)*x *)
            then bitimes ~size ~nsw ~nuw domain (Z.pred ka) va
            else No_Rewrite.bisub ~size ~nsw ~nuw domain a b
      ;;

      let bshl ~size ~nsw ~nuw domain a b =
        match b with
        | Constraints.(Binary{term=T0{tag=TC.Biconst(_size,k)}}) ->
          let two_power_k = Z.(lsl) Z.one @@ Z.to_int k in
          bitimes ~size ~nsw ~nuw domain two_power_k a
        | _ -> No_Rewrite.bshl ~size ~nsw ~nuw domain a b

      let band ~size domain a b =
        if Constraints.equal a b then
          (* a & a *)
          a, domain
        else No_Rewrite.band ~size domain a b

    end

    module R = Rewrite
    
    let biadd ~size ~nsw ~nuw ctx = ar2_binary_binary_binary' ~size @@ R.biadd ~size ~nsw ~nuw
    let bisub ~size ~nsw ~nuw ctx = ar2_binary_binary_binary' ~size @@ R.bisub ~size ~nsw ~nuw
    let bimul ~size ~nsw ~nuw ctx = ar2_binary_binary_binary' ~size @@ R.bimul ~size ~nsw ~nuw
    let bxor ~size ctx = ar2_binary_binary_binary' ~size @@ R.bxor ~size
    let band ~size ctx = ar2_binary_binary_binary' ~size @@ R.band ~size
    let bor ~size ctx = ar2_binary_binary_binary' ~size @@ R.bor ~size
    let bashr ~size ctx = ar2_binary_binary_binary' ~size @@ R.bashr ~size
    let blshr ~size ctx = ar2_binary_binary_binary' ~size @@ R.blshr ~size
    let bshl ~size ~nsw ~nuw ctx = ar2_binary_binary_binary' ~size @@ R.bshl ~size ~nsw ~nuw

    let bisdiv ~size ctx = ar2_binary_binary_binary' ~size @@ R.bisdiv ~size
    let biudiv ~size ctx = ar2_binary_binary_binary' ~size @@ R.biudiv ~size
    let bismod ~size ctx = ar2_binary_binary_binary' ~size @@ R.bismod ~size
    let biumod ~size ctx = ar2_binary_binary_binary' ~size @@ R.biumod ~size

    let beq ~size ctx = ar2_binary_binary_boolean ~size (Constraints.Build.Binary.beq ~size) (Domain.Binary_Forward.beq ~size)

    (* Quite ugly: we use ar2' just to compare the constraints. This optimises equality. *)
    let beq ~size ctx a b =
      let exception Opt of bool in
      try begin
        ignore @@ ar2' (fun domain a b -> raise (Opt(Constraints.equal a b))) a b;
        assert false
      end
      with Opt res ->
        if res
        then Boolean_Forward.true_ ctx
        else beq ~size ctx a b
    ;;

    let biule ~size ctx = ar2_binary_binary_boolean ~size (Constraints.Build.Binary.biule ~size) (Domain.Binary_Forward.biule ~size)
    let bisle ~size ctx = ar2_binary_binary_boolean ~size (Constraints.Build.Binary.bisle ~size) (Domain.Binary_Forward.bisle ~size)                

    let bsext ~size ~oldsize ctx = ar1_binary_binary ~size (Constraints.Build.Binary.bsext ~size ~oldsize) (Domain.Binary_Forward.bsext ~size ~oldsize)      
    let buext ~size ~oldsize ctx = ar1_binary_binary ~size (Constraints.Build.Binary.buext ~size ~oldsize) (Domain.Binary_Forward.buext ~size ~oldsize)
    let bchoose ~size cond ctx = ar1_binary_binary ~size (Constraints.Build.Binary.bchoose ~size cond) (Domain.Binary_Forward.bchoose ~size cond)
    let bofbool ~size ctx = ar1_boolean_binary ~size (Constraints.Build.Binary.bofbool ~size) (Domain.Binary_Forward.bofbool ~size)                                 
    let bconcat ~size1 ~size2 ctx = ar2_binary_binary_binary ~size:(size1 + size2) (Constraints.Build.Binary.bconcat ~size1 ~size2) (Domain.Binary_Forward.bconcat ~size1 ~size2)
    let bextract ~size ~index ~oldsize ctx = ar1_binary_binary ~size (Constraints.Build.Binary.bextract ~size ~index ~oldsize) (Domain.Binary_Forward.bextract ~size ~index ~oldsize)
    let biconst ~size k ctx = ar0_binary (Constraints.Build.Binary.biconst ~size k) (Domain.Binary_Forward.biconst ~size k)
    let buninit ~size _ = assert false
    let bshift ~size ~offset ~max _ = assert false
    let bindex ~size _ = assert false
    let valid ~size _ = assert false
    let valid_ptr_arith ~size _ = assert false
      
  end
  module Memory_Forward = Assert_False_Transfer_Functions.Memory.Memory_Forward
  
  (* Note: we do not create them every time *)
  let integer_empty = let res = ar0_integer Constraints.Build.Integer.empty Domain.integer_empty in fun ctx -> res
  let boolean_empty = let res = ar0_boolean Constraints.Build.Boolean.empty Domain.boolean_empty in fun ctx -> res

  (* MAYBE: Avoid re-creation in most common cases *)
  let binary_empty ~size ctx =
    let res = ar0_binary (Constraints.Build.Binary.empty ~size) (Domain.binary_empty ~size) in
    res
  ;;

  let boolean_unknown ~level ctx = ar0_boolean (Constraints.Build.Boolean.unknown ~level) (Domain.boolean_unknown)  
  let binary_unknown ~size ~level ctx = ar0_binary (Constraints.Build.Binary.unknown ~level ~size) (Domain.binary_unknown ~size)
  let integer_unknown ~level ctx = ar0_integer (Constraints.Build.Integer.unknown ~level) (Domain.integer_unknown);;
  ;;

  
  
  (**************** Pretty printing ****************)

  let memory_pretty ctx fmt = assert false

  module type Pretty_Constraints = sig
    val boolean_pretty: Context.t -> Format.formatter -> boolean -> unit
    val integer_pretty: Context.t -> Format.formatter -> integer -> unit
    val binary_pretty: size:int -> Context.t -> Format.formatter -> binary -> unit      
  end
  
  module Pretty_Both:Pretty_Constraints = struct

    let boolean_pretty ctx fmt = function
      | Bottom -> Format.fprintf fmt "<bottom>"
      | NonBottom x ->
        (* Format.fprintf fmt "value %a domain %a"
         *   (Domain.boolean_pretty x.domain) x.constrain
         *   Domain.pretty x.domain *)
        Format.fprintf fmt "%a (value %a, domain %a)"
          Constraints.pretty x.constrain
          (Domain.boolean_pretty x.domain) x.constrain
          Domain.pretty x.domain        
    ;;

    let integer_pretty ctx fmt = function
      | Bottom -> Format.fprintf fmt "<bottom>"
      | NonBottom x ->
        Format.fprintf fmt "%a (value %a, domain %a)"
          Constraints.pretty x.constrain
          (Domain.integer_pretty x.domain) x.constrain
          Domain.pretty x.domain
          (* Format.fprintf fmt "value %a domain %a"
           *   (Domain.integer_pretty x.domain) x.constrain
           *   Domain.pretty x.domain *)
    ;;

    let binary_pretty ~size ctx fmt = function
      | Bottom -> Format.fprintf fmt "<bottom>"
      | NonBottom x ->
        Format.fprintf fmt "%a (value %a, domain %a)"
          Constraints.pretty x.constrain
          (Domain.binary_pretty ~size x.domain) x.constrain
          Domain.pretty x.domain
          (* Format.fprintf fmt "value %a domain %a"
           *   (Domain.integer_pretty x.domain) x.constrain
           *   Domain.pretty x.domain *)
    ;;
    
  end

  module Pretty_Value:Pretty_Constraints = struct

    let boolean_pretty ctx fmt = function
      | Bottom -> Format.fprintf fmt "<bottom>"
      | NonBottom x ->
        (* Format.fprintf fmt "value %a domain %a"
         *   (Domain.boolean_pretty x.domain) x.constrain
         *   Domain.pretty x.domain *)
        Domain.boolean_pretty x.domain fmt x.constrain
    ;;

    let integer_pretty ctx fmt = function
      | Bottom -> Format.fprintf fmt "<bottom>"
      | NonBottom x ->
        Domain.integer_pretty x.domain fmt x.constrain      
        (* Format.fprintf fmt "value %a domain %a"
         *   (Domain.integer_pretty x.domain) x.constrain
         *   Domain.pretty x.domain *)
    ;;

    let binary_pretty ~size ctx fmt = function
      | Bottom -> Format.fprintf fmt "<bottom>"
      | NonBottom x ->
        Domain.binary_pretty ~size x.domain fmt x.constrain      
        (* Format.fprintf fmt "value %a domain %a"
         *   (Domain.integer_pretty x.domain) x.constrain
         *   Domain.pretty x.domain *)
    ;;
    
  end

  module Pretty_Symbolic:Pretty_Constraints = struct
    let boolean_pretty ctx = Identifier.pretty
    let integer_pretty ctx = Identifier.pretty
    let binary_pretty ~size ctx = Identifier.pretty
  end

  let boolean_pretty,integer_pretty,binary_pretty =
    let (module X : Pretty_Constraints) = (match option_pretty_constraints with
      | `Value -> (module Pretty_Value)
      | `Both -> (module Pretty_Both)
      | `Symbolic -> (module Pretty_Symbolic))
    in X.boolean_pretty,X.integer_pretty,X.binary_pretty
  ;;

  (**************** Tuple construction and fixpoint/nondet. ****************)

  type 'a mapping =
    | EmptyMapping: unit mapping
    | ConsSame: 'a Constraints.t * 'b mapping -> ('a identifier * 'b) mapping
    | ConsDifferent: 'a Constraints.t * 'a Constraints.t * 'b mapping -> ('a identifier * 'b) mapping
  ;;

  (* MAYBE: implement a Bottom tuple? or a bottom case in a_dom and b_dom? *)
  type 'a in_tuple =
    {  a_dom: Domain.t;
       b_dom: Domain.t;
       mapping: 'a mapping;
    }

  type 'a in_acc = bool * 'a in_tuple

  (* We reconstruct the identifiers on-demand. MAYBE: have the bottom case. *)
  type 'a out_tuple = {
    index: int;
    domain: Domain.t;
    mapping: 'a mapping;
    constraints: Constraints.any Immutable_array.t
  }
  
  type ('a,'b) result =
      Result: bool * 'some in_tuple * ('some out_tuple -> 'a * 'b out_tuple) -> ('a,'b) result
        
  type empty_tuple = unit
  let empty_tuple = { mapping = EmptyMapping; a_dom = Domain.top; b_dom = Domain.top }

  let serialize (type a)
      (type_check:Constraints.any -> a Constraints.t)
      ctx (a:a identifier) (b:a identifier) (included, acc) =
    match a,b with
    | Bottom, _ | _, Bottom -> assert false (* TODO. *)
    | NonBottom a, NonBottom b ->
      let a_dom = Domain.inter acc.a_dom a.domain in
      let b_dom = Domain.inter acc.b_dom b.domain in
      let mapping =
        if Constraints.equal a.constrain b.constrain
        then ConsSame(a.constrain,acc.mapping)
        else ConsDifferent(a.constrain,b.constrain,acc.mapping)
      in
      let deserialize {index;domain;mapping;constraints} =
        let constrain,mapping,index =
          match mapping with
          | ConsSame(x,mapping) ->
            (* let _ = type_check @@ Immutable_array.get constraints index  in *)
            x,mapping,index
          | ConsDifferent(_,_,mapping) ->
            let index = index - 1 in
            (* Cannot avoid this type check due to the use of an array. *)
            let constrain = type_check @@ Immutable_array.get constraints index in
            constrain,mapping,index
        in
        let res = NonBottom{domain;constrain} in
        res,{index;domain;mapping;constraints}
      in
      Result(included, { a_dom; b_dom; mapping}, deserialize)
  ;;


  let serialize_memory _ = assert false

  let serialize_binary ~size ctx a b acc =
    serialize (function (Constraints.(Any(Binary {size=s} as x))) when s == size -> x | _ -> assert false)
      ctx a b acc
  let serialize_integer ctx a b acc =
    serialize (function (Constraints.(Any(Integer _ as x))) -> x | _ -> assert false)
      ctx a b acc
  let serialize_boolean ctx a b acc =
    serialize (function (Constraints.(Any(Bool _ as x))) -> x | _ -> assert false)
      ctx a b acc

  (**************** Nondet and union. ****************)

  let typed_nondet2 ctx (in_tup: _ in_tuple) =

    let filtereda,filteredb,total =
      let rec loop: type a. _ -> _ -> _ -> a mapping -> _ = fun la lb count -> function
        | EmptyMapping -> la,lb,count
        | ConsSame(_,mapping) -> loop la lb count mapping
        | ConsDifferent(a,b,mapping) -> loop ((Constraints.Any a)::la) ((Constraints.Any b)::lb) (count + 1) mapping
      in loop [] [] 0 in_tup.mapping
    in

    let tupa = Immutable_array.of_list filtereda in
    let tupb = Immutable_array.of_list filteredb in      

    let doma = in_tup.a_dom in
    let domb = in_tup.b_dom in

    let conda_bool = Domain.to_constraint doma in
    let condb_bool = Domain.to_constraint domb in

    let tupres =
      Constraints.Build.Tuple.nondet ~level:ctx.level
        ~conda_bool ~a:tupa
        ~condb_bool ~b:tupb
    in
    let domain =  Domain.nondet ~doma ~tupa ~domb ~tupb ~tupres in

    { mapping = in_tup.mapping; domain; index = total; constraints = tupres}
  ;;

  let union cond ctx (in_tup: _ in_tuple) =

    let filtereda,filteredb,total =
      let rec loop: type a. _ -> _ -> _ -> a mapping -> _ = fun la lb count -> function
        | EmptyMapping -> la,lb,count
        | ConsSame(_,mapping) -> loop la lb count mapping
        | ConsDifferent(a,b,mapping) -> loop ((Constraints.Any a)::la) ((Constraints.Any b)::lb) (count + 1) mapping
      in loop [] [] 0 in_tup.mapping
    in

    let tupa = Immutable_array.of_list filtereda in
    let tupb = Immutable_array.of_list filteredb in
    let tupres = Immutable_array.map2 (fun (Constraints.Any x1) (Constraints.Any x2) ->
       match (x1,x2) with
       | Constraints.Binary{size=s1}, Constraints.Binary{size=s2} ->
          assert(s1 == s2);
          Constraints.Any(Constraints.Build.Binary.bunion ~size:s1 cond x1 x2)
       | _ -> assert false ) tupa tupb in
    let doma = in_tup.a_dom in
    let domb = in_tup.b_dom in
    let domres = Domain.inter doma domb in

    (* We reuse nondet for now. *)
    let domain =  Domain.nondet ~doma:domres ~tupa ~domb:domres ~tupb ~tupres in

    { mapping = in_tup.mapping; domain; index = total; constraints = tupres}
  ;;

  (**************** Fixpoint computation. ****************)
  

  let typed_fixpoint_step ctx (included, in_tup) =

    let cur_level = ctx.mu_ctx.level in

    let arg_dom = in_tup.a_dom in
    let final_dom = in_tup.b_dom in

    (* Codex_log.feedback "argument level = %d,@ context level = %d" (Constraints.level @@ Domain.to_constraint arg_dom) cur_level; *)
    assert(Constraints.level @@ Domain.to_constraint arg_dom < cur_level);    

    let fixpoint_reached = ref included in
    
    let actuals,old_args,finals,total,arg_dom =
      let rec loop: type a. _ -> _ -> _ -> _ -> _ -> a mapping -> _ = fun actuals old_args finals count arg_dom -> function
        | EmptyMapping -> actuals,old_args,finals,count,arg_dom
        | ConsSame(x,mapping) ->
          assert(Constraints.level x < cur_level);
          loop actuals old_args finals count arg_dom mapping
        | ConsDifferent(input,final,mapping) ->
          assert(Constraints.level input <= cur_level);
          assert(Constraints.level final <= cur_level);            
          let arg_dom,old_arg,actual =
            input |> fun (type a) (input:a Constraints.t) -> match input with

            (* If already a variable. *)
            | Constraints.Bool{term = Constraints.Mu_formal {level;actual=(actual_constrain,_)}}
              when level == cur_level -> 
              arg_dom,Constraints.Any input,Constraints.Any actual_constrain
            | Constraints.Integer{term = Constraints.Mu_formal {level;actual=(actual_constrain,_)}}
              when level == cur_level ->
              arg_dom,Constraints.Any input,Constraints.Any actual_constrain
            | Constraints.Binary{term = Constraints.Mu_formal {level;actual=(actual_constrain,_)}}
              when level == cur_level ->
              arg_dom,Constraints.Any input,Constraints.Any actual_constrain

  
            (* Normal introduction of a variable. *)
            | _ when Constraints.level input < cur_level -> begin
                fixpoint_reached := false;
                arg_dom,Constraints.Any input, Constraints.Any input
              end
            (* In some rare cases (e.g. reads of different sizes may
               happen at the same memory location), the memory
               domain makes small modifications to an argument
               already introduced. We make some simple substitutions
               to support this. *)              
            | _ when Constraints.level input == cur_level ->
              fixpoint_reached := false;
              (* Note that this is not just substitution, but also
                 an evaluation of the function in the domain. *)
              let rec subst: type a. a Constraints.t -> a identifier =
                let open Constraints in function
                  | Integer{term=T2{tag=TC.Imod;a;b=Integer{term=T0{tag=TC.Iconst k}}}} ->
                    Integer_Forward.imod () (subst a) (Integer_Forward.iconst k ())
                  | Integer{term=T2{tag=TC.Ishr;a;b=Integer{term=T0{tag=TC.Iconst k}}}} ->
                    Integer_Forward.ishr () (subst a) (Integer_Forward.iconst k ())
                  | Integer{term=Mu_formal{actual=(v,_)}} -> NonBottom{domain=arg_dom;constrain=v}
                  | Binary{term=T1{tag=TC.Bextract{size;index;oldsize};a}} ->
                    Binary_Forward.bextract () ~size ~index ~oldsize (subst a)
                  | Binary{term=Mu_formal{actual=(v,_)}} -> NonBottom{domain=arg_dom;constrain=v}
                  | _ -> assert false
              in
              begin match subst input with
                | Bottom -> assert false (* Should not happen. *)
                | NonBottom{constrain=substed;domain} ->
                   assert(Constraints.level @@ Domain.to_constraint arg_dom < cur_level);
                   assert(Constraints.level @@ Domain.to_constraint domain < cur_level);                       
                   (*assert(domain == arg_dom);*) (* Not true when `subst input` becomes a constant *)
                  let substed = Constraints.Any substed in
                   
                  (* Save the result of evaluation in arg_dom. *)
                  let arg_dom = Domain.inter arg_dom domain in
                  arg_dom,substed, substed
              end
            | _ ->
              assert(Constraints.level input > cur_level);
              assert false (* This case should not happen. *)
          in loop (actual::actuals) (old_arg::old_args) 
            ((Constraints.Any final)::finals) (count + 1) arg_dom mapping
      in loop [] [] [] 0 arg_dom in_tup.mapping
    in

    let actuals = Immutable_array.of_list actuals in
    let old_args = Immutable_array.of_list old_args in
    let finals = Immutable_array.of_list finals in            

    let cond_arg = Domain.to_constraint arg_dom in    

    (* It is important that the domain corresponding to the argument
       is suitable for use outside of the mu, when we leave the mu. *)
    assert(Constraints.level cond_arg < cur_level);    

    let bool,domainf = Domain.fixpoint_step ~lvl:ctx.mu_ctx.level ~actuals arg_dom ~args:old_args final_dom ~finals in
    let res = !fixpoint_reached && bool in

    let continuef ~close =
      let constraints = 
        if close then begin
          let body_cond = Domain.to_constraint final_dom in
          Constraints.Build.Tuple.mu
            ~level:ctx.mu_parent_ctx.level
            ~init:actuals ~var:old_args ~body:finals ~body_cond
          end
        else
          (* Restart *)
          let new_args = actuals |> Immutable_array.map (fun actual ->
              let intro constrain x =
                if option_fresh_variable_every_time
                || Constraints.level constrain != ctx.mu_ctx.level (* Is not already a variable. *)
                then
                  (* TODO: Remove the actual_cond argument, that is not needed in this implementation. *)
                  Constraints.Build.Mu_Formal.intro
                    ~level:ctx.mu_ctx.level ~actual:constrain ~actual_cond:Constraints.Build.Boolean.true_ x
                else constrain
              in
              match actual with
              | Constraints.(Any (Bool _ as c)) -> Constraints.Any (intro c TC.Boolean)
              | Constraints.(Any (Integer _ as c)) -> Constraints.Any (intro c TC.Integer)
              | Constraints.(Any (Binary {size} as c)) -> Constraints.Any (intro c @@ TC.Binary size))
          in
          new_args
      in
      let domain = domainf ~close constraints in
      assert(Constraints.level @@ Domain.to_constraint domain < cur_level);
      { mapping = in_tup.mapping; domain; index = total; constraints}

    in
    res,continuef

  let mu_context_upcast ctx = ctx.mu_ctx
  let mu_context_open parent_ctx =
    Domain.fixpoint_open();
    let rec mu_ctx = { mu_ctx = ctx; mu_parent_ctx = parent_ctx;  }
    and ctx = { ctx = Mu_context mu_ctx; level = parent_ctx.level + 1 }
    in mu_ctx


  (**************** Queries ****************)

  module Query = struct
    include Domain.Query
    let boolean ctx  = function
      | Bottom -> Quadrivalent_Lattice.Bottom
      | NonBottom x -> Domain.Query.boolean x.domain x.constrain
    ;;
    let binary ~size ctx = function
      | Bottom -> Domain.Query.Binary_Lattice.bottom ~size
      | NonBottom x -> Domain.Query.binary ~size x.domain x.constrain
    let integer ctx = function
      | Bottom -> Domain.Query.Integer_Lattice.bottom
      | NonBottom x -> Domain.Query.integer x.domain x.constrain
                         
    let reachable _ = assert false
  end

  let assume_memory = assume
  let assume_binary ~size = assume
  let assume_boolean = assume
  let assume_integer = assume

  
  let memory_is_empty _ = assert false
  let binary_is_empty ~size _ = assert false
  let integer_is_empty _ = assert false
  let boolean_is_empty _ = assert false

  module Satisfiable = struct

    module To_SMT_FirstOrder = Constraints_smt.MakeFirstOrder(Constraints);;
    module To_SMT_Horn = Constraints_smt.MakeHorn(Constraints);;

    let satisfiable ctx bool =
      match bool with
      | Bottom -> Domain_sig.Unsat
      | NonBottom{ constrain; domain } ->
        let condition = Domain.to_constraint domain in
        Smtbackend_smtlib.with_z3
          (fun (module SMT:Smtbackend_smtlib.Untyped_Muz) ->
             let translate = match option_translate with
               | `Horn -> let module Inst = To_SMT_Horn(SMT) in Inst.translate
               | `First_order -> let module Inst = To_SMT_FirstOrder(SMT) in Inst.translate
             in
             translate @@ Constraints.Build.Boolean.(&&) constrain condition)       
    ;;

    
  end

  let reachable _ _ = assert false
  let satisfiable = Satisfiable.satisfiable

  let should_focus ~size:_ _ = assert false
  let may_alias ~size:_ _ = assert false

end
