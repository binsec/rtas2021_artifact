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

module Address = struct

  type t =
    (* malloc_size. TODO: Should probably be stored in the region. *)
    | Malloc of Transfer_functions.Malloc_id.t * int 
    | Null

  let compare a b = match a, b with
    | Malloc (a,_), Malloc (b,_) -> Pervasives.compare a b
    | Null, Null -> 0
    | Malloc _, Null  -> 1
    | Null, (Malloc _ ) -> -1
  ;;

  let hash = function
    | Null -> 0
    | Malloc (id,_) -> 3 + 2 * (Transfer_functions.Malloc_id.hash id)

  let pretty fmt = function
    | Null -> Format.fprintf fmt "null"
    | Malloc (id,_) -> Format.fprintf fmt "&%s" @@ Transfer_functions.Malloc_id.to_string id
  ;;

  let equal a b = (compare a b) == 0;;

  let size_of = function
    | Null ->
      let max = Codex_config.max_valid_absolute_address() in
      if (not @@ Z.equal Z.zero max) && (not @@ Z.equal max Z.minus_one)
      then ((* Kernel.feedback "NULL size %d" (Z.to_int max); *) (Z.to_int max))
      else assert false
    | Malloc(id,size) -> size
  ;;

end

module AddressMap = struct
  include Extstdlib.Map.Make(Address)

end

module AddressSet = Set.Make(Address);;


open Memory_sig

module type Operable_Value_Region= sig
  include Operable_Value_Region
end


module type Operable_Value_Whole = sig
  include Operable_Value_Whole;;
  type offset
  val is_precise:  Scalar.Context.t -> binary -> (Address.t * offset) precision
  val new_baddr: Address.t -> Scalar.Context.t -> max:int option -> binary
end


