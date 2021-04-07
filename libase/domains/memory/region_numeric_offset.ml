(**************************************************************************)
(*                                                                        *)
(*  This file is part of Codex.                                           *)
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

open Memory_sig;;

module Ival = Framac_ival.Ival


module type Operable_Value_Enumerable = sig
  include Operable_Value_Region
  val fold_crop:
    size:int -> Scalar.Context.t ->
    binary -> inf:Z.t -> sup:Z.t -> (Z.t -> 'a -> 'a) -> 'a -> 'a
  (* MAYBE: could be subsumed by fold_crop. *)
  val is_precise:
    size:int -> Scalar.Context.t -> binary -> Z.t Memory_sig.precision             
end

module type S = sig

  module Scalar : Domain_sig.Base

  (* A simple operable value that directly uses the Scalar.binary to
     represent memory locations. *)
  module Operable_Value:sig

    type binary
    
    module Binary_Forward:Transfer_functions.Binary_Forward
      with module Arity := Domain_sig.Context_Arity_Forward(Scalar.Context)
       and type boolean := Scalar.boolean
       and type binary := binary  
         
    include Operable_Value_Enumerable
      with module Scalar = Scalar
       and type binary := binary
       and module Binary_Forward := Binary_Forward
         
  end
  

  (* Can be used with any address, as long as we can enumerate on their value. *)
  module Memory
      (Address:Operable_Value_Enumerable with module Scalar = Scalar)
      (Value:Value with module Scalar = Scalar):sig
    include Memory_Region
      with module Scalar = Scalar
       and module Value = Value
       and type address = Address.binary
  end

end

module Make(Scalar:Domain_sig.Base)
  : S with module Scalar = Scalar
