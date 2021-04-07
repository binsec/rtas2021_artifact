module Ctypes = Ctypes

module type Address_sig = sig
  type t
  val create : int -> t

  module Set : Stdlib.Set.S with type elt = t
  module Htbl : Stdlib.Hashtbl.S with type key = t
end

module type RegionS = sig
  module Virtual_address : Address_sig
  include Memory_sig.Whole_Memory
  val written_data_addrs : Virtual_address.Set.t ref
  val read_data_addrs : Virtual_address.Set.t ref
  val set_untyped_load : bool -> unit
  val set_check_store_intvl : bool -> unit
  val set_param_typing : (Ctypes.typ * int) Virtual_address.Htbl.t -> unit
end

module type S = sig
  module BR : Memory_sig.Operable_Value_Whole
  module Type : Type_domain.S
    with module BR = BR
  module TS : Ctypes.Type_settings
  module Virtual_address : Address_sig
  type 'a t = private
  | Bottom
  | T of { typ : Type.t; value : 'a; }
      (** invariant: none of the components is bottom. *)
  type binary = BR.binary t
  module type Binary_Representation_S = Memory_sig.Operable_Value_Whole
    with module Scalar = BR.Scalar
    and type binary = binary
  module Binary_Representation : Binary_Representation_S

  include Binary_Representation_S with type binary := binary

  val typ : size:int -> BR.Scalar.Context.t -> binary -> Type.t option
  val with_type : Type.t -> size:int -> BR.Scalar.Context.t -> binary -> binary
  val give_type : Type.t -> BR.binary -> binary
  val global_symbol : BR.Scalar.Context.t -> string -> int * binary
  val use_invariant : size:int -> BR.Scalar.Context.t -> Ctypes.Pred.t -> binary -> binary
  val check_invariant : size:int -> BR.Scalar.Context.t -> Ctypes.Pred.t -> binary -> bool

  module Region (DataReg : Memory_sig.Memory_Region
       with module Scalar = BR.Scalar
        (* The below restriction is necessary so that load(unknown) = unknown.
           It is acceptable because this domain is special-purpose. *)
        and type Value.binary = binary
        and type address = BR.binary
      ) : RegionS
        with module Scalar = BR.Scalar
        and module Value = DataReg.Value
        and type address = binary
        and module Virtual_address := Virtual_address
end

open Memory_sig

module type Needed_stuff = sig
  module Virtual_address : Address_sig
  val readable_interval : int -> Framac_ival.Ival.t
  val writable_interval : int -> Framac_ival.Ival.t
  val mmio_interval : int -> Framac_ival.Ival.t
  module Logger : Alarm_logger.S
  val option_use_shape : unit -> bool
end

module Make
    (BR : Operable_Value_Whole)
    (Type : Type_domain.S with module BR = BR)
    (TS : Ctypes.Type_settings)
    (Stuff : Needed_stuff)
: S
      with module BR = BR
      and module Type = Type
      and module TS = TS
      and module Virtual_address = Stuff.Virtual_address
= struct

  module Virtual_address = Stuff.Virtual_address
  module Logger = Stuff.Logger

  module BR = BR
  module Type = Type
  module TS = TS

  let zero ~size ctx =
    BR.Binary_Forward.biconst ~size Z.zero ctx

  let minus_one ~size ctx =
    BR.Binary_Forward.bisub ~size ctx ~nsw:false ~nuw:false (zero ~size ctx) @@
    BR.Binary_Forward.biconst ~size Z.one ctx

  let br_unknown ~size ctx =
    BR.binary_unknown ~size ~level:(BR.Scalar.Context.level ctx) ctx

  type 'a t =
  | Bottom
  | T of { typ : Type.t; value : 'a; }
      (** invariant: none of the components is bottom. *)

  let mk_compare cmp a b =
    match a,b with
    | Bottom,Bottom -> 0
    | Bottom,_ -> -1
    | _,Bottom -> 1
    | T a, T b ->
        let c = Type.compare a.typ b.typ in
        if c <> 0 then c else cmp a.value b.value

  let mk_pretty pp_val pp_typ fmt x =
    match x with
    | Bottom -> Format.pp_print_string fmt "Bottom"
    | T x ->
        Format.fprintf fmt "@[<hov 2>(%a, %a)@]" pp_val x.value pp_typ x.typ

  type binary = BR.binary t

  let param_typing : (Ctypes.typ * int) Virtual_address.Htbl.t option ref = ref None

  let unoption msg = function
  | Some x -> x
  | None -> raise (Failure msg)

  let is_empty ~size ctx b =
    let tmp = BR.Query.binary ~size ctx b in
    BR.Query.binary_is_empty ~size tmp

  let typ ~size:_ _ctx = function
  | Bottom -> None
  | T { typ;_ } -> Some typ

  let with_type typ ~size:_ _ctx = function
  | Bottom -> Bottom
  | T { typ = _; value } -> T { value; typ }

  (** Helpers, simply to type less characters. *)
  let give_type typ x = T { value = x; typ }
  let with_type_top value = T { typ = Type.Top; value }
  let binary_zero ~size ctx =
    let v = BR.Binary_Forward.biconst ~size Z.zero ctx in
    T { typ = Type.ZeroT; value = v }

  let coalesce_zero ~size ctx x =
    match x with
    | T {typ = Type.(Top|ParamT _); value} ->
        if Type.check_invariant_subdomain ~size ctx Ctypes.(Pred.eq (Const Z.zero)) value then
          T {typ = Type.ZeroT; value}
        else x
    | T {typ = Type.ZeroT;_} | Bottom -> x
    | T {typ = Type.Bottom;_} -> assert false

    let biconst ~size k ctx =
      if Z.equal k Z.zero then binary_zero ~size ctx
      else with_type_top @@ BR.Binary_Forward.biconst ~size k ctx

    let coalesce_bottom ~size ctx x =
      match x with
      | T { typ=Type.Bottom;_} -> Bottom
      | T { typ=_; value } ->
          if is_empty ~size ctx value then Bottom else x
      | Bottom -> Bottom

  module type Binary_Representation_S = Memory_sig.Operable_Value_Whole
    with module Scalar = BR.Scalar
    and type binary = binary
  (*
  module type Binary_Representation_S = sig
    include Memory_sig.Operable_Value_Whole
      with module Scalar = BR.Scalar
      and type binary = BR.binary t
    val subst_type : size:int -> BR.Scalar.Context.t -> binary -> binary
  end
  *)

  module Binary_Representation (*: Binary_Representation_S:
  sig
    include Operable_Value_Whole
      with module Scalar = BR.Scalar
       and type binary = BR.binary t
       (*and type Query.Binary.t = BR.Query.Binary.t t*)
    val unknown : size:int -> Scalar.Context.t -> binary
    val subst_type : size:int -> BR.Scalar.Context.t -> binary -> Type.t -> binary
  end*) =
  struct
    module Scalar = BR.Scalar

    type binary = BR.binary t

    module Binary = struct
      type t = binary
      let pretty _ = assert false
      let compare a b =
        mk_compare BR.Binary.compare a b
      (* Warning: inherits the imprecision of {!Type.imprecise_eq}. Should only
       * be used for optimization or in other contexts where exactitude does
       * not matter. *)
      let equal a b = 
        match a,b with
        | Bottom,Bottom -> true
        | Bottom,_ | _,Bottom -> false
        | T a, T b ->
            BR.Binary.equal a.value b.value 
            && (Type.imprecise_eq a.typ b.typ)

      let hash _ = assert false
    end


    module Query = struct
      let binary ~size ctx x =
        let b = match x with
        | Bottom -> BR.binary_empty ~size ctx
        | T {value;_} -> value
        in
        BR.Query.binary ~size ctx b
      let binary_to_ival = BR.Query.binary_to_ival
      let binary_is_singleton = BR.Query.binary_is_singleton
      module Binary_Lattice = BR.Query.Binary_Lattice
      let binary_fold_crop = BR.Query.binary_fold_crop
      let binary_is_empty = BR.Query.binary_is_empty

      let binary_to_known_bits = BR.Query.binary_to_known_bits
    end

    (* A map from adress to offset. Maybe: the condition for which the
       value has this offset; to eliminate most useless cases. *)

    let binary_pretty ~size ctx fmt x =
      let pp_bin = BR.binary_pretty ctx ~size in
      mk_pretty pp_bin (Type.pp ctx) fmt x

    (** If a type is unknown and the value is a special address, change its
       type. *)
    let subst_type ~size ctx a =
      match a with
      | Bottom -> Bottom
      | T a' ->
          if Type.eq ~only_descr:false ctx a'.typ Type.Top then begin
            match a'.value |> BR.Query.binary ~size ctx |> BR.Query.binary_to_ival
                ~signed:true ~size |> Framac_ival.Ival.project_int |> Z.to_int with
            | value ->
              let open Ctypes in
              let t = match !param_typing with
              | Some t -> Virtual_address.(Htbl.find_opt t (create value)) | None -> None in
              let t = match t with
              | Some (typ,ofs) ->
                  Logger.result "subst_type: case type info exists:@ %a,@,%d" Ctypes.pp typ ofs;
                  t
              | None ->
                begin match List.assoc_opt value TS.special_data_addrs with
                | Some (typ,ofs) as t ->
                    Logger.result "subst_type: case special_data_addrs:@ %a,@,%d"
                      Ctypes.pp typ ofs;
                    t
                | None -> None
              end
              in
              begin match t with
              | Some (typ,ofs) ->
                begin
                  let open Pred in
                  (* [a] is an address to a cell of type [(typ,ofs)], therefore [a]
                   * has type "pointer to [typ] with offset [ofs]". *)
                  (* We add a second level of pointer and remove it immediately
                   * with [deref]. This enables to have a precise value for the
                   * index. Then we add [ofs] with [Type.add_offset]. *)
                  let zero = zero ~size:Type.array_index_size ctx in
                  let minus_one = minus_one ~size:Type.array_index_size ctx in
                  let pred = (if value - ofs <> 0 then neq (Const Z.zero) else True) in
                  let ptyp = {descr = Ptr{pointed=typ;index=Zero}; pred; const = false} in
                  let pptyp = Type.(ParamT ({descr = Ptr{pointed = ptyp;index=Zero};
                    pred = True; const = false}, zero, minus_one, 0)) in
                  let ptyp',_ = Type.deref ctx pptyp in
                  match ptyp' with
                  | Type.ParamT (ptyp', idx, fe, 0) ->
                      let new_t = Type.add_offset ctx ptyp' idx fe 0
                        (BR.Binary_Forward.biconst ~size:Type.offset_size (Z.of_int ofs) ctx) in
                      coalesce_bottom ~size ctx @@ T { a' with typ = new_t }
                  | _ -> assert false
                end
              | None -> Logger.result "subst_type: case no type info"; a
              end
            | exception Framac_ival.Ival.Not_Singleton_Int -> a
            | exception Z.Overflow -> a
          end
          else a

    let serialize ~size ctx a b ((included,acc) as in_acc) =
      let a = subst_type ~size ctx a in
      let b = subst_type ~size ctx b in
      match a,b with
      | Bottom, Bottom ->
          BR.Scalar.Result (included, acc, fun output -> Bottom, output)
      | Bottom, T {typ; value} ->
          let BR.Scalar.Result (inc, acc, deserialize_typ) =
            Type.serialize ctx Type.Bottom typ in_acc in
          let dummy_a = BR.binary_empty ~size ctx in
          let BR.Scalar.Result(inc, acc,deserialize) =
            BR.serialize ~size ctx dummy_a value (inc,acc) in
          BR.Scalar.Result (inc, acc, fun output ->
            let value, output = deserialize output in
            let typ, output = deserialize_typ output in
            T {typ; value}, output
          )
      | T {typ; value}, Bottom ->
          let BR.Scalar.Result (inc, acc, deserialize_typ) =
            Type.serialize ctx typ Type.Bottom in_acc in
          let dummy_b = BR.binary_empty ~size ctx in
          let BR.Scalar.Result(inc,acc,deserialize) =
            BR.serialize ~size ctx value dummy_b (inc,acc) in
          BR.Scalar.Result (inc, acc, fun output ->
            let value, output = deserialize output in
            let typ, output = deserialize_typ output in
            T {typ; value}, output
          )
      | T a, T b ->
          let BR.Scalar.Result (included, acc, deserialize_typ) =
            Type.serialize ctx a.typ b.typ in_acc in
          if a.value == b.value then
            BR.Scalar.Result(included, acc, fun output ->
              let new_typ, output = deserialize_typ output in
              T { a with typ = new_typ }, output
          )
          else
            let BR.Scalar.Result(inc, acc, deserialize) =
              BR.serialize ~size ctx a.value b.value (included,acc) in
            BR.Scalar.Result(inc, acc, (fun output ->
                let v, output = deserialize output in
                let new_typ, output = deserialize_typ output in
                give_type new_typ v, output))
    ;;

    let concat ~size1 ~size2 ctx a b =
      Logger.debug ~level:3 "Region_binary.concat %d %d %a %a" size1 size2 (binary_pretty ~size:size1 ctx) a (binary_pretty ~size:size2 ctx) b;
      match a, b with
      | Bottom,_ | _,Bottom -> Bottom
      | T {typ = Type.ZeroT; value = value_a}, T {typ = Type.ZeroT; value = value_b} ->
          T {typ = Type.ZeroT; value = (BR.Binary_Forward.bconcat ctx ~size1 value_a ~size2
            value_b)}
      | T {typ = Type.(Top|ZeroT); value = value_a}, T {typ = Type.(Top|ZeroT); value = value_b} ->
          with_type_top (BR.Binary_Forward.bconcat ctx ~size1 value_a ~size2
            value_b)
      | T {typ = Type.ParamT _;_}, _ | _, T {typ = Type.ParamT _;_} ->
          let msg = Format.asprintf "concatenation should always happen \
            between Top values." in
          raise (Type.Type_error msg)
      | T {typ=Type.Bottom;_}, _ | _, T {typ=Type.Bottom;_} ->
          assert false

    module Binary_Forward = struct

      let biconst = biconst

      let bofbool ~size ctx bool =
        with_type_top @@ BR.Binary_Forward.bofbool ~size ctx bool

      let biule ~size ctx a b =
        let open Ctypes in
        match a,b with
        | Bottom,_ | _,Bottom ->
            BR.Scalar.boolean_empty ctx
        | T a, T b ->
            begin match a.typ, b.typ with
            | Type.ParamT ({descr = Ptr{pointed={descr=Array (elem_a, sz_a);_};_};_},
                  _idx_a, fe_a, ofs_a),
              Type.ParamT ({descr = Ptr{pointed={descr=Array (elem_b, sz_b);_};_};_},
                  _idx_b, fe_b, ofs_b) when ofs_a = ofs_b && sz_a = sz_b && sz_a <> None ->
                let c = List.exists (fun t -> Ctypes.equal ~only_descr:true elem_a t)
                  TS.unique_array_types in
                if c && Ctypes.equal ~only_descr:true elem_a elem_b then begin
                  (* There is only one array of this type. *)
                  Logger.result "exploiting uniqueness";
                  (* We return the result of testing equality on the indices. *)
                  (*let c1 = BR.Binary_Forward.biule ~size:Type.offset_size ctx idx_a idx_b in*)
                  let c2 = BR.Binary_Forward.biule ~size:Type.offset_size ctx fe_a fe_b in
                  (*BR.Scalar.Boolean_Forward.(&&) ctx c1 c2*)
                  c2
                end else
                  BR.Binary_Forward.biule ~size ctx a.value b.value
            | _ ->
                (* In all other cases, ignore the type *)
                BR.Binary_Forward.biule ~size ctx a.value b.value
            end

      let bisle ~size ctx a b =
        let open Ctypes in
        match a,b with
        | Bottom,_ | _,Bottom ->
            BR.Scalar.boolean_empty ctx
        | T a, T b ->
            begin match a.typ, b.typ with
            | Type.ParamT ({descr = Ptr{pointed={descr=Array (elem_a, sz_a);_};_};_},
                  _idx_a, fe_a, ofs_a),
              Type.ParamT ({descr = Ptr{pointed={descr=Array (elem_b, sz_b);_};_};_},
                  _idx_b, fe_b, ofs_b) when ofs_a = ofs_b && sz_a = sz_b && sz_a <> None ->
                let c = List.exists (fun t -> Ctypes.equal ~only_descr:true elem_a t)
                  TS.unique_array_types in
                if c && Ctypes.equal ~only_descr:true elem_a elem_b then begin
                  (* There is only one array of this type. *)
                  Logger.result "exploiting uniqueness";
                  (* We return the result of testing equality on the indices. *)
                  (*let c1 = BR.Binary_Forward.bisle ~size:Type.offset_size ctx idx_a idx_b in*)
                  let c2 = BR.Binary_Forward.bisle ~size:Type.offset_size ctx fe_a fe_b in
                  (*BR.Scalar.Boolean_Forward.(&&) ctx c1 c2*)
                  c2
                end else
                  BR.Binary_Forward.bisle ~size ctx a.value b.value
            | _ ->
                (* In all other cases, ignore the type *)
                BR.Binary_Forward.bisle ~size ctx a.value b.value
            end

      let beq ~size ctx a b =
        match a,b with
        | Bottom,_ | _,Bottom ->
            BR.Scalar.boolean_empty ctx
        | T a, T b ->
            let open Ctypes in
            begin match a.typ, b.typ with
            | Type.ParamT ({descr = Ptr{pointed={descr=Array (elem_a, sz_a);_};_};_},
                  _idx_a, fe_a, ofs_a),
              Type.ParamT ({descr = Ptr{pointed={descr=Array (elem_b, sz_b);_};_};_},
                  _idx_b, fe_b, ofs_b) when ofs_a = ofs_b && sz_a = sz_b && sz_a <> None ->
                let c = List.exists (fun t -> Ctypes.equal ~only_descr:true elem_a t)
                  TS.unique_array_types in
                if c && Ctypes.equal ~only_descr:true elem_a elem_b then begin
                  (* There is only one array of this type. *)
                  Logger.result "exploiting uniqueness";
                  (* We return the result of testing equality on the indices. *)
                  (*let c1 = BR.Binary_Forward.beq ~size:Type.offset_size ctx idx_a idx_b in*)
                  let c2 = BR.Binary_Forward.beq ~size:Type.offset_size ctx fe_a fe_b in
                  (*BR.Scalar.Boolean_Forward.(&&) ctx c1 c2*)
                  c2
                end else
                  BR.Binary_Forward.beq ~size ctx a.value b.value
            | _ ->
                (* In all other cases, ignore the type *)
                BR.Binary_Forward.beq ~size ctx a.value b.value
            end

      (* TODO: Only checks array bounds. Other checks are still done in the
         [store] and [load] functions. It would be better programming taste to
         put all checks in here. *)
      let valid ~size:_ _acc_type ctx (ptr : binary) =
        let open Ctypes in
        let isize = Type.array_index_size in
        match ptr with
        | T {typ = Type.ParamT ({descr = Ptr{pointed={descr = Array (_, _);_};_};_}, idx, fe, _);_} ->
            let zero = zero ~size:isize ctx in
            let c1 = BR.Binary_Forward.bisle ~size:isize ctx zero idx in
            let c2 = BR.Scalar.Boolean_Forward.not ctx @@
              BR.Binary_Forward.bisle ~size:isize ctx zero fe in
            BR.Scalar.Boolean_Forward.(&&) ctx c1 c2
        | Bottom | T {typ = Type.Bottom | Type.ZeroT | Type.ParamT _ | Type.Top;_} ->
            BR.Scalar.Boolean_Forward.true_ ctx

      let valid_ptr_arith ~size ctx x y =
        match x,y with
        | T {typ = Type.ParamT (_,_,_,_); value = ptrval}, _
        | _, T {typ = Type.ParamT (_,_,_,_); value = ptrval} ->
            let zero = zero ~size ctx in
            BR.Scalar.Boolean_Forward.not ctx @@
            BR.Binary_Forward.beq ~size ctx ptrval zero
        | (Bottom | T {typ=Type.(Bottom|ZeroT|Top);_}),
          (Bottom | T {typ=Type.(Bottom|ZeroT|Top);_}) ->
            BR.Scalar.Boolean_Forward.true_ ctx


      (**************** Arithmetic operations. ****************)

      let buninit ~size:_ _ctx = assert false

      (* Common to arithmetical unary operations. *)
      let ar1 op ~size ctx x =
        match x with
        | Bottom -> Bottom
        | T x -> T { x with value = op ~size ctx x.value }

      (* TODO: The [bitimes] constructor should disappear. *)
      (* let bitimes ~size k ctx x =
       *   if Z.equal Z.one k then x
       *   else ar1 (BR.Binary_Forward.bitimes k) ~size ctx x *)

      let buext ~size ~oldsize ctx x =
        match x with
        | Bottom -> Bottom
        | T x -> with_type_top (BR.Binary_Forward.buext ~size ~oldsize ctx x.value)

      let bsext ~size ~oldsize ctx x =
        match x with
        | Bottom -> Bottom
        | T x -> with_type_top (BR.Binary_Forward.bsext ~size ~oldsize ctx x.value)

      let bextract ~size ~index ~oldsize ctx x =
        if size == oldsize then x
        else match x with
        | Bottom -> Bottom
        (* Extraction erases the type of a pointer. *)
        | T x -> coalesce_zero ~size ctx @@
            T { value = BR.Binary_Forward.bextract ~index ~oldsize ~size ctx x.value;
            typ = Type.Top; }

      let assume ~size ctx cond = function
        | Bottom -> Bottom
        | T x ->
            coalesce_zero ~size ctx @@ coalesce_bottom ~size ctx @@
              T { value = BR.assume ~size ctx cond x.value;
                typ = Type.assume ctx cond x.typ }

      let ar2 op_name op ~size ctx a b =
        match a,b with
        | _,Bottom | Bottom,_ -> Bottom
        | T a', T b' ->
            let value = op ~size ctx a'.value b'.value in
            begin match a'.typ, b'.typ with
            | Type.((Top|ZeroT), (Top|ZeroT)) -> with_type_top value
            | Type.(ParamT _, _ | _, ParamT _) ->
                (* Binary operation other than add or sub on pointers is invalid.
                 * XXX: some operators, such as bitwise, should probably not
                 * generate an alarm but simply result in type Top. *)
                Logger.warning "@[<hov 2>%s between %a and %a returns Top@]"
                  (String.capitalize_ascii op_name)
                  (binary_pretty ~size ctx) a
                  (binary_pretty ~size ctx) b;
                coalesce_zero ~size ctx @@ with_type_top value
            | Type.Bottom, _ | _, Type.Bottom -> assert false
            end

      let bimul ~size ~nsw ~nuw ctx a b =
        ar2 "multiplication" (BR.Binary_Forward.bimul ~nsw ~nuw) ~size ctx a b
      let bxor ~size ctx a b = ar2 "binary xor" (BR.Binary_Forward.bxor) ~size ctx a b
      let band ~size ctx a b = ar2 "binary and" (BR.Binary_Forward.band) ~size ctx a b
      let bor ~size ctx a b = ar2 "binary or" (BR.Binary_Forward.bor) ~size ctx a b

      let bashr ~size ctx a b = ar2 "arith. right shift" (BR.Binary_Forward.bashr) ~size ctx a b
      let blshr ~size ctx a b = ar2 "blshr" (BR.Binary_Forward.blshr) ~size ctx a b
      let bshl ~size ~nsw ~nuw ctx a b =
        ar2 "binary left shift" (BR.Binary_Forward.bshl ~nsw ~nuw) ~size ctx a b

      let bisdiv ~size ctx a b = ar2 "bisdiv" (BR.Binary_Forward.bisdiv) ~size ctx a b
      let bismod ~size ctx a b = ar2 "bismod" (BR.Binary_Forward.bismod) ~size ctx a b
      let biudiv ~size ctx a b = ar2 "biudiv" (BR.Binary_Forward.biudiv) ~size ctx a b
      let biumod ~size ctx a b = ar2 "biumod" (BR.Binary_Forward.biumod) ~size ctx a b

      let bconcat = concat

      let bindex ~size:_ _ _ = assert false
      let bshift  ~size:_ ~offset:_ ~max:_ _ = assert false

      let binary_of_value ~size ctx v =
        let open Ctypes in
        match v with
        | Const x -> biconst ~size x ctx
        | Sym s ->
            let sz,v = Type.symbol ctx s in
            assert (sz = size);
            with_type_top v

      let cond_of_cmp ~size ctx cmpop v1 v2 =
        let open Ctypes.Pred in
        match cmpop with
        | Equal ->
            beq ~size ctx v1 v2
        | NotEqual ->
            BR.Scalar.Boolean_Forward.not ctx @@ beq ~size ctx v1 v2
        | ULt ->
            BR.Scalar.Boolean_Forward.not ctx @@ biule ~size ctx v2 v1
        | SLt ->
            BR.Scalar.Boolean_Forward.not ctx @@ bisle ~size ctx v2 v1
        | ULeq ->
            biule ~size ctx v1 v2
        | SLeq ->
            bisle ~size ctx v1 v2
        | UGeq ->
            biule ~size ctx v2 v1
        | SGeq ->
            bisle ~size ctx v2 v1
        | UGt ->
            BR.Scalar.Boolean_Forward.not ctx @@ biule ~size ctx v1 v2
        | SGt ->
            BR.Scalar.Boolean_Forward.not ctx @@ bisle ~size ctx v1 v2

      let lift_unop ~size ctx op =
        let open Ctypes.Pred in
        match op with
        | Extract (index,len) -> fun x ->
          bextract ~size:len ~index ~oldsize:size ctx x
          |> buext ~size ~oldsize:len ctx

      let rec lift_binop ~size ctx op =
        let open Ctypes.Pred in
        match op with
        | Add -> biadd ~size ctx ~nsw:false ~nuw:false
        | Sub -> bisub ~size ctx ~nsw:false ~nuw:false
        | Mul -> bimul ~size ctx ~nsw:false ~nuw:false
        | And -> band ~size ctx
        | Concat (size1,size2) -> bconcat ~size1 ~size2 ctx

      and binary_of_expr ~size ctx ~self e : binary =
        let open Ctypes.Pred in
        match e with
        | Self -> self
        | Val v -> binary_of_value ~size ctx v
        | Unop (op, e) -> lift_unop ~size ctx op (binary_of_expr ~size ctx ~self e)
        | Binop (op, e1, e2) -> lift_binop ~size ctx op
            (binary_of_expr ~size ctx ~self e1) (binary_of_expr ~size ctx ~self e2)

      and cond_of_pred ~size ctx pred ~self =
        let open Ctypes.Pred in
        match pred with
        | True -> BR.Scalar.Boolean_Forward.true_ ctx
        | Cmp (op, e1, e2) ->
            cond_of_cmp ~size ctx op (binary_of_expr ~size ctx ~self e1)
            (binary_of_expr ~size ctx ~self e2)
        | And (p,q) ->
            let c1 = cond_of_pred ~size ctx p ~self in
            let c2 = cond_of_pred ~size ctx q ~self in
            BR.Scalar.Boolean_Forward.(&&) ctx c1 c2

      and check_invariant ~size ctx pred v =
        Logger.result "check_invariant %a" Ctypes.Pred.pp pred;
        let c = cond_of_pred ~size ctx pred ~self:v in
        let is_true =
          BR.Scalar.Query.convert_to_quadrivalent (BR.Scalar.Query.boolean ctx c) in
        match is_true with
        | Quadrivalent_Lattice.True -> true
        | _ -> false (* Possibly false *)

      (** Returns a value created from the invariants that exist on the field
         pointed by (typ,offset), or "top" if there is no invariant.
         @param is_pointer true iff (typ,offset) points to a pointer type. *)
      and use_invariant ~size ctx pred value =
        Logger.result "use_invariant %a" Ctypes.Pred.pp pred;
        let open Ctypes.Pred in
        match pred with
        | True -> value
        | _ -> assume ~size ctx (cond_of_pred ~size ctx pred ~self:value) value

      (* XXX: When adding an offset to a pointer, what happens if the addition
       * overflows? It should be reported.  But for that, a certain amount of
       * precision is needed on the initial offset. We should make sure to have
       * it. *)
      and biadd ~size ~nsw ~nuw ctx a0 b0 =
        match a0,b0 with
        | Bottom,_ | _,Bottom -> Bottom
        | T a, T b ->
            let value = BR.Binary_Forward.biadd ~size ~nsw ~nuw ctx a.value b.value in
            let add_to_addr addr ptr_t addr_idx addr_fe addr_offset increment =
              Logger.result "@[<hov 2>addition of offset %a to param. addr. %a@]"
                (BR.binary_pretty ~size:Type.offset_size ctx) increment
                (binary_pretty ~size ctx) addr;
              (try
                (* Adding the offset. If the validity of the new index couldn't be
                 * verified, it's fine to keep and use it (other verifications will
                 * be made upon dereferencing), but non-nullity of the pointer is
                 * no longer guaranteed to be conserved. *)
                let new_ptr_typ =
                  Type.add_offset ctx ptr_t addr_idx addr_fe addr_offset
                    increment in
                Logger.result "new pointer type:@ %a" (Type.pp ctx) new_ptr_typ;
                Logger.check "manipulating_null_ptr";
                let t_value = coalesce_bottom ~size ctx @@ T { value; typ = new_ptr_typ } in
                let t_value =
                  if check_invariant ~size ctx Ctypes.(Pred.neq (Const Z.zero)) addr then
                    (* A well-typed offset preserves non-nullity. If the offset was
                     * not well-typed, an alarm was emitted by [add_offset] above,
                     * and we will just assume it was. *)
                      use_invariant ~size ctx Ctypes.(Pred.neq (Const Z.zero)) t_value
                  else begin
                    Logger.alarm "manipulating_null_ptr";
                    Logger.error "Adding offset to possibly null pointer";
                    t_value
                  end
                in
                t_value
              with
              | Type.Type_error msg ->
                  Logger.warning "Ill-typed offset addition: %s" msg;
                  T { value; typ = Type.Top }
              )
            in
            let open Type in
            match a.typ, b.typ with
            | ZeroT,_ -> b0
            | _,ZeroT -> a0
            | Top, Top ->
                coalesce_zero ~size ctx @@ with_type_top value
            | ParamT (_,_,_,_), ParamT (_,_,_,_) ->
                (* Remember that {ParamT} always contains a pointer type. *)
                Logger.warning "@[<hov 2>addition of pointers %a and %a is not \
                  allowed@]"
                  (binary_pretty ~size ctx) a0 (binary_pretty ~size ctx) b0;
                with_type_top value
            | Top, ParamT (t, idx, fe, offset) ->
                add_to_addr b0 t idx fe offset a.value
            | ParamT (t, idx, fe, offset), Top ->
                add_to_addr a0 t idx fe offset b.value
            | Type.Bottom,_ | _,Type.Bottom -> assert false

      and bisub ~size ~nsw ~nuw ctx a0 b0 =
        match a0,b0 with
        | Bottom,_ | _,Bottom -> Bottom
        | T a, T b ->
            let value = BR.Binary_Forward.bisub ~size ~nsw ~nuw ctx a.value b.value in
            let zero = zero ~size ctx in
            let open Ctypes in
            begin match a.typ, b.typ with
            | _, Type.ZeroT -> a0
            | Type.((Top|ZeroT), Top) ->
                coalesce_zero ~size ctx @@ with_type_top value
            | Type.ParamT ({descr = Ptr{pointed={descr=Array (elem_a, sz_a);_};_};_},
                  _idx_a, fe_a, ofs_a),
              Type.ParamT ({descr = Ptr{pointed={descr=Array (elem_b, sz_b);_};_};_},
                  _idx_b, fe_b, ofs_b) when ofs_a = ofs_b && sz_a = sz_b && sz_a <> None ->
                let c = List.exists (fun t -> Ctypes.equal ~only_descr:true elem_a t)
                  TS.unique_array_types in
                if c && Ctypes.equal ~only_descr:true elem_a elem_b then begin
                  Logger.result "substraction of same-array pointers identified.";
                  (* We return the difference of from the point of view of the
                   * distance to the end, which should be the most useful thing to
                   * back-propagate on. *)
                  with_type_top @@ BR.Binary_Forward.bisub ~nsw:false ~nuw:false
                  ~size:Type.offset_size ctx fe_a fe_b
                end else begin
                  if not (Ctypes.equal ~only_descr:true elem_a elem_b) then
                    Logger.warning "@[<hov 2>substraction of pointers %a and %a of \
                      different types is weird@]"
                      (binary_pretty ~size ctx) a0 (binary_pretty ~size ctx) b0;
                  with_type_top value
                end
            | Type.ParamT (_,_,_,_), Type.ParamT (_,_,_,_) ->
                Logger.warning "@[<hov 2>substraction of pointers %a and %a of \
                  different types is weird@]"
                  (binary_pretty ~size ctx) a0 (binary_pretty ~size ctx) b0;
                with_type_top value
            | Type.(Top|ZeroT), Type.ParamT (_,_,_,_) ->
                let minus_a =
                  T { a with value = BR.Binary_Forward.bisub ~size ~nsw:false ~nuw:false ctx zero a.value } in
                biadd ~size ~nsw:false ~nuw:false ctx minus_a b0
            | Type.ParamT (_,_,_,_), Type.Top ->
                let minus_b =
                  T { b with value = BR.Binary_Forward.bisub ~size ~nsw:false ~nuw:false ctx zero b.value } in
                biadd ~size ~nsw:false ~nuw:false ctx a0 minus_b
            | Type.Bottom, _ | _, Type.Bottom -> assert false
            end

      let bchoose ~size:_ _ = assert false

    end

    let binary_empty ~size:_ _ctx = Bottom

    (* XXX: The offset for unknown is essentially useless, and we create a lot of them.
       Maybe it is important to optimize them. *)
    (* XXX: I should generate assumptions based on the type:
       if an integer, state that it must be in some interval, and the base is null;
       if a pointer, keep it that way. *)
    let binary_unknown ~size ~level ctx =
      with_type_top @@ BR.binary_unknown ~size ~level ctx

    let assume = Binary_Forward.assume

  end

  include (Binary_Representation : Binary_Representation_S with type binary := binary)
  let use_invariant = Binary_Representation.Binary_Forward.use_invariant
  let check_invariant = Binary_Representation.Binary_Forward.check_invariant

  let global_symbol ctx name =
    let size,v = Type.symbol ctx name in
    size, give_type Type.Top v

  module ParamReg = struct
    type memory = unit

    (* Subtyping check, with special case for Bottom. *)
    let included ctx a b_typ =
      match a with
      | Bottom -> true
      | T a -> Type.subtype ctx a.typ b_typ

    let may_intersect ctx t u =
      Type.may_subtype ctx t u || Type.may_subtype ctx u t

    type nullity = MaybeNull | NotNull

    (** Checks that the type of the address is consistent (i.e. represents the
       same concrete type) with the type of the value to store. *)
    let store ~size ctx (_mem : memory) addr_t addr_val value =
      (*
      Logger.result "ParamReg.store value %a at addr. %a"
        (Binary_Representation.binary_pretty ~size ctx) value
        (Binary_Representation.binary_pretty ~size:32 ctx) addr;
      *)
      Logger.check "store_param_nonptr";
      let pointed_t, pointed_val_pred = try Type.deref ctx addr_t
        with Type.Type_error msg ->
          Logger.alarm "store_param_nonptr"; Logger.error "%s" msg;
          Type.Top, Ctypes.Pred.True
      in
      (* If this is a special data address, change its type accordingly. *)
      let value' = Binary_Representation.subst_type ~size ctx value in
      Logger.check "typing_store";
      if not (included ctx value' pointed_t) then begin
        Logger.alarm "typing_store";
        Logger.error "Storing value@ %a to address@ %a: incompatible types"
          (Binary_Representation.binary_pretty ctx ~size) value'
          (Type.pp ctx) addr_t
      end;
      let ptr_size = Codex_config.ptr_size () in
      let zero = zero ~size:ptr_size ctx in
      let is_zero = BR.Binary_Forward.beq ~size:ptr_size ctx addr_val zero
        |> BR.Scalar.Query.boolean ctx |> BR.Scalar.Query.convert_to_quadrivalent
      in
      Logger.check "predicate";
      if not (Binary_Representation.Binary_Forward.check_invariant ~size ctx
          pointed_val_pred value') then begin
        Logger.alarm "predicate";
        Logger.error "Predicate %a possibly does not hold."
          Ctypes.Pred.pp pointed_val_pred
      end;
      Logger.check "store_param_nonzero";
      begin match is_zero with
      | Quadrivalent_Lattice.True | Quadrivalent_Lattice.Top
      | Quadrivalent_Lattice.Bottom ->
          Logger.alarm "store_param_nonzero";
          Logger.error "Possibly dereferencing Zero.";
          MaybeNull
      | Quadrivalent_Lattice.False -> NotNull
      end

    let load ~size ctx (_mem : memory) addr_t addr_val =
      Logger.debug ~level:3 "Region_binary.ParamReg.load ~size:%d" size;
      Logger.check "load_param_nonptr";
      let value_type, pred =
        try Type.deref ctx addr_t
        with Type.Type_error msg ->
          Logger.alarm "load_param_nonptr";
          Logger.error "%s" msg; Type.Top, Ctypes.Pred.True
      in
      let ptr_size = Codex_config.ptr_size () in
      let zero = zero ~size:ptr_size ctx in
      let is_zero = BR.Binary_Forward.beq ~size:ptr_size ctx addr_val zero
        |> BR.Scalar.Query.boolean ctx |> BR.Scalar.Query.convert_to_quadrivalent
      in
      Logger.check "load_param_nonzero";
      begin match is_zero with
      | Quadrivalent_Lattice.True | Quadrivalent_Lattice.Top ->
          Logger.alarm "load_param_nonzero";
          Logger.error "Possibly dereferencing Zero."
      | _ -> ()
      end;
      let value_type = if Type.pointer value_type then value_type else Type.Top in
      let unknown = br_unknown ~size ctx in
      let ret = T { typ = value_type; value = unknown } in
      let value = Binary_Representation.Binary_Forward.use_invariant ~size ctx pred ret in
      Logger.result "ParamReg.load: value = %a" (Binary_Representation.binary_pretty ~size ctx) value;
      value
  end

  module Region (DataReg : Memory_Region
       with module Scalar = BR.Scalar
        (* The below restriction is necessary so that load(unknown) = unknown.
           It is acceptable because this domain is special-purpose. *)
        and type Value.binary = Binary_Representation.binary
        and type address = BR.binary
      ) : RegionS
        with module Scalar = BR.Scalar
        and module Value = DataReg.Value
        and type address = binary
        and module Virtual_address = Virtual_address
  = struct
    module Scalar = BR.Scalar

    module Value = DataReg.Value

    module Virtual_address = Virtual_address

    type address = DataReg.address t

    type memory =
      | BottomMem
      | M of { data_mem : DataReg.memory; param_mem : ParamReg.memory; }

    let memory_empty _ = BottomMem

    let mk m = M
      { data_mem = m;
        param_mem = ();
      }

    let br_unknow ~size ctx =
      BR.binary_unknown ~size ~level:(BR.Scalar.Context.level ctx) ctx

    let serialize ctx m1 m2 ((included,acc) as input) =
      match m1,m2 with
      (* FIXME: well that seems wrong. *)
      | BottomMem,_ ->
          Scalar.Result (included, acc, fun output -> BottomMem, output)
      | _,BottomMem ->
          Scalar.Result (false, acc, fun output -> BottomMem, output)
      | M m1, M m2 ->
          let Scalar.Result(inc,a,d) = DataReg.serialize ctx m1.data_mem m2.data_mem input in
          Scalar.Result(inc, a, fun output -> let mem,output = d output in (mk mem),output)

    let pretty ctx fmt = function
      | BottomMem -> Format.pp_print_string fmt "BottomMem"
      | M mem ->
          Format.fprintf fmt "@[<v 2>data mem:@ %a,@.param. mem:@ ()@]" (DataReg.pretty ctx) mem.data_mem

    let ptr_size () =
      Codex_config.ptr_size ()

    let join_values ~size ctx v1 v2 =
      let Scalar.Result(_,tup,deserialize2) = Binary_Representation.serialize ~size
        ctx v1 v2 (true, BR.Scalar.empty_tuple) in
      let res_tup = Scalar.typed_nondet2 ctx tup in
      fst @@ deserialize2 res_tup
    ;;


    let written_data_addrs = ref Virtual_address.Set.empty
    let read_data_addrs = ref Virtual_address.Set.empty

    let untyped_load = ref false
    let set_untyped_load b = untyped_load := b
    let check_store_intvl = ref true
    let set_check_store_intvl b = check_store_intvl := b

    let set_param_typing typing =
      param_typing := Some typing

    let should_focus ~size ctx _mem (value : binary) =
      (*
      let open Ctypes in
      match value with
      | Bottom -> None
      | T { typ = Type.(Top | ZeroT); value = _ } -> None
      | T { typ = Type.ParamT ({descr=Ptr {pointed;_};_},_,_,ofs);_ } ->
        begin
          let descr = match pointed.descr with
          | Ctypes.Array (elt_typ, _) -> elt_typ.descr
          | other -> other
          in
          let base = Binary_Forward.bisub ~nsw:false ~nuw:false ~size ctx value @@ Binary_Forward.biconst ~size (Z.of_int ofs) ctx in
          Some (base, Type.type_size descr * 8, ofs * 8)
        end
      | T { typ = Type.ParamT _; value = _}
      | T { typ = Type.Bottom; value = _ } -> assert false
      *)
      None

    let may_alias ~size ctx x y =
      match x,y with
      | Bottom,_ | _,Bottom -> false
      | T tx, T ty ->
          ParamReg.may_intersect ctx tx.typ ty.typ
          && (
            let quadri = Binary_Forward.beq ~size ctx x y
              |> Scalar.Query.boolean ctx |> Scalar.Query.convert_to_quadrivalent in
            match quadri with
            | Quadrivalent_Lattice.False -> false
            | Quadrivalent_Lattice.Top | Quadrivalent_Lattice.True
            | Quadrivalent_Lattice.Bottom-> true
          )

    let store_nonbottom ~size ctx data_mem addr_typ addr_val value =
      let open Framac_ival in
      let addr_ival = addr_val
        |> BR.Query.binary ~size:(ptr_size ()) ctx
        |> BR.Query.binary_to_ival ~signed:false ~size:(ptr_size ())
      in
      if Type.eq ~only_descr:false ctx addr_typ Type.Top then begin
        (* If the store is made to a presumed MMIO address, we just ignore it. *)
        if Ival.is_included addr_ival (Stuff.mmio_interval size) then begin
          Logger.result "Storing to (presumed) MMIO address %a"
            Ival.pretty addr_ival;
          mk data_mem
        end
        else begin
          if !check_store_intvl then begin
            (* The store must me done in data memory. We check that the address
             * value belongs to that region. *)
            Logger.check "data_store_invalid";
            if not (Ival.is_included addr_ival (Stuff.writable_interval size)) then begin
              if not (Ival.is_included addr_ival (Stuff.mmio_interval size)) then begin
                Logger.alarm "data_store_invalid";
                Logger.error "store: Address %a of type Top not included in writable range."
                  (BR.binary_pretty ~size:(ptr_size ()) ctx) addr_val
              end
            end
          end;
          Logger.result "Storing %a to data address %a"
            (Binary_Representation.binary_pretty ~size ctx) value
            (Binary_Representation.binary_pretty ~size:(ptr_size ()) ctx)
            (T {typ = addr_typ; value = addr_val});
          if Stuff.option_use_shape () && not (Ival.cardinal_is_less_than addr_ival 8) then
            raise (Failure "store: precision of address too low");
          (* Record the set of written addresses *)
          written_data_addrs := Ival.fold_int (fun integer acc ->
              Virtual_address.Set.add (Virtual_address.create @@ Z.to_int @@ integer) acc
            ) addr_ival !written_data_addrs;
          mk @@ DataReg.store ~size ctx data_mem addr_val value
        end
      end
      else begin
        let mem = match ParamReg.store ~size ctx () addr_typ addr_val value with
          | ParamReg.NotNull -> mk data_mem
          | ParamReg.MaybeNull ->
            (* Store the value at address zero. Of course, something is
             * terribly wrong here, but this is needed to preserve an important
             * invariant of the analysis: that a loop invariant may only grow.
             * That includes memory cells, and notably the memory cell zero,
             * which is the only address common to parameter and data regions
             * in our model. We have had cases when something would be written
             * to address ({0}, Top) during the first iteration and to address
             * ({0}, ZeroT) in the second. The second store was ignored,
             * breaking the invariant above crashing the fixpoint engine.
             *
             * Of course storing to address zero is probably an error, but it
             * shoud be reported by an independent alarm.
             *)
            mk @@ DataReg.store ~size ctx data_mem (zero ~size:(ptr_size ()) ctx) value
        in
        let zero = zero ctx ~size:Type.offset_size in
        let minus_one = minus_one ctx ~size:Type.offset_size in
        Logger.check "modifying_mmu_table";
        (* We check for intersection with either a pointer inside an L1 table
         * (hence a pointer to an L2 table, or a pointer to an L2 table (hence
         * a pointer to a memory page). *)
        let forbidden_store = List.exists (fun t ->
            ParamReg.may_intersect ctx addr_typ Type.(Ctypes.(ParamT (
              {descr=Ptr {pointed=t; index=Zero}; const=false; pred=Pred.True},
              zero, minus_one, 0)))
          )
          TS.non_modifiable_types
        in
        if forbidden_store then begin
          Logger.alarm "modifying_mmu_table";
          Logger.error "Storing to l1 or l2 table!"
        end;
        let rec loop mem = function
          | [] -> mem
          | (data_addr, (typ,ofs)) :: rest ->
              let sp_addr_typ = Ctypes.({descr = Ptr{pointed=typ;index=Zero};
                pred = Pred.(neq (Const Z.zero)); const = false}) in
              if ParamReg.may_intersect ctx addr_typ (Type.ParamT (sp_addr_typ,zero,minus_one,ofs)) then begin
                let data_addr_b = BR.Binary_Forward.biconst ~size:(ptr_size ())
                  (Z.of_int data_addr) ctx in
                let former_val = DataReg.load ~size ctx data_mem data_addr_b in
                let new_val = join_values ~size ctx former_val value in
                Logger.result "storing also to special address %x value %a"
                  data_addr (Binary_Representation.binary_pretty ~size ctx)
                  new_val;
                mk @@ DataReg.store ~size ctx data_mem data_addr_b new_val
              end
              else loop mem rest
        in
        loop mem TS.special_data_addrs
      end

    let store ~size ctx mem addr0 value =
      match addr0,mem with
      | Bottom,_ | _,BottomMem -> memory_empty ctx
      | T addr, M mem -> store_nonbottom ~size ctx mem.data_mem addr.typ addr.value value

    let load_nonbottom ~size ctx data_mem addr0 addr_typ addr_val =
      let open Framac_ival in
      (*let res = DataReg.load ~size ctx mem addr.value in*)
      let addr_ival = addr_val
        |> BR.Query.binary ~size:(ptr_size ()) ctx
        |> BR.Query.binary_to_ival ~signed:false ~size:(ptr_size ())
      in
      if !untyped_load then begin
          read_data_addrs := Ival.fold_int (fun integer acc ->
              Virtual_address.Set.add (Virtual_address.create @@ Z.to_int @@ integer) acc
            ) addr_ival !read_data_addrs;
        DataReg.load ~size ctx data_mem addr_val
      end
      else if Type.eq ~only_descr:false ctx addr_typ Type.Top then begin
        Logger.check "data_load_invalid";
        if Ival.is_included addr_ival (Stuff.readable_interval size) then begin
          (* Cut trace if address is too imprecise. The number 161 is quite
           * arbritrary. *)
          Logger.result "Loading from data address %a"
            (Binary_Representation.binary_pretty ~size:(ptr_size ()) ctx) addr0;
          if Stuff.option_use_shape () && not (Ival.cardinal_is_less_than addr_ival 161) then
            raise (Failure "load: precision of address too low");
          read_data_addrs := Ival.fold_int (fun integer acc ->
              Virtual_address.Set.add (Virtual_address.create @@ Z.to_int @@ integer) acc
            ) addr_ival !read_data_addrs;
          let res = DataReg.load ~size ctx data_mem addr_val in
          Logger.result "-> result: %a"
            (Binary_Representation.binary_pretty ~size ctx) res;
          res
        end else begin
          if Ival.is_included addr_ival (Stuff.mmio_interval size) then begin
            Logger.result "Loading from (presumed) MMIO address %a, unknown result"
              Ival.pretty addr_ival;
            T { typ = Type.Top; value = br_unknow ~size ctx }
          end
          else begin
            Logger.alarm "data_load_invalid";
            Logger.error "load: Address %a of type Top not included in readable range."
              (BR.binary_pretty ~size:(ptr_size ()) ctx) addr_val;
            (* Still try to return something, if the cardinal of the address is
             * reasonably small. We must try to fetch something because
             * sometimes the address is not included in the readable interval
             * because of an imprecision, but since addresses outside of that
             * interval have Bottom in them, the result won't be affected. *)
            if Ival.cardinal_is_less_than addr_ival 8 then
              let value = DataReg.load ~size ctx data_mem addr_val in
              (*
              (* Is the value bottom? *)
              let ival = value |> Binary_Representation.Query.binary ~size ctx
                |> Binary_Representation.Query.binary_to_ival ~signed:true ~size in
              if Ival.is_bottom ival then begin
                (* Returning bottom would be illogical in this case, top makes more
                 * sense *)
                Logger.warning "Result is Bottom, returning Top instead";
                T { typ = Type.Top; value = BR.Binary_Forward.bunknown ~size ctx }
              end
              else
              *)
                value
            else
              T { typ = Type.Top; value = br_unknown ~size ctx }
          end
        end
      end else begin
        ParamReg.load ~size ctx () addr_typ addr_val
      end

    let load ~size ctx mem addr =
      let value = match mem,addr with
      | BottomMem,_ | _,Bottom -> Bottom
      | M {data_mem; param_mem=()}, T {typ;value} ->
          load_nonbottom ~size ctx data_mem addr typ value
      in
      Logger.debug ~level:2 "load: returns %a" (binary_pretty ~size ctx) value;
      value

    (* Whole memory functions. *)
    let free _ = assert false
    let malloc ~id:_ ~malloc_size:_ _ = assert false
    let unknown ~level:_ ctx =
      mk @@ DataReg.initial ctx (1 lsl 30)  (* ~level ctx *)
  end
end