module Make(BR:Operable_Value_Region) = struct
  
  module Operable_Value:sig
    include Operable_Value_Whole
      with module Scalar = BR.Scalar
       and type offset = BR.binary
    val fold:BR.Scalar.Context.t ->
      binary -> (AddressMap.key -> BR.binary -> 'a -> 'a) -> 'a -> 'a
  end
  =
  struct

    module Scalar = BR.Scalar

    type offset = BR.binary

    (* A map from region to offset. MAYBE: add the condition for which
       the value has this offset; this could allow to gain more
       precision at a low cost. *)
    type binary = {
      null: Scalar.binary option;

      map: BR.binary AddressMap.t;

      unknown: bool;

      (* We could remove this if we tested, for every operation, that we
         are not combining pointers together. This ensures that we warn
         when such a pointer is used. *)
      invalid: bool;
    }


    (* let equal a b =
     *   a == b ||
     *   (a.unknown == b.unknown &&
     *    a.invalid == b.invalid &&
     *    (match a.null,b.null with
     *    | None, None -> true
     *    | Some a, Some b -> Scalar.Binary.equal a b
     *    | None, Some _ | Some _, None -> false) &&
     *    (a.map == b.map || AddressMap.equal BR.equal a.map b.map))
     *    ;; *)


    (* Common pretty-printer, with or without context. *)
    let pp scalar_pretty br_pretty fmt x =
      let map_pretty fmt map =
        let first = ref true in
        Format.fprintf fmt "{{ @[<v>";
        map |> AddressMap.iter (fun addr v ->
                   (if !first then first := false else Format.pp_print_cut fmt ());
                   Format.fprintf fmt "%a%a" Address.pretty addr br_pretty v);
        Format.fprintf fmt "@] }}"
      in

      match x.null, AddressMap.is_empty x.map with
      | None, true -> Format.fprintf fmt "<bottom>"
      | Some x, true -> scalar_pretty fmt x
      | Some v, false -> Format.fprintf fmt "%a or %a"
                           scalar_pretty v
                           map_pretty x.map
      | None, false -> map_pretty fmt x.map
    ;;

                
                
    module Binary = struct
      type t = binary

      let pretty = pp Scalar.Binary.pretty BR.Binary.pretty
      let hash x = assert false
      let compare _ _ = assert false
      let equal _ _ = assert false
    end

    module Query = struct
      module Binary_Lattice = struct

        type t = {
            null: Scalar.Query.Binary_Lattice.t option;
            map: BR.Query.Binary_Lattice.t AddressMap.t;
            invalid: bool;
            unknown: bool;
          }

        let top ~size = assert false
        let singleton ~size = assert false
        let hash _ = assert false
        let equal _ _ = assert false
        let compare _ _ = assert false
        let is_included _ _ = assert false
        let inter ~size _ _ = assert false
        let includes ~size a b = is_included b a
        let includes_or_widen ~size ~previous _ = assert false
        let widen ~size ~previous _ = assert false
        let is_bottom ~size x =
          x.null == None &&
            x.invalid = false &&
              x.unknown = false &&
                AddressMap.is_empty x.map
        ;;

        let bottom ~size = {map=AddressMap.empty;unknown=false;invalid=false;null=None}

        let pretty ~size fmt x =
          if is_bottom ~size x
          then Format.fprintf fmt "{{  }}"
          else
            let smth = "" in
            let smth = if x.invalid then (Format.fprintf fmt "invalid"; " or ") else smth in
            let smth = if x.unknown then (Format.fprintf fmt "unknown"; " or ") else smth in
            let smth = (match x.null with
                        | None -> smth
                        | Some x -> Format.fprintf fmt "%s%a" smth (Scalar.Query.Binary_Lattice.pretty ~size) x; " or ")
            in
            if not @@ AddressMap.is_empty x.map then
              ( let first = ref false in
                Format.fprintf fmt "%s@[<hv 3>{{ " smth;
                x.map |> AddressMap.iter (fun addr v ->
                             (if !first then Format.fprintf fmt ";@ "
                              else first := true);          
                             if (match BR.Query.binary_is_singleton ~size v with Some x when Z.equal x Z.zero -> true | _ -> false)
                             then Format.fprintf fmt "%a@ " Address.pretty addr
                             else Format.fprintf fmt "%a + %a@ " Address.pretty addr (BR.Query.Binary_Lattice.pretty ~size) v);
                Format.fprintf fmt "@]}}")
        ;;

        let join ~size a b =
          let f a b = match a,b with
            | None, x | x, None -> x
            | Some a, Some b -> Some(Scalar.Query.Binary_Lattice.join ~size a b)
          in
          { null = f a.null b.null;
            map = AddressMap.union (fun _ a b -> Some (BR.Query.Binary_Lattice.join ~size a b)) a.map b.map;
            invalid = a.invalid || b.invalid;
            unknown = a.unknown || b.unknown
          }

      end

                            

      let binary ~size ctx x =
        let null = match x.null with
          | None -> None
          | Some off -> Some (Scalar.Query.binary ~size ctx off)
        in
        let map = x.map |> AddressMap.map (fun off ->
            BR.Query.binary ~size ctx off) in
        Binary_Lattice.{null;map;invalid=x.invalid;unknown=x.unknown}

      let binary_to_ival ~signed ~size _ = assert false
      let binary_to_known_bits  ~size x =
        let exception Top in
        let open Binary_Lattice in
        try if x.invalid || x.unknown || not (AddressMap.is_empty x.map) then raise Top
        else 
          match x.null with
          | None -> raise Top
          | Some x -> Scalar.Query.binary_to_known_bits ~size x
        with Top -> Single_value_abstraction.Known_bits.top ~size
      ;;

      let binary_fold_crop ~size bin ~inf ~sup f acc = assert false
      let binary_is_empty ~size _ = assert false                
      let binary_is_singleton ~size x =
        let open Binary_Lattice in
        if x.invalid = false && x.unknown = false && AddressMap.is_empty x.map
        then match x.null with
          | None -> None
          | Some x -> Scalar.Query.binary_is_singleton ~size x
        else None
        ;;
        
    end

    let binary_pretty ~size ctx = pp (Scalar.binary_pretty ~size ctx) (BR.binary_pretty ~size ctx)
    
    let serialize ~size ctx a b (included, acc) =
      (* Codex_log.feedback "Region_separation serialize2 %a %a"
       *   (binary_pretty ~size ctx) a (binary_pretty ~size ctx) b; *)
      let f = function
        | None -> BR.binary_empty ~size ctx
        | Some a -> a
      in
      let f' = function
        | None -> Scalar.binary_empty ~size ctx
        | Some a -> a
      in
      let Scalar.Result(included,acc,d_map) =
        AddressMap.fold_on_diff2 a.map b.map (Scalar.Result(included,acc,fun tup -> a.map,tup))
          (fun addr a b (Scalar.Result(inc,acc,d_map) as res) ->
             match a,b with
             | Some a, Some b when a == b || BR.Binary.equal a b -> res
             | None, None -> res
             | _ -> 
               let a = f a and b = f b in
               let Scalar.Result(inc,acc,d_offset) = BR.serialize ~size ctx a b
                (inc,acc) in
               Scalar.Result(inc, acc, fun tup ->
                 let offset,tup = d_offset tup in
                 let map,tup = d_map tup in
                 AddressMap.add addr offset map,tup)) in
      let Scalar.Result(included,acc,d_null) = match a.null,b.null with
        | None, None -> Scalar.Result(included, acc, (fun tup -> None,tup))
        | (Some a as res), Some b when a == b || Scalar.Binary.equal a b ->
          Scalar.Result(included, acc, (fun tup -> res,tup))
        | _ ->
          let Scalar.Result(inc,acc,d) =
            Scalar.serialize_binary ~size ctx (f' a.null) (f' b.null)
              (included,acc)
          in Scalar.Result(inc,acc,fun tup -> let res,tup = d tup in Some res,tup)
      in
      Scalar.Result(included,acc,fun tup ->
          let null,tup = d_null tup in
          let map,tup = d_map tup in
          let res = {map;null;
                     unknown = a.unknown || b.unknown;
                     invalid = a.invalid || b.invalid} in
        (* Codex_log.feedback "OV.deserialize2 @\na:%a@\nb:%a@\nres:%a" *)
          (* (binary_pretty ~size:32 ctx) a (binary_pretty ~size:32 ctx) b (binary_pretty ~size:32 ctx) res; *)
        res,tup)
    ;;

    let is_precise ctx x =
      if x.unknown then Imprecise
      else match x.null with
        | Some v when AddressMap.is_empty x.map ->
          let off = BR.zero_offset ~size:(Codex_config.ptr_size()) ~max:None ctx in
          let off = BR.bindex ~size:(Codex_config.ptr_size()) 1 ctx off v in
          Singleton(Address.Null, off)
        | Some _ -> Imprecise
        | None when AddressMap.is_empty x.map -> Empty
        | None -> match AddressMap.is_singleton x.map with None -> Imprecise | Some x -> Singleton x
    ;;

    let fold ctx x f acc =
      let acc = match x.null with
        | None -> acc
        | Some v ->
          let off = BR.zero_offset ~size:(Codex_config.ptr_size()) ~max:None ctx in
          let off = BR.bindex ~size:(Codex_config.ptr_size()) 1 ctx off v in
          f Address.Null off acc
      in
      AddressMap.fold f x.map acc

    module Binary_Forward = struct

      let bottom = {null = None; map = AddressMap.empty; unknown = false; invalid = false }
      let buninit ~size ctx = bottom
      (* Operations on pointers. *)

      let pred =
        fun pred1 pred init ctx a b ->
        begin
          let common = AddressMap.merge (fun addr a b ->
              match(a,b) with
              | None,None -> assert false
              | None, _ | _, None -> None
              | Some a, Some b -> Some (pred ctx a b)
            ) a.map b.map in
          let list = AddressMap.fold (fun _ bool acc -> bool::acc) common init in
          let list = match a.null,b.null with
            | Some a, Some b -> (pred1 ctx a b)::list
            | _, _ -> list
          in
          match list with
            | [] -> Scalar.boolean_empty ctx (* MAYBE: assume false of boolean_empty? *)
            | [x] -> x
            | a::b ->
              let nondet_boolean x y =
                let Scalar.Result(_,tup,d) =
                  Scalar.serialize_boolean ctx x y (true,Scalar.empty_tuple)
                in
                fst @@ d @@ Scalar.typed_nondet2 ctx tup
              in
              List.fold_left nondet_boolean a b
        end
      ;;

      (* Here, comparing different bases is forbidden. *)
      let biule ~size ctx a b =
        if a.invalid || b.invalid || a.unknown && b.unknown
        then (* Scalar.Boolean_Forward.unknown ctx *)assert false
        else pred (Scalar.Binary_Forward.biule ~size) (BR.ble ~size) [] ctx a b;;

      let bisle ~size ctx a b =
        (* Kernel.feedback "invalid %b %b %b %b" a.invalid b.invalid a.unknown b.unknown; *)
        if a.invalid || b.invalid || a.unknown && b.unknown
        then (* Scalar.Boolean_Forward.unknown ctx *)assert false
        else pred (Scalar.Binary_Forward.bisle ~size) (BR.ble ~size) [] ctx a b;;

      let beq ~size ctx a b =
        if a.invalid || b.invalid || a.unknown || b.unknown
        then assert false  (* Scalar.Boolean_Forward.unknown ~level ctx  *)
        else begin 
          let false_ = Scalar.Boolean_Forward.false_ ctx in
          match a.null, AddressMap.is_empty a.map, b.null, AddressMap.is_empty b.map with
          | None, true, _, _ | _, _, None, true -> Scalar.Boolean_Forward.false_ ctx (* XXX: empty *)
          | Some a, true, Some b, true -> Scalar.Binary_Forward.beq ~size ctx a b
          | None, false, None, false ->
            (match AddressMap.is_singleton a.map, AddressMap.is_singleton b.map
             with
             | Some (adda,offa), Some(addb,offb) ->
               if Address.equal adda addb
               then BR.beq ~size ctx offa offb
               else false_
            (* Else: We cannot rule false_ out, which happens when comparing different bases. *)
             | _ -> pred (Scalar.Binary_Forward.beq ~size) (BR.beq ~size) [false_] ctx a b)
          | _ -> 
            pred (Scalar.Binary_Forward.beq ~size) (BR.beq ~size) [false_] ctx a b
        end
      ;;

      (* This version represent the cases where pointer addition and
         indices is done using biadd/bisub, rather than
         bshift/bindex. *)
      (* module Untyped_pointers = struct
       * 
       * 
       * 
       *   let biadd ~size ctx (a:binary) (b:binary) =
       *     (\* Kernel.feedback "adding %a %b %a %b" 
       *        (binary_pretty ~size ctx) a a.invalid (binary_pretty ~size ctx) b b.invalid; *\)
       *     (\* We remove null from b, because we don't want to add Null->a to Null->b twice. *\)
       *     let null = match a.null,b.null with
       *       | Some a, Some b -> Some(Scalar.Binary_Forward.biadd ~size ctx a b)
       *       | _,_ -> None in
       *     let add_to null map =
       *       match null with
       *       | None -> AddressMap.empty
       *       | Some x -> map |> AddressMap.map (fun off ->
       *           BR.Binary_Forward.biadd ~size ctx off x) 
       *     in
       *     (\* Assertion is unlikely in real programs. *\)
       *     let map = AddressMap.union (fun addr v1 v2 -> assert false)
       *         (add_to a.null b.map) (add_to b.null a.map)
       *     in
       *     let invalid =
       *       a.invalid || b.invalid || not @@ AddressMap.inter_empty b.map a.map in
       *     (\* Kernel.feedback "result: invalid = %b" invalid; *\)
       *     { null; map;
       *       unknown = a.unknown || b.unknown;
       *       invalid }
       *   ;;
       * 
       *   let bisub ~size ctx a b =
       *     let sub a b = BR.Binary_Forward.bisub ~size ctx a b in
       *     let sub_from null map f =
       *       match null with
       *       | None -> AddressMap.empty
       *       | Some x -> map |> AddressMap.map (fun off -> f x off)
       *     in
       *     (\* Assertion is unlikely in real programs. *\)
       *     let map = AddressMap.union (fun addr v1 v2 -> assert false)
       *         (sub_from a.null b.map sub) (sub_from b.null a.map @@ fun a b -> sub b a)
       *     in
       *     let inter = AddressMap.merge (fun addr a b -> match a,b with
       *         | Some a, Some b -> Some(sub a b)
       *         | _,_ -> None) a.map b.map
       *     in
       *     let null =
       *       let list = AddressMap.fold (fun _addr x acc -> x::acc) inter [] in
       *       let list = match a.null, b.null  with
       *         | Some a, Some b -> (Scalar.Binary_Forward.bisub ~size ctx a b)::list
       *         | _,_ -> list in
       *       match list with
       *       | [] -> None
       *       | [x] -> Some x
       *       | list -> assert false
       *     in
       *     {null;map;unknown=a.unknown||b.unknown;invalid=a.invalid||b.invalid}
       *   ;;
       * end *)


      (* Supports both integer and pointer substraction. Always returns an integer. *)
      let bisub ~size ~nsw ~nuw ctx a b =
        let inter = AddressMap.merge (fun addr a b -> match a,b with
            | Some a, Some b -> Some(BR.bisub ~size ctx a b)
            | _,_ -> None) a.map b.map
        in
        let l = AddressMap.fold (fun _ x acc -> x::acc) inter [] in
        let l = match a.null,b.null with
          | Some a, Some b -> (Scalar.Binary_Forward.bisub ~size ~nsw ~nuw ctx a b)::l
          | _ -> l
        in
        match l with
          | [] -> bottom
          | [x] -> {null=Some x;map=AddressMap.empty;unknown=false;invalid=false}
          | a::b -> assert false               (* nondet *)
      ;;

      
      
      let ptrsize = Codex_config.ptr_size()

      let valid ~size _acc_type ctx (ptr:binary) =

        (* For each region, we check two things: that the pointer is
           within the bounds of the region, but also that each
           internal index of an array in the region is within bounds.

           (At least currently) the latter does imply the former, as
           it is possible to perform dummy pointer shifts (e.g. using
           cast to a structure that is too big) that are checked only
           here.  *)
        (* min <= off < max *)
        let between_min_max min off max =
          let zero = BR.zero_offset ~size:ptrsize ctx ~max:None in
          assert (Z.equal min Z.zero);
          Scalar.Boolean_Forward.(&&) ctx 
            (BR.ble ~size:ptrsize ctx zero off) @@
          (BR.ble ~size:ptrsize ctx off
             (BR.bindex ~size:ptrsize 1 ctx zero @@ Scalar.Binary_Forward.biconst ~size:ptrsize max ctx))
        in

        AddressMap.fold (fun key off acc ->
            match key with
            | Address.Null -> assert false
            | Address.Malloc (id,malloc_size) ->
              let max = Z.of_int (malloc_size - (size/8)) in
              (* Codex_log.feedback "valid var %s %s %d %d" (Term_constructors.Malloc_id.to_string id) (Z.to_string max) malloc_size size; *)
              Scalar.Boolean_Forward.(&&) ctx acc @@
              Scalar.Boolean_Forward.(&&) ctx 
                (between_min_max Z.zero off max)
                (BR.within_bounds ~size ctx off)
                
              (* | Address.Unknown -> assert false *)
          ) ptr.map @@ Scalar.Boolean_Forward.true_ ctx
      ;;

      let valid_ptr_arith ~size:_ ctx _x _y = Scalar.Boolean_Forward.true_ ctx

      (* let valid ~size ctx ptr =
       *   let t = valid ~size ctx ptr in
       *   Codex_log.feedback "Valid: %a" (Scalar.boolean_pretty ctx) t;
       *   t
       * ;; *)
      
      (* let valid ~size ctx ptr = Scalar.Boolean_Forward.true_ ctx;; (\* XXX *\) *)


      let bshift ~size ~offset ~max ctx x = {
        null = (match x.null with
            | None -> None
            | Some x -> Some(
                let k = Scalar.Binary_Forward.biconst ~size (Z.of_int offset) ctx in
                Scalar.Binary_Forward.biadd ~size ~nsw:false ~nuw:true ctx x k));
        map = AddressMap.map (BR.bshift ~size ~offset ~max ctx) x.map;
        unknown = x.unknown;
        invalid = x.invalid;
      }


      let bindex ~size k ctx x e =
        let invalid = x.invalid || not (AddressMap.is_empty x.map) in
        match e.null with
          | None -> { invalid; unknown = false; map = AddressMap.empty; null = None }
          | Some e ->
            {
              null = (match x.null with
                  | None -> None
                  | Some x -> Some(
                      let k = Scalar.Binary_Forward.biconst ~size (Z.of_int k) ctx in
                      let off =Scalar.Binary_Forward.bimul ~size ~nsw:true ~nuw:true ctx k e in
                      Scalar.Binary_Forward.biadd ~size ~nsw:true ~nuw:true ctx x off));
              map = AddressMap.map (fun x -> BR.bindex ~size k ctx x e) x.map;
              unknown = x.unknown;
              invalid = x.invalid;
            }

      (**************** Arithmetic operations. ****************)

      let biconst ~size k ctx =
        { null = Some(Scalar.Binary_Forward.biconst ~size k ctx);
          map = AddressMap.empty;
          unknown = false; invalid = false }

      let bofbool ~size ctx x =
        { null = Some(Scalar.Binary_Forward.bofbool ~size ctx x);
          map = AddressMap.empty;
          unknown = false; invalid = false }
      ;;

      let bchoose ~size cond ctx x =
        { null = Extstdlib.Option.map (Scalar.Binary_Forward.bchoose ~size cond ctx) x.null;
          map = AddressMap.map (BR.bchoose ~size cond ctx) x.map;
          unknown = x.unknown; invalid = x.invalid }

      (* Common to arithmetical unary operations. *)
      let ar1 op1 ~size ctx x =
        let invalid = x.invalid || not @@ AddressMap.is_empty x.map in
        let null = match x.null with None -> None | Some x -> Some(op1 ~size ctx x) in
        {null;map=AddressMap.empty;invalid;unknown=x.unknown}
      ;;

      let buext ~size ~oldsize ctx x =
        if size == oldsize then x
        else ar1 (Scalar.Binary_Forward.buext ~oldsize) ~size ctx x;;

      let bsext ~size ~oldsize ctx x =
        if size == oldsize then x
        else ar1 (Scalar.Binary_Forward.bsext ~oldsize) ~size ctx x;;

      let bextract ~size ~index ~oldsize ctx x =
        (* Codex_log.feedback "region_separation.bextract"; *)
        if size == oldsize then x
        else ar1 (Scalar.Binary_Forward.bextract ~index ~oldsize) ~size ctx x
      ;;

      let ar2 op1 ~size ctx a b =
        let invalid = a.invalid || b.invalid || (not @@ AddressMap.is_empty a.map) || (not @@ AddressMap.is_empty b.map) in
        let unknown = a.unknown || b.unknown in
        let null = match a.null, b.null with
          | Some a, Some b -> Some(op1 ~size ctx a b)
          | _ -> None
        in
        if invalid && not (a.invalid || b.invalid)
        then Codex_log.warning "invalid operation";
        {null;map=AddressMap.empty;invalid;unknown}
      ;;


      let biadd ~size ~nsw ~nuw ctx a b = ar2 (Scalar.Binary_Forward.biadd ~nsw ~nuw) ~size ctx a b
      let bimul ~size ~nsw ~nuw ctx a b = ar2 (Scalar.Binary_Forward.bimul ~nsw ~nuw) ~size ctx a b
      let bxor ~size ctx a b = ar2 (Scalar.Binary_Forward.bxor) ~size ctx a b
      let band ~size ctx a b = ar2 (Scalar.Binary_Forward.band) ~size ctx a b
      let bor ~size ctx a b = ar2 (Scalar.Binary_Forward.bor) ~size ctx a b

      let bashr ~size ctx a b = ar2 (Scalar.Binary_Forward.bashr) ~size ctx a b
      let blshr ~size ctx a b = ar2 (Scalar.Binary_Forward.blshr) ~size ctx a b
      let bshl ~size ~nsw ~nuw ctx a b = ar2 (Scalar.Binary_Forward.bshl ~nsw ~nuw)   ~size ctx a b                    

      let bisdiv ~size ctx a b = ar2 (Scalar.Binary_Forward.bisdiv) ~size ctx a b
      let bismod ~size ctx a b = ar2 (Scalar.Binary_Forward.bismod) ~size ctx a b        
      let biudiv ~size ctx a b = ar2 (Scalar.Binary_Forward.biudiv) ~size ctx a b
      let biumod ~size ctx a b = ar2 (Scalar.Binary_Forward.biumod) ~size ctx a b        

      let bconcat ~size1 ~size2 ctx a b =    
        let invalid = a.invalid || b.invalid || (not @@ AddressMap.is_empty a.map) || (not @@ AddressMap.is_empty b.map) in
        let unknown = a.unknown || b.unknown in
        let null = match a.null, b.null with
          | Some a, Some b -> Some(Scalar.Binary_Forward.bconcat ~size1 ~size2 ctx a b)
          | _ -> None
        in {null;map=AddressMap.empty;invalid;unknown}
      ;;
    end

    let assume ~size  ctx cond x =
      let f v = BR.assume ~size ctx cond v in
      { x with
        null = (match x.null with
                | None -> None
                | Some v -> Some (Scalar.assume_binary ~size ctx cond v));
        map = x.map |> AddressMap.map f }
    ;;

    let binary_empty ~size ctx = Binary_Forward.bottom

    (* An unknown (i.e. outside-constructed) value may either be a
         constant, or point to the unknown part of the memory. *)
    (* XXX: The offset for unknown is essentially useless, and we create a lot of them. 
         Maybe it is important to optimize unknown, i.e. add a special "Unknown" variable. *)
    (* XXX: I should generate assumptions based on the type:
         if an integer, state that it must be in some interval, and the base is null;
         if a pointer, keep it that way. *)
    let binary_unknown ~size ~level ctx =
      (* AddressMap.add Address.Unknown (Scalar.Binary_Forward.bunknown ~size ctx) @@ *)
      { null = Some(Scalar.binary_unknown ~size ~level ctx);
        map = AddressMap.empty;
        (* XXX: si je met true, tous les entiers generes deviennent des valeurs inconnues...
             Il faudrait au moins separer bunknown_int et bunknown_ptr. *)
        unknown = false;
        invalid = false }


    
    let new_baddr addr ctx ~max =
      { null = None;
        map = AddressMap.singleton addr @@
          BR.zero_offset ~size:(Codex_config.ptr_size()) ~max ctx;
        invalid = false; unknown = false }

  end

  module Memory(Region:Memory_sig.Memory_Region
                with module Scalar = BR.Scalar
                 and type address = BR.binary
                 and module Value.Scalar = BR.Scalar
               ):Memory_sig.Whole_Memory
    with module Scalar = BR.Scalar
     and module Value = Region.Value
     and type address = Operable_Value.binary
  = struct

    module Scalar = BR.Scalar
    module Value = Region.Value
    type address = Operable_Value.binary

    type region = Region.memory
    type memory = {
      (* TODO: for recency: a pair of recent,older regions. *)
      known: region AddressMap.t; 
      escaped: unit (* set of bases that escaped. *);
      (* TODO: as we have regions here, I can associate to each region
         the addresses that may have been written in this region. And
         implement efficiently load(store(x)=x) *)
    }

    let pretty ctx fmt {known} = AddressMap.mk_pretty Address.pretty (Region.pretty ctx) fmt known

    let memory_empty ctx = {
      known = AddressMap.empty;
      escaped = ()  
    }

    let serialize_known ctx a b (included, acc) =
      let f addr = function
        | Some x -> x
        | None -> Region.initial ctx (Address.size_of addr)
      in
      (* Note: we do not reconstruct in the order in which we iterate,
         which is perfectly OK. *)
      AddressMap.fold_on_diff2 a b (Scalar.Result(included,acc,fun tup -> a,tup))
        (fun addr a b (Scalar.Result(inc,acc,d_map)) ->
           let Scalar.Result(inc,acc,d_region) = Region.serialize ctx (f addr a) (f addr b)
            (inc,acc) in
           Scalar.Result(inc,acc,fun tup ->
               let region,tup = d_region tup in
               let map,tup = d_map tup in
               AddressMap.add addr region map,tup)
        )
    ;;

    let serialize ctx mema memb acc =
      let Scalar.Result(included,acc,d_known) = serialize_known ctx mema.known memb.known acc in
      Scalar.Result(included, acc, fun tup ->
        let known,tup = d_known tup in
        {known;escaped=()},tup)

    let join_values ~size ctx v1 v2 =
      let Scalar.Result(_,tup,deserialize2) = Value.serialize ~size ctx v1 v2
        (true, Scalar.empty_tuple) in
      let res_tup = Scalar.typed_nondet2 ctx tup in
      fst @@ deserialize2 res_tup
    ;;

    let load ~size ctx mem at =
      let r = Operable_Value.fold ctx at (fun addr offset acc ->
          (* Codex_log.feedback "load addr %a"  (Operable_Value.binary_pretty ~size ctx) at; *)
          let v =
            match AddressMap.find addr mem.known with
            | exception Not_found -> Value.binary_empty ~size ctx
            | region -> Region.load ~size ctx region offset
          in v::acc
        ) [] in
      match r with
      | [] -> Value.binary_empty ~size ctx
      | [hd] -> hd  (* Most common case *)
      | hd::tl -> List.fold_left (join_values ~size ctx) hd tl
    ;;

    let _load ~size ctx mem at =
      let res = load ~size ctx mem at in
      Codex_log.feedback "load result: %a" (Value.binary_pretty ~size ctx) res;
      res
    ;;
    
    let store_single ~size ctx mem addr offset value =
        let region =
          try AddressMap.find addr mem.known
          with Not_found -> Region.initial ctx (Address.size_of addr)
        in
        let region = Region.store ~size ctx region offset value in
        let result = { mem with known = AddressMap.add addr region mem.known } in
        result
    ;;

    let join_memories ctx v1 v2 =
      let Scalar.Result(_,tup,deserialize2) = serialize ctx v1 v2
        (true,Scalar.empty_tuple) in
      let res_tup = Scalar.typed_nondet2 ctx tup in
      fst @@ deserialize2 res_tup
    ;;


    let store ~(size:int) ctx mem at value =
      (* Simple way: we perform different writes, and join the
         whole memory. We could be joining the regions instead,
         but it's probably not worth it. *)
      let r = Operable_Value.fold ctx at (fun addr offset acc ->
          try 
            let v = store_single ~size ctx mem addr offset value in
            v::acc
          with Memory_Empty -> acc
        ) [] in
      match r with
       | [] -> (* memory_empty ctx *) raise Memory_Empty
       | [hd] -> hd         (*  Most common case. *)
       | hd::tl -> List.fold_left (join_memories ctx) hd tl
    ;;    

    let store ~(size:int) ctx mem at value =
      (* Codex_log.feedback "before store value %a at: %a " (pretty ctx) mem (Operable_Value.binary_pretty ~size:32 ctx) at; *)
      let res = store ~size ctx mem at value in
      (* Codex_log.feedback "storing result %a" (pretty ctx) res; *)
      res
    ;;
    
    
    let malloc ~id ~malloc_size ctx mem =
      let addr = Address.Malloc (id,malloc_size) in
      (if AddressMap.mem addr mem.known
       then Codex_log.fatal "Malloc in a loop is not yet supported");
      let mem' = {
        known = AddressMap.add addr (Region.initial ctx malloc_size) mem.known;
        escaped = mem.escaped;
      }
      in
      Operable_Value.new_baddr addr ctx ~max:(Some malloc_size), mem'
    ;;

    let free ctx mem ptr =
      match Operable_Value.is_precise ctx ptr with
      | Empty -> mem
      | Imprecise -> Codex_log.fatal "Can only handle precise frees for now"
      | Singleton(addr,_) -> begin
          (if not @@ AddressMap.mem addr mem.known
           then Codex_log.fatal "Free on an object not previously allocated (%a)"
               Address.pretty addr);
          {
            known = AddressMap.remove addr mem.known;
            escaped = mem.escaped
          }
        end
    ;;

    let unknown ~level ctx = {known = AddressMap.empty; escaped = () }

    let should_focus ~size:_ _ = assert false

    let may_alias ~size:_ _ = assert false
  end
end