= struct

  module Scalar = Scalar

  (* Direct implementation: Operable_Value = Scalar. *)
  module [@always inline] Operable_Value(* :
     * Operable_Value_Enumerable
     * with module Scalar = Scalar
     *  and type binary = Scalar.binary *)
  =
  struct
    module Scalar = Scalar

    type binary = Scalar.binary
    module Binary = Scalar.Binary

    module Query = Scalar.Query

    let serialize ~size ctx a b acc =
      Scalar.serialize_binary ~size ctx a b acc

    let binary_empty = Scalar.binary_empty
    let ble = Scalar.Binary_Forward.biule
    let assume = Scalar.assume_binary
    let beq = Scalar.Binary_Forward.beq
    let zero_offset ~size ~max ctx = Scalar.Binary_Forward.biconst ~size Z.zero ctx
    let binary_unknown ~size ~level ctx = Scalar.binary_unknown ~size ~level ctx

    let is_precise ~size ctx offset =
      let x = Query.binary ~size ctx offset in
      match Query.binary_is_singleton ~size x with
      | None -> Imprecise
      | Some x -> Singleton x
    ;;

    let fold_crop ~size ctx (offset:binary) ~inf ~sup f acc =
      let x = Query.binary ~size ctx offset in
      Query.binary_fold_crop ~size x ~inf:Z.zero ~sup acc (fun int acc ->
          f int acc)
    ;;

    let binary_pretty ~size ctx fmt x =
      Format.fprintf fmt " + %a" (Scalar.binary_pretty ~size ctx) x


    let bisub = Scalar.Binary_Forward.bisub ~nsw:false ~nuw:false

    let bshift ~size ~offset ~max ctx x =
      Scalar.Binary_Forward.biadd ~size ~nsw:false ~nuw:true ctx x
        (Scalar.Binary_Forward.biconst ~size (Z.of_int offset) ctx)

    let bchoose ~size cond ctx x = Scalar.Binary_Forward.bchoose ~size cond ctx x
    
    let bindex ~size k ctx x i =
      let v =
        if k == 1 then i
        else
          let k = Scalar.Binary_Forward.biconst ~size (Z.of_int k) ctx in
          Scalar.Binary_Forward.bimul ~size ~nsw:false ~nuw:false ctx k i
      in          
      Scalar.Binary_Forward.biadd ~size ~nsw:false ~nuw:true ctx x v    

    module Binary_Forward = Scalar.Binary_Forward

    let within_bounds ~size ctx _ = Scalar.Boolean_Forward.true_ ctx
  end

  
  module Memory
      (Address:Operable_Value_Enumerable with module Scalar = Scalar)
      (Value:Value with module Scalar = Scalar) = struct
    module Operable_Value = Address
    
    module Scalar = Scalar
    module Value = Value
    type address = Operable_Value.binary

    (* Note: inside this module, the sizes are in bytes. *)
    module IntervalMap = struct

      module I = Interval_map.With_Extract(struct
          type t = Value.binary
        end)

      type t = I.t
      let create = I.create
      let store = I.store

      let extract ctx v ~idx ~size ~oldsize =
        if idx == 0 && size == oldsize (* Fast path *)
        then v
        else Value.Binary_Forward.bextract ctx v ~index:(idx * 8) ~size:(size * 8) ~oldsize:(oldsize * 8)
      ;;


      let fold_on_diff ctx a b acc f =
        I.fold_on_diff a b acc ~extract:(extract ctx) f

      let pretty ~size ctx fmt map =
        I.iter_between ~size 0 map ~extract:(extract ctx) (fun ~size offset v ->
            Format.fprintf fmt " (%d-%d)" offset (offset+size-1);
            Value.binary_pretty ctx ~size fmt v) ;;

      let get_size = I.get_size

    end

    (* The arguments ~size passed to this module are also in bytes. *)
    module Memory = struct
      type t = {map : IntervalMap.t; } [@@unboxed];;

      let pretty ctx fmt {map} =
        IntervalMap.pretty ctx fmt ~size:(IntervalMap.get_size map) map

      let initial ctx size =
        {map = IntervalMap.create ~size @@ (Value.binary_empty ctx ~size)}
      ;;

      let get_size {map} = IntervalMap.get_size map

      let load ~size ctx {map} key  =
        let key = Z.to_int key in
        assert(size > 0);
        assert(key >= 0);
        let region_size = IntervalMap.get_size map in
        (* Codex_log.feedback "loading offset %d size %d region_size %d %a" key size region_size (pretty ctx) {map}; *)                
        assert(key + size <= region_size);
        (* Kernel.feedback "load idx %d size %d" key size; *)
        let l = IntervalMap.I.fold_between ~size key map []
            ~extract:(IntervalMap.extract ctx)
            (fun ~size key value acc -> (size,value)::acc)
        in match l with
        | [] -> assert false    (* The address is correct, we must load something. *)
        | [_,v] -> v
        | (sizea,a)::rest ->
          (* TODO: this depends on endiannees *)
          let f (size,acc) (newsize,newv) =
            let newsize = newsize * 8 in
            let acc = Value.Binary_Forward.bconcat ctx ~size1:size acc ~size2:newsize newv in
            (size+newsize,acc)
          in
          let fsize,v = List.fold_left f (sizea*8,a) rest in
          assert (fsize == size * 8);
          v
      ;;

      let store ~size ctx region offset value =
        (* Codex_log.feedback "Store offset %d size %d" (Z.to_int offset) size; *)
        {map=IntervalMap.store ~size  (Z.to_int offset) region.map value}
      ;;

      let serialize ctx a b (included, acc) =
        let Scalar.Result(included,acc,d) =
          IntervalMap.fold_on_diff ctx a.map b.map (Scalar.Result(included,acc,fun tup -> a.map,tup))
            (fun ~size offset a b  (Scalar.Result(included,acc,d_map)) ->
              (* Codex_log.feedback "### serialize at offset@ %x:@ a =@ %a b =@ %a" offset (Value.binary_pretty ~size ctx) a (Value.binary_pretty ~size ctx) b; *)
              let Scalar.Result(included,acc,d_value) = Value.serialize ~size:(size * 8)
                ctx a b (included,acc) in
              Scalar.Result(included,acc, fun tup ->
                let value,tup = d_value tup in                  
                let map,tup = d_map tup in
                IntervalMap.store offset map ~size value,tup)
            ) in
        Scalar.Result(included,acc,fun tup -> let map,tup = d tup in {map},tup)
      ;;

    end

    (* Here, the ~size arguments are in bits. *)

    let is_precise = Operable_Value.is_precise;;      

    let fold_on_values ~size ctx (offset:Operable_Value.binary) ~sup acc f =
      Operable_Value.fold_crop ~size ctx offset ~inf:Z.zero ~sup:(Z.of_int sup) f acc
    ;;

    let join_values ~size ctx v1 v2 =
      let Scalar.Result(_,tup,deserialize) = Value.serialize ~size ctx v1 v2
        (true, Scalar.empty_tuple) in
      let res_tup = Scalar.typed_nondet2 ctx tup in
      let res,(_: Scalar.empty_tuple Scalar.out_tuple) = deserialize res_tup in
      res
    ;;

    
    let load ~size ctx region offset =
      assert(size land 7 == 0);
      let region_size = Memory.get_size region in
      let loaded = fold_on_values ~size ctx offset ~sup:(region_size - size/8) [] (fun offset acc ->
          let v = Memory.load ~size:(size/8) ctx region offset in
          v::acc
        ) in
      match loaded with
      | [] -> Value.binary_empty ~size ctx
      | [x] -> x                (* Most common case *)
      | head::rest -> List.fold_left (join_values ~size ctx) head rest
    ;;

    let get_size = Memory.get_size

    (* We have two version of stores. This one keeps a single memory,
       detects whether we make a weak or strong update, and join
       values. The other join memories, and is thus much slower. *)
    let store ~size ctx region offset (value:Value.binary) =
      assert(size land 7 == 0);
      match is_precise ~size ctx offset with
      | Empty -> raise Memory_Empty
      | Singleton(offset)
        when Z.lt offset Z.zero
          || Z.gt offset (Z.of_int (Memory.get_size region - size/8))               
        -> raise Memory_Empty                   
      | Singleton(offset) -> Memory.store ~size:(size/8) ctx region offset value
      | Imprecise ->
        let region_size = Memory.get_size region in
        (* As we fold on the same memory, this works even when the
           potential writes overlap on one another. *)
        let region = fold_on_values ~size ctx offset
            ~sup:(region_size - size/8) region (fun offset region ->
                let v = Memory.load ~size:(size/8) ctx region offset in
                let v' = join_values ~size ctx v value in
                let region = Memory.store ~size:(size/8) ctx region offset v' in
                region
              ) in
        region
    ;;

    let join_memories ctx v1 v2 =
      let Scalar.Result(included,tup,deserialize) =
        Memory.serialize ctx v1 v2 (true, Scalar.empty_tuple) in
      let res_tup = Scalar.typed_nondet2 ctx tup in
      let res,(_:Scalar.empty_tuple Scalar.out_tuple) = deserialize res_tup in
      res
    ;;

    let _store ~size ctx region offset (value:Value.binary) =
      assert(size land 7 == 0);
      let region_size = Memory.get_size region in
      (* As we fold on the same memory, this works even when the
           potential writes overlap on one another. *)
      let regions = fold_on_values ~size ctx offset
          ~sup:(region_size - size/8) [] (fun offset acc ->
              (* let v = Memory.load ~size:(size/8) ctx region offset in
               * let v' = join_values ~size ctx v value in *)
              let region' = Memory.store ~size:(size/8) ctx region offset value in
              region'::acc
            ) in
      match regions with
      | [] -> raise Memory_Empty
      | [x] -> x
      | a::b -> List.fold_left (join_memories ctx) a b
    ;;

    let serialize = Memory.serialize
    let initial = Memory.initial
    let pretty = Memory.pretty
    type memory = Memory.t

  end
  
end
