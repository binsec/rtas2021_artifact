module Ctypes = Ctypes

module type S = sig
  module BR : Memory_sig.Operable_Value_Whole
  type t =
    | Top
    | ParamT of Ctypes.typ * BR.binary * BR.binary * int
    | ZeroT
    | Bottom

  val array_index_size : int
  val index_zero : BR.Scalar.Context.t -> BR.binary
  val index_minus_one : BR.Scalar.Context.t -> BR.binary
  val offset_size : int
  val eq : only_descr:bool -> BR.Scalar.Context.t -> t -> t -> bool
  val imprecise_eq : t -> t -> bool
  val type_size : Ctypes.descr -> int
  exception Global_symbol_not_found
  val symbol : BR.Scalar.Context.t -> string -> int * BR.binary
  val cond_of_pred_subdomain : size:int -> BR.Scalar.Context.t -> Ctypes.Pred.t
    -> BR.binary -> BR.Scalar.boolean
  val check_invariant_subdomain : size:int -> BR.Scalar.Context.t -> Ctypes.Pred.t
    -> BR.binary -> bool
  val use_invariant_subdomain : size:int -> BR.Scalar.Context.t -> Ctypes.Pred.t
    -> BR.binary -> BR.binary
  val assume : BR.Scalar.Context.t -> BR.Scalar.boolean -> t -> t
  val compare : t -> t -> int
  val pp : BR.Scalar.Context.t -> Format.formatter -> t -> unit
  exception Type_error of string
  val serialize : BR.Scalar.Context.t -> t -> t -> 'a BR.Scalar.in_acc -> (t, 'a) BR.Scalar.result
  val join : BR.Scalar.Context.t -> t -> t -> t
  val pointer : t -> bool
  val deref : BR.Scalar.Context.t -> t -> t * Ctypes.Pred.t
  val subtype : BR.Scalar.Context.t -> t -> t -> bool
  val may_subtype : BR.Scalar.Context.t -> t -> t -> bool
  val add_offset : BR.Scalar.Context.t -> Ctypes.typ -> BR.binary
    -> BR.binary -> int -> BR.Binary.t
    -> t
end

module Make (BR : Memory_sig.Operable_Value_Whole)
    (TS : Ctypes.Type_settings)
    (Logger : Alarm_logger.S)
= struct
  type offset = int
  let offset_size = 32

  module BR = BR

  type t =
    | Top
    | ParamT of Ctypes.typ * BR.binary * BR.binary * offset
        (** Pointer type, index of element pointed (if pointing into an
           array), offset in that element. *)
        (* invariant: the first component is a pointer type.
         * The [index] field of that pointer only has upon loading and
         * storing data, and to define ordering in the lattice of types. The
         * rest of the time, this module should only manipulate the
         * [BR.binary] index. *)
    | ZeroT
    | Bottom

  let array_index_size = 32

  let pp ctx fmt = function
    | Top -> Format.pp_print_string fmt "Top"
    | ParamT (typ, array_index, from_end, struct_offset) ->
        Format.fprintf fmt "ParamT (%a,@ %a,@ %a,@ %d)"
          Ctypes.pp typ
          (BR.binary_pretty ~size:offset_size ctx) array_index
          (BR.binary_pretty ~size:offset_size ctx) from_end
          struct_offset
    | ZeroT -> Format.pp_print_string fmt "ZeroT"
    | Bottom -> Format.pp_print_string fmt "Bottom"

  let eq ~only_descr ctx a b =
    match a,b with
    | Top,Top -> true
    | ParamT (t1,i1,fe1,j1), ParamT (t2,i2,fe2,j2) when
          Ctypes.equal ~only_descr t1 t2 && ((=) : int -> int -> bool) j1 j2
          && BR.Binary.equal i1 i2 && BR.Binary.equal fe1 fe2 ->
        true
    | ParamT (t1,i1,fe1,j1), ParamT (t2,i2,fe2,j2) ->
        if Ctypes.equal ~only_descr t1 t2 && ((=) : int -> int -> bool) j1 j2 then
          let size = offset_size in
          let i1 = BR.Query.binary_is_singleton ~size @@ BR.Query.binary ~size ctx i1 in
          let i2 = BR.Query.binary_is_singleton ~size @@ BR.Query.binary ~size ctx i2 in
          match i1,i2 with
          | None,_ | _,None -> false
          | Some x, Some y ->
              let c = Z.equal x y in
              if not c then false else
                let fe1 =
                  BR.Query.binary_is_singleton ~size @@ BR.Query.binary ~size ctx fe1 in
                let fe2 =
                  BR.Query.binary_is_singleton ~size @@ BR.Query.binary ~size ctx fe2 in
                match fe1,fe2 with
                | None,_ | _,None -> false
                | Some x, Some y -> Z.equal x y
        else false
    | _ -> false

  let imprecise_eq a b = match a,b with
    | Bottom,Bottom | ZeroT,ZeroT | Top,Top -> true
    | ParamT (t1, i1, fe1, j1), ParamT (t2, i2, fe2, j2) ->
        if Ctypes.equal ~only_descr:false t1 t2 && ((=) : int -> int -> bool) j1 j2
          then BR.Binary.equal i1 i2 (* Imprecision comes from these two calls *)
            && BR.Binary.equal fe1 fe2
          else false
    | _ -> false

  (* Size in bytes *)
  let type_size =
    let open Ctypes in function
    | Base (sz,_) | Structure {st_byte_size = Some sz;_}
    | Enum {en_byte_size = sz;_} -> sz
    | Ptr _ -> 4
    | Void | Structure { st_byte_size = None;_ }
    | Array (_, _) | Function (_, _) ->
        (* Not handled. *)
        raise (Invalid_argument "type_size")

  let compare (x : t) (y : t) =
    match x,y with
    | Top, Top -> 0
    | Top, _ -> 1
    | _, Top -> -1
    | ParamT (t1,i1,fe1,j1), ParamT (t2,i2,fe2,j2) ->
        let c = Ctypes.compare ~only_descr:false t1 t2 in
        if c <> 0 then c else
          let c = (compare : int -> int -> int) j1 j2 in
          if c <> 0 then c
          else
            let c = BR.Binary.compare i1 i2 in
            if c <> 0 then c else BR.Binary.compare fe1 fe2
    | ParamT _, _ -> 1
    | _, ParamT _ -> -1
    | ZeroT, ZeroT -> 0
    | ZeroT, _ -> 1
    | _, ZeroT -> -1
    | Bottom,Bottom -> 0

  exception Type_error of string

  let pointer = function
    | Top -> false
    | ParamT (_,_,_,_) | ZeroT | Bottom ->
        (* Invariant: [ParamT] always contains a pointer type. *)
        true

  let is_empty ~size ctx b =
    let tmp = BR.Query.binary ~size ctx b in
    BR.Query.binary_is_empty ~size tmp

  let lift_unop ~size ctx op =
    let open Ctypes.Pred in
    let open BR.Binary_Forward in
    match op with
    | Extract (index,len) -> fun x ->
      bextract ~size:len ~index ~oldsize:size ctx x
      |> buext ~size ~oldsize:len ctx

  let lift_binop ~size ctx op =
    let open Ctypes.Pred in
    let open BR.Binary_Forward in
    match op with
    | Add -> biadd ~size ctx ~nsw:false ~nuw:false
    | Sub -> bisub ~size ctx ~nsw:false ~nuw:false
    | Mul -> bimul ~size ctx ~nsw:false ~nuw:false
    | And -> band ~size ctx
    | Concat (size1,size2) -> bconcat ~size1 ~size2 ctx

  let cond_of_cmp ~size ctx cmpop v1 v2 =
    let open Ctypes.Pred in
    match cmpop with
    | Equal ->
        BR.Binary_Forward.beq ~size ctx v1 v2
    | NotEqual ->
        BR.Scalar.Boolean_Forward.not ctx @@ BR.Binary_Forward.beq ~size ctx v1 v2
    | ULt ->
        BR.Scalar.Boolean_Forward.not ctx @@ BR.Binary_Forward.biule ~size ctx v2 v1
    | SLt ->
        BR.Scalar.Boolean_Forward.not ctx @@ BR.Binary_Forward.bisle ~size ctx v2 v1
    | ULeq ->
        BR.Binary_Forward.biule ~size ctx v1 v2
    | SLeq ->
        BR.Binary_Forward.bisle ~size ctx v1 v2
    | UGeq ->
        BR.Binary_Forward.biule ~size ctx v2 v1
    | SGeq ->
        BR.Binary_Forward.bisle ~size ctx v2 v1
    | UGt ->
        BR.Scalar.Boolean_Forward.not ctx @@ BR.Binary_Forward.biule ~size ctx v1 v2
    | SGt ->
        BR.Scalar.Boolean_Forward.not ctx @@ BR.Binary_Forward.bisle ~size ctx v1 v2

  exception Global_symbol_not_found

  let rec global_constants =
    let table = ref ([] : (string * (int * BR.binary)) list) in
    let initialized = ref false in
    fun ctx ->
      if !initialized then !table
      else begin
        table := List.map (fun (name,size,pred) ->
          let b = BR.binary_unknown ~size ~level:(BR.Scalar.Context.level ctx) ctx in
          (name, (size, use_invariant_subdomain ~size ctx pred b))
        ) TS.global_symbols;
        initialized := true;
        !table
      end

  and symbol ctx s =
    try List.assoc s (global_constants ctx)
    with Not_found -> raise Global_symbol_not_found

  and binary_of_value ~size ctx v =
    let open Ctypes in
    match v with
    | Const x -> BR.Binary_Forward.biconst ~size x ctx
    | Sym s ->
        let sz,v = symbol ctx s in
        assert (sz = size);
        v

  and binary_of_expr ~size ctx self e =
    let open Ctypes.Pred in
    match e with
    | Self -> self
    | Val v -> binary_of_value ~size ctx v
    | Unop (op, e) -> lift_unop ~size ctx op (binary_of_expr ~size ctx self e)
    | Binop (op, e1, e2) -> lift_binop ~size ctx op
        (binary_of_expr ~size ctx self e1) (binary_of_expr ~size ctx self e2)

  and cond_of_pred_subdomain ~size ctx pred v =
    let open Ctypes.Pred in
    match pred with
    | True -> BR.Scalar.Boolean_Forward.true_ ctx
    | Cmp (op, e1, e2) ->
        cond_of_cmp ~size ctx op (binary_of_expr ~size ctx v e1) (binary_of_expr ~size ctx v e2)
    | And (p,q) ->
        let c1 = cond_of_pred_subdomain ~size ctx p v in
        let c2 = cond_of_pred_subdomain ~size ctx q v in
        BR.Scalar.Boolean_Forward.(&&) ctx c1 c2

  and check_invariant_subdomain ~size ctx pred v =
    let c = cond_of_pred_subdomain ~size ctx pred v in
    let is_true =
      BR.Scalar.Query.convert_to_quadrivalent (BR.Scalar.Query.boolean ctx c) in
    match is_true with
    | Quadrivalent_Lattice.True -> true
    | _ -> false (* Possibly false *)

  and use_invariant_subdomain ~size ctx pred (value : BR.binary) =
    let open Ctypes.Pred in
    match pred with
    | True -> value
    | _ ->
        BR.assume ~size ctx (cond_of_pred_subdomain ~size ctx pred
          value) value

  let index_zero ctx =
    BR.Binary_Forward.biconst ~size:array_index_size Z.zero ctx

  let index_minus_one ctx =
    BR.Binary_Forward.biconst ~size:array_index_size (Z.sub Z.zero Z.one) ctx

  let assume ctx cond t =
    match t with
    | ParamT (u, idx, fe, ofs) ->
        let new_idx = BR.assume ~size:offset_size ctx cond idx in
        let new_fe = BR.assume ~size:offset_size ctx cond fe in
        let size = array_index_size in
        if is_empty ~size ctx new_idx || is_empty ~size ctx new_fe then Bottom
        else ParamT (u, new_idx, new_fe, ofs)
    | Top | ZeroT | Bottom -> t

  (** Returns the type at a given offset of a non-scalar type.
     @param binary zero a [BR.binary] of value zero and of size
     [Type.offset_size].
     @return a triple (subtyp, idx, ofs) such that [offset] is in [subtyp] at
     index [idx] (if [subtyp] is an array) and offset [ofs].
  *)
  let type_at_offset typ offset : Ctypes.typ * int =
    Logger.debug ~level:2 "type_at_offset.@ typ =@ %a.@ offset =@ %d"
      Ctypes.pp_descr typ
      offset;
    let open Ctypes in
    match typ with
    | Structure st ->
      begin
        let members =
          List.sort_uniq (fun (x,_,_) (y,_,_) -> Pervasives.compare x y) st.st_members in
        let struct_size =
          match st.st_byte_size with Some s -> s | None -> assert false in
        let rec loop cur_field = function
          | (ofs,_,_) as field :: fields when offset >= ofs -> loop field fields
          | _ ->
              let (prec_ofs,fname,ftyp) = cur_field in
              if offset >= struct_size then begin
                let msg = Format.asprintf "Offset %d not found in structure %a."
                  offset Ctypes.pp_descr typ in
                raise (Type_error msg)
              end;
              Logger.debug ~level:1 "Access to field %s : %a (offset %d) of struct %a"
                fname
                Ctypes.pp ftyp
                prec_ofs
                Ctypes.pp_descr typ;
              ftyp, offset - prec_ofs
        in
        let fst_field = try List.hd members with Failure _ ->
          raise (Invalid_argument "type_at_offset: empty structure") in
        loop fst_field (List.tl members)
      end
    | Array (elem_typ, Some (Const nb_elem)) ->
        let elem_sz = type_size elem_typ.descr in
        let array_index = offset / elem_sz in
        (* Already emitted in Dba2Codex *)
        (*Logger.check "array_offset_access";*)
        if not (array_index >= 0 && Z.lt (Z.of_int array_index) nb_elem) then begin
          Logger.alarm "array_offset_access";
          Logger.error "@[<hov 2>Out-of-bounds access at index %d in array of \
            size %a.@]" array_index Z.pp_print nb_elem
        end else
          Logger.debug ~level:1 "@[<hov 2>Access at index %d in array of size %a@]"
            array_index Z.pp_print nb_elem
        ;
        let offset_in_elem = offset mod elem_sz in
        elem_typ,
        offset_in_elem
    | _ when offset = 0 ->
        (* This is a scalar type and does not have any fields. *)
        raise Exit
    | _ ->
        let msg = Format.asprintf "@[<hov 2>Access in type %a \
          with non-zero offset %d is not allowed@]" Ctypes.pp_descr typ offset in
        raise (Type_error msg)

  (** Like {!type_at_offset} but also takes an index and an [fe] and does
     array bound checking, if need be. This will typically be needed for the
     first loss of precision on a pointer, but not for the following steps,
     because structures do not contain arrays of unknown size directly.
     Returns optionally an alarm message if out-of-array occurred. *)
  let type_at_offset_complex ctx typ idx fe offset : Ctypes.typ * int * string option =
    let open Ctypes in
    Logger.debug ~level:1 "type_at_offset_complex.@ typ =@ %a.@ idx =@ %a.@ fe =@ %a.@ offset =@ %d"
      Ctypes.pp_descr typ
      (BR.binary_pretty ~size:offset_size ctx) idx
      (BR.binary_pretty ~size:offset_size ctx) fe
      offset;
    match typ with
    | Array (elem_typ, None) ->
        let elem_sz = type_size elem_typ.descr in
        let array_index_diff = offset / elem_sz in
        let offset_in_elem = offset mod elem_sz in
        (* Perform true euclidean division in case [offset] is negative. *)
        let array_index_diff, offset_in_elem =
          if offset < 0 then array_index_diff - 1, offset_in_elem + elem_sz
          else array_index_diff, offset_in_elem in
        Logger.debug ~level:2 "array_index_diff = %d,@ offset_in_elem =@ %d" array_index_diff offset_in_elem;
        (* I don't remember why I restricted {offset_in_elem} to zero. *)
        (*
        if offset_in_elem = 0 then begin
          Logger.result "@[<hov 2>Array access at index %d yields %a@]"
            array_index Ctypes.pp elem_typ;
          elem_typ, offset_in_elem
        end else
          let msg = Format.asprintf "@[<hov 2>Access at \
            non-congruent offset %d (elem size is %d, elem type is %a)@]"
            offset elem_sz Ctypes.pp elem_typ in
          raise (Type_error msg)
        *)
        let array_index = BR.Binary_Forward.(biadd ~size:offset_size ctx idx
          ~nsw:false ~nuw:false
          (biconst ~size:offset_size (Z.of_int array_index_diff) ctx)) in
        let index_zero = check_invariant_subdomain ~size:array_index_size ctx
          (Ctypes.Pred.eq (Const Z.zero)) array_index in
        let alarm =
          if not index_zero then begin
            (* Already emitted in Dba2Codex *)
            (*Logger.check "array_offset_access";*)
            let msg = Format.asprintf "Access at non-zero index %a in array of unknown size, \
              assuming safe" (BR.binary_pretty ~size:offset_size ctx) array_index in
            Some msg
          end else
            None
        in
        Logger.debug ~level:1 "@[<hov 2>Array access at index %a yields %a@]"
          (BR.binary_pretty ~size:offset_size ctx) array_index Ctypes.pp elem_typ;
        elem_typ, offset_in_elem, alarm
    | Array (elem_typ, Some nb_elem) ->
        let elem_sz = type_size elem_typ.descr in
        let array_index_diff = offset / elem_sz in
        let offset_in_elem = offset mod elem_sz in
        (* Perform true euclidean division in case [offset] is negative. *)
        let array_index_diff, offset_in_elem =
          if offset < 0 then array_index_diff - 1, offset_in_elem + elem_sz
          else array_index_diff, offset_in_elem in
        Logger.debug ~level:2 "array_index_diff = %d,@ offset_in_elem =@ %d" array_index_diff offset_in_elem;
        let array_index = BR.Binary_Forward.(biadd ~size:offset_size ctx idx
          ~nsw:false ~nuw:false
          (biconst ~size:offset_size (Z.of_int array_index_diff) ctx)) in
        let new_fe = BR.Binary_Forward.(biadd ~size:offset_size ctx fe
          ~nsw:false ~nuw:false
          (biconst ~size:offset_size (Z.of_int array_index_diff) ctx)) in
        let lt_size =
          check_invariant_subdomain ~size:array_index_size ctx
            Ctypes.Pred.(slt (Const Z.zero)) new_fe ||
          (* [i <u s] implies that [i <s s], provided that [s >=s 0].
           * Here we assume [s >=s 0].
           * [i <u s] also implies [i <=s s], which is what we
           * need to verify. Incidentally, [i <u s] is the
           * condition that will be the easiest to verify with our
           * constraint domain and hypotheses on array sizes. *)
          (* XXX: factorize this by using [valid] here. *)
          check_invariant_subdomain ~size:array_index_size ctx 
            Ctypes.Pred.(ult nb_elem) array_index
        in
        let geq_zero =
          check_invariant_subdomain ~size:array_index_size ctx
            Ctypes.Pred.(sgeq (Const Z.zero)) array_index in
        let alarm =
          if not (geq_zero && lt_size) then begin
            let msg = Format.asprintf "@[<hov 2>Out-of-bounds access at index %a in array of \
              size %a.@]" (BR.binary_pretty ~size:offset_size ctx) array_index
              Ctypes.pp_value nb_elem in
            Some msg
          end else begin
            Logger.debug ~level:1 "@[<hov 2>Access at index %a in array of size %a@]"
              (BR.binary_pretty ~size:offset_size ctx) array_index
              Ctypes.pp_value nb_elem;
            None
          end
        in
        elem_typ,
        offset_in_elem,
        alarm
    | _ ->
        let typ, ofs = type_at_offset typ offset in
        typ, ofs, None

  let coalesce_bottom ctx t =
    match t with
    | ParamT (_,idx,fe,_) ->
        let size = array_index_size in
        if is_empty ~size ctx idx || is_empty ~size ctx fe then Bottom
        else t
    | Bottom | ZeroT | Top -> t

  let focus_idx ctx upper_bound index =
    let size = array_index_size in
    let open Ctypes in
    match index, upper_bound with
    | Zero, None ->
        BR.Binary_Forward.biconst ~size Z.zero ctx,
        use_invariant_subdomain ~size ctx Pred.(slt (Const Z.zero)) @@
          BR.binary_unknown ~size ~level:(BR.Scalar.Context.level ctx) ctx
    | Zero, Some upper ->
        let zero = BR.Binary_Forward.biconst ~size Z.zero ctx in
        zero,
        BR.Binary_Forward.bisub ~size ctx ~nsw:false ~nuw:false zero @@
          binary_of_value ~size ctx upper
    | ValidUnknown, None ->
        use_invariant_subdomain ~size ctx Pred.(sgeq (Const Z.zero)) @@
        BR.binary_unknown ~size ~level:(BR.Scalar.Context.level ctx) ctx,
        use_invariant_subdomain ~size ctx Pred.(slt (Const Z.zero)) @@
        BR.binary_unknown ~size ~level:(BR.Scalar.Context.level ctx) ctx
    | ValidUnknown, Some upper ->
        let upper = binary_of_value ~size ctx upper in
        Logger.debug ~level:3 "upper = %a" (BR.binary_pretty ~size ctx) upper;
        let idx = BR.binary_unknown ~size ~level:(BR.Scalar.Context.level ctx) ctx in
        Logger.debug ~level:3 "idx = %a" (BR.binary_pretty ~size ctx) idx;
        let idx = use_invariant_subdomain ~size ctx Pred.(sgeq (Const Z.zero)) idx in
        Logger.debug ~level:3 "idx = %a" (BR.binary_pretty ~size ctx) idx;
        let idx = BR.assume ~size ctx (BR.Scalar.Boolean_Forward.not ctx @@
            BR.Binary_Forward.bisle ~size ctx upper idx) idx in
        Logger.debug ~level:3 "idx = %a" (BR.binary_pretty ~size ctx) idx;
        let fe = BR.Binary_Forward.bisub ~size ctx ~nsw:false ~nuw:false idx upper in
        Logger.debug ~level:3 "fe = %a" (BR.binary_pretty ~size ctx) fe;
        let fe = use_invariant_subdomain ~size ctx Pred.(slt (Const Z.zero)) fe in
        Logger.debug ~level:3 "fe = %a" (BR.binary_pretty ~size ctx) fe;
        idx, fe
    | PastEnd, None ->
        use_invariant_subdomain ~size ctx Pred.(sgt (Const Z.zero)) @@
          BR.binary_unknown ~size ~level:(BR.Scalar.Context.level ctx) ctx,
        index_zero ctx
    | PastEnd, Some upper ->
        let upper = binary_of_value ~size ctx upper in
        let idx = use_invariant_subdomain ~size ctx Pred.(sgt (Const Z.zero)) @@
          BR.binary_unknown ~size ~level:(BR.Scalar.Context.level ctx) ctx in
        let idx = BR.assume ~size ctx (BR.Binary_Forward.beq ~size ctx
          upper idx) idx in
        idx,
        index_zero ctx

  let rec add_offset ctx typ old_index old_fe old_offset (offset : BR.binary) =
    let open Framac_ival in
    let open Ctypes in
    match typ.descr with
    | Void ->
        raise (Type_error "add_offset: Cannot guess size of pointed type in case of void*");
    | Ptr {pointed = pointed_t;_} ->
      begin match pointed_t.descr with
      | Array (elem_t, size) ->
          let ofs_size = offset_size in
          let elem_sz = type_size elem_t.descr in
          let elem_sz_b = BR.Binary_Forward.biconst ~size:ofs_size
            (Z.of_int elem_sz) ctx in
          let delta = BR.Binary_Forward.(biadd ~size:ofs_size ctx ~nsw:false ~nuw:false
            offset
            (biconst ~size:ofs_size (Z.of_int old_offset) ctx)) in
          let index_diff = BR.Binary_Forward.bisdiv ~size:ofs_size ctx delta elem_sz_b in
          let new_offset = BR.Binary_Forward.bismod ~size:ofs_size ctx delta elem_sz_b in
          let new_offset_ival = BR.Query.binary ~size:ofs_size ctx new_offset
            |> BR.Query.binary_to_ival ~signed:true ~size:ofs_size in
          let zero = BR.Binary_Forward.biconst ~size:ofs_size Z.zero ctx in
          if not (Ival.is_singleton_int new_offset_ival) then begin
            Logger.check "array_offset_add";
            Logger.alarm "array_offset_add";
            let msg =
              let size = ofs_size in
              Format.asprintf "@[<hov 2>Dereference with imprecise offset \
                (index diff %a, new offset in type %a) is not allowed@]"
                (BR.binary_pretty ctx ~size) index_diff
                (BR.binary_pretty ctx ~size) new_offset in
            raise (Type_error msg)
          end;
          let new_offset = Ival.project_int new_offset_ival |> Z.to_int in
          (* If [new_offset] represents a negative integer (on 32 bits),
           * first compute that negative integer. Then, it means that [delta]
           * was negative, and we adjust the values to reflect a real
           * euclidean division. *)
          (* XXX: this code only works in a 64-bit OCaml runtime. *)
          let new_offset, index_diff =
            if new_offset >= 1 lsl 31 then
              elem_sz + (new_offset - (1 lsl 32)), BR.Binary_Forward.(bisub ~size:ofs_size
                ~nsw:false ~nuw:false ctx index_diff (biconst ~size:ofs_size Z.one ctx))
            else new_offset, index_diff
          in
          let index_diff_zero = BR.Binary_Forward.beq ~size:ofs_size ctx
            index_diff zero |> BR.Scalar.Query.boolean ctx
            |> BR.Scalar.Query.convert_to_quadrivalent in
          let index_diff_zero = match index_diff_zero with
            | Quadrivalent_Lattice.True -> true
            | _ -> false
          in
          (* In this function, we allow the new pointer to point past the end
           * of the array, but no further. I.e., if ar is of size s, the
           * pointer may point between ar[s] (included) and ar[s+1]
           * (excluded), but no further, without triggering an alarm. Of
           * course, it is still an invalid pointer, and trying to
           * dereference it will cause an alarm. *)
          (* We assume that the addition does not wrap. If it does, this will
           * be caught by the "array_offset_add" alarm below. *)
          let new_index = BR.Binary_Forward.biadd ~size:ofs_size ~nsw:true
            ~nuw:false ctx old_index index_diff in
          let new_fe = BR.Binary_Forward.biadd ~size:ofs_size ~nsw:false
            ~nuw:false ctx old_fe index_diff in
          if index_diff_zero then
            coalesce_bottom ctx @@ ParamT (typ, new_index, new_fe, new_offset)
          else begin
            Logger.check "array_offset_add";
            let c1 = check_invariant_subdomain ~size:array_index_size ctx
              Pred.(sgeq (Const Z.zero)) new_index in
            (* Just past the end is allowed, so this is Leq and not Lt *)
            let c2 =
              check_invariant_subdomain ~size:array_index_size ctx
                Pred.(sleq (Const Z.zero)) new_fe
              || match size with
                 | Some s ->
                     (* [i <u s] implies that [i <s s], provided that [s >=s 0].
                      * Here we assume [s >=s 0].
                      * [i <u s] also implies [i <=s s], which is what we
                      * need to verify. Incidentally, [i <u s] is the
                      * condition that will be the easiest to verify with our
                      * constraint domain and hypotheses on array sizes. *)
                     check_invariant_subdomain ~size:array_index_size ctx Pred.(ult s) new_index
                     (* If that it false, we will also try to see if this is
                      * true (useful only for [s_stack_descriptor.size]. *)
                     || check_invariant_subdomain ~size:array_index_size ctx (Pred.eq s) @@
                        BR.Binary_Forward.biadd ~size:array_index_size ~nsw:false ~nuw:false ctx new_index @@
                        BR.Binary_Forward.biconst ~size:array_index_size Z.one ctx
                 | None -> false
            in
            if c1 && c2
            then coalesce_bottom ctx @@ ParamT (typ, new_index, new_fe, new_offset)
            else begin
              Logger.alarm "array_offset_add";
              Logger.error "Array index %a (fe %a) possibly out of bounds."
                (BR.binary_pretty ctx ~size:ofs_size) new_index
                (BR.binary_pretty ctx ~size:ofs_size) new_fe;
              let new_index_sng = BR.Query.binary_is_singleton ~size:offset_size @@
                BR.Query.binary ~size:offset_size ctx new_index in
              let new_fe_sng = BR.Query.binary_is_singleton ~size:offset_size @@
                BR.Query.binary ~size:offset_size ctx new_fe in
              let new_index', new_fe' =
                (* We assume the index is in the bounds or just past the end,
                 * except if its cardinal is one, in which case the problem
                 * is probably that this code is not well understood (e.g.
                 * hardcoded index -1 at 1200535c. *)
                if (new_index_sng, new_fe_sng) = (None,None) then begin
                  Logger.warning "Assuming index is in bounds.";
                  use_invariant_subdomain ~size:array_index_size ctx Pred.(sgeq (Const Z.zero))
                    new_index,
                  use_invariant_subdomain ~size:array_index_size ctx Pred.(sleq (Const Z.zero))
                    new_fe
                end else new_index, new_fe
              in
              coalesce_bottom ctx @@ ParamT (typ, new_index', new_fe', new_offset)
            end
          end
      | stripped_pointed_t ->
          let t_sz = type_size stripped_pointed_t in
          let ofs_size = offset_size in
          let t_sz_b = BR.Binary_Forward.biconst ~size:ofs_size (Z.of_int t_sz) ctx in
          let zero = BR.Binary_Forward.biconst ~size:ofs_size Z.zero ctx in
          let total_b =
            let size = ofs_size in BR.Binary_Forward.(
              biadd ~size ~nsw:false ~nuw:false ctx offset @@
              biadd ~size ~nsw:false ~nuw:false ctx (biconst ~size (Z.of_int old_offset) ctx) @@
              bimul ~size ~nsw:false ~nuw:false ctx old_index t_sz_b
            ) in
          let total_ival = BR.Query.binary ~size:ofs_size ctx total_b
            |> BR.Query.binary_to_ival ~signed:true ~size:ofs_size in
          if not (Ival.is_singleton_int total_ival) then begin
            Logger.debug ~level:3 "Imprecise offset. Attempt to weaken the type \
              to find an array...";
            (* Can be improved. Notably, this weakening could be performed more
             * than once and only give up when reaching Top. *)
            let give_up () =
              let msg = Format.asprintf "@[<hov 2>Adding imprecise offset %a is \
                not  allowed@]" (BR.binary_pretty ctx ~size:ofs_size) total_b in
              raise (Type_error msg)
            in
            match Ival.min_int total_ival with
            | Some min ->
                let new_t, _ = type_at_offset stripped_pointed_t (Z.to_int min) in
                begin match new_t.Ctypes.descr with
                | Ctypes.(Array (_, ar_size)) ->
                    let new_ofs = BR.Binary_Forward.(bisub ~size:ofs_size ctx ~nsw:false ~nuw:false total_b @@ biconst ~size:ofs_size min ctx) in
                    let idx, fe = focus_idx ctx ar_size Ctypes.Zero in
                    let new_t = Ptr{pointed=new_t;index=Zero} in
                    let new_t = {descr=new_t;const=false;pred=Pred.True} in
                    add_offset ctx new_t idx fe 0 new_ofs
                | _ -> give_up ()
                end
            | None -> give_up ()
          end else begin
            let total = Ival.project_int total_ival |> Z.to_int in
            let lesser_sz = BR.Scalar.Boolean_Forward.not ctx @@
              BR.Binary_Forward.bisle ~size:ofs_size ctx t_sz_b total_b in
            let geq_zero = BR.Binary_Forward.bisle ~size:ofs_size ctx zero total_b in
            let in_bounds = BR.Scalar.Boolean_Forward.(&&) ctx geq_zero lesser_sz in
            let in_bounds = BR.Scalar.Query.(boolean ctx in_bounds |> convert_to_quadrivalent) in
            begin match in_bounds with
            | Quadrivalent_Lattice.True -> coalesce_bottom ctx @@
                ParamT (typ, old_index, old_fe, total)
            | _ ->
                let msg = Format.asprintf "Offset %a is out of the bounds of the pointed type"
                  (BR.binary_pretty ~size:ofs_size ctx) total_b in
                raise (Type_error msg)
            end
          end
    end
    | _ -> assert false

  let tmp_count = ref 0

  type inclusion = LeftIncluded | RightIncluded | NoInclusion

  (* Also returns a boolean telling whether [p2] is included in [p1]. *)
  let join_pred p1 p2 =
    if Ctypes.Pred.(equal p1 True) || p1 = p2 then true, p1
    else false, Ctypes.Pred.True

  let serialize ctx t u (included, acc) =
    incr tmp_count;
    let count = !tmp_count in
    Logger.debug ~level:2 "serialize no. %d: %a %a" count (pp ctx) t (pp ctx) u;
    Logger.debug ~level:2 "serialize no. %d: initial included = %b" count included;
    match t, u with
    | Bottom, Bottom ->
        BR.Scalar.Result (included, acc, fun output -> Bottom, output)
    | Bottom, ZeroT ->
        Logger.debug ~level:2 "serialize no. %d: inclusion is false" count;
        Logger.debug ~level:2 "serialize no. %d returns ZeroT" count;
        BR.Scalar.Result (false, acc, fun output -> ZeroT, output)
    | ZeroT, (Bottom|ZeroT) ->
        Logger.debug ~level:2 "serialize no. %d: inclusion is true" count;
        Logger.debug ~level:2 "serialize no. %d returns ZeroT" count;
        BR.Scalar.Result (included, acc, fun output -> ZeroT, output)
    | Top,Top ->
        Logger.debug ~level:2 "serialize no. %d: inclusion is true" count;
        Logger.debug ~level:2 "serialize no. %d returns Top" count;
        BR.Scalar.Result (included, acc, fun output -> Top, output)
    | Top, _ ->
        Logger.debug ~level:2 "serialize no. %d: inclusion is true" count;
        Logger.debug ~level:2 "serialize no. %d returns Top" count;
        BR.Scalar.Result (included, acc, fun output -> Top, output)
    | _, Top ->
        Logger.debug ~level:2 "serialize no. %d: inclusion is false" count;
        Logger.debug ~level:2 "serialize no. %d returns Top" count;
        BR.Scalar.Result (false, acc, fun output -> Top, output)
    | (Bottom | ZeroT), ParamT (t,idx,fe,ofs) ->
        let idx_dummy = BR.binary_empty ~size:array_index_size ctx in
        let fe_dummy = BR.binary_empty ~size:array_index_size ctx in
        let BR.Scalar.Result (_, acc, deserialize_idx) = BR.serialize
          ctx ~size:array_index_size idx_dummy idx (true,acc) in
        let BR.Scalar.Result (_, acc, deserialize_fe) = BR.serialize
          ctx ~size:array_index_size fe_dummy fe (true,acc) in
        BR.Scalar.Result (false, acc, fun output ->
          let new_fe, output = deserialize_fe output in
          let new_idx, output = deserialize_idx output in
          ParamT (t, new_idx, new_fe, ofs), output
        )
    | ParamT (t,idx,fe,ofs), (Bottom | ZeroT) ->
        let idx_dummy = BR.binary_empty ~size:array_index_size ctx in
        let fe_dummy = BR.binary_empty ~size:array_index_size ctx in
        let BR.Scalar.Result (_, acc, deserialize_idx) = BR.serialize
          ctx ~size:array_index_size idx idx_dummy (true,acc) in
        let BR.Scalar.Result (_, acc, deserialize_fe) = BR.serialize
          ctx ~size:array_index_size fe fe_dummy (true,acc) in
        BR.Scalar.Result (included, acc, fun output ->
          let new_fe, output = deserialize_fe output in
          let new_idx, output = deserialize_idx output in
          ParamT (t, new_idx, new_fe, ofs), output
        )
    (* This is only an optimization. Is it safe? Apparently not, got
     * [Never_refined] exceptions using it. *)
    (*
    | ParamT _, ParamT _ when eq ctx t u ->
        BR.Scalar.Result (acc, fun output -> t, output)
    *)
    | Ctypes.(
      ParamT ({descr=Ptr{pointed = ({descr=Array (elem_t, m_sz1);_} as t_ar); index=index_t}; pred=pred_t; _}, t_idx, t_fe, t_ofs),
          ParamT ({descr=Ptr{pointed = ({descr=Array (elem_u, m_sz2);_} as u_ar); index=index_u}; pred=pred_u; _}, u_idx, u_fe, u_ofs))
          when Ctypes.equal ~only_descr:false elem_t elem_u
            && t_ofs = u_ofs ->
        let inc0, new_pred = join_pred pred_t pred_u in
        Logger.debug ~level:2 "predicate inclusion: %B" inc0;
        (* Join indices *)
        Logger.debug ~level:2 "joining of indices...";
        let map2 f default x y = match x,y with
          | Some x, Some y -> f x y
          | _ -> default
        in
        (* We don't want to test inclusion of indices, but compare their minima or maxima. *)
        let wrap_z = function
        | Some z -> Some (if Z.geq z (Z.shift_left Z.one 31) then Z.sub z (Z.shift_left Z.one 32)
            else z)
        | None -> None
        in
        let BR.Scalar.Result (_, acc, deserialize_idx) = BR.serialize ~size:offset_size
          ctx t_idx u_idx (true, acc) in
        let open Framac_ival in
        let inc1 =
          let idx_t = BR.Query.binary ~size:offset_size ctx t_idx
            |> BR.Query.binary_to_ival ~signed:true ~size:offset_size in
          let idx_u = BR.Query.binary ~size:offset_size ctx u_idx
            |> BR.Query.binary_to_ival ~signed:true ~size:offset_size in
          Logger.debug ~level:2 "case else with idx_t = %a@ and idx_u = %a" Ival.pretty idx_t Ival.pretty idx_u;
          map2 Z.leq false (Ival.min_int idx_t |> wrap_z) (Ival.min_int idx_u |> wrap_z)
        in
        Logger.debug ~level:2 "index inclusion: %B" inc1;
        let BR.Scalar.Result (_, acc, deserialize_fe) = BR.serialize ~size:offset_size
          ctx t_fe u_fe (true, acc) in
        let inc2 =
          let fe_t = BR.Query.binary ~size:offset_size ctx t_fe
            |> BR.Query.binary_to_ival ~signed:true ~size:offset_size in
          let fe_u = BR.Query.binary ~size:offset_size ctx u_fe
            |> BR.Query.binary_to_ival ~signed:true ~size:offset_size in
          map2 Z.leq false (Ival.max_int fe_u |> wrap_z) (Ival.max_int fe_t |> wrap_z)
        in
        Logger.debug ~level:2 "fe inclusion: %B" inc2;
        (* XXX: compute new size. Is this sound? *)
        let inc3, m_new_size = let open Ctypes in
        begin match m_sz1, m_sz2 with
        | Some (Const sz1), Some (Const sz2) when sz1 = sz2 ->
            true, Some (Const sz1)
        | Some (Sym s1), Some (Sym s2) when s1 = s2 ->
            true, Some (Sym s1)
        | _ -> false, None
        end in
        Logger.debug ~level:2 "size inclusion: %B" inc3;
        let inc4, new_const =
          let open Ctypes in
          not u_ar.const || t_ar.const,
          t_ar.const || u_ar.const in
        Logger.debug ~level:2 "const inclusion: %B" inc4;
        let included' = inc0 && inc1 && inc2 && inc3 && inc4 in
        Logger.debug ~level:2 "inclusion is %B" included';
        let included = included && included' in
        BR.Scalar.Result (included, acc, fun output ->
          let new_fe, output = deserialize_fe output in
          let new_index, output = deserialize_idx output in
          let ret =
            if m_new_size = None
            then Ctypes.(
              ParamT ({descr = Ptr {pointed = {elem_t with const = new_const}; index = Zero};
                  const = false; pred = new_pred },
                index_zero ctx, index_minus_one ctx, t_ofs)
              )
            else Ctypes.(
              ParamT ({descr = Ptr {pointed = ({descr=Array (elem_t, m_new_size);
                                     const = new_const; pred = Pred.True });
                                    index = join_indices index_t index_u };
                       const = false; pred = new_pred },
                new_index, new_fe, t_ofs))
          in
          Logger.debug ~level:2 "serialize no. %d returns %a%!" count (pp ctx) ret;
          ret, output)
    | Ctypes.(
          ParamT ({descr=Ptr {pointed=pt;index=i_t};pred=pred_t;_}, t_idx, t_fe, t_ofs),
          ParamT ({descr=Ptr {pointed=pu;index=i_u};pred=pred_u;_}, u_idx, u_fe, u_ofs))
          when Ctypes.(equal_descr ~only_descr:false pt.descr pu.descr)
          && t_ofs = u_ofs ->
        assert (i_t = i_u);
        let inc0, new_pred = join_pred pred_t pred_u in
        Logger.debug ~level:2 "predicate inclusion: %B" inc0;
        Logger.debug ~level:2 "joining of indices...";
        (* Join indices. This should only happen between indices zero and
         * bottom, since it is an invariant that all indices of non-array
         * types should be included in the singleton zero. *)
        let BR.Scalar.Result (inc1, acc, deserialize_idx) = BR.serialize ~size:offset_size
          ctx t_idx u_idx (true,acc) in
        Logger.debug ~level:2 "idx inclusion: %B" inc1;
        let BR.Scalar.Result (inc2, acc, deserialize_fe) = BR.serialize ~size:offset_size
          ctx t_fe u_fe (true,acc) in
        Logger.debug ~level:2 "fe inclusion: %B" inc2;
        let inc3, new_const = Ctypes.(not pu.const || pt.const, pt.const || pu.const) in
        Logger.debug ~level:2 "const inclusion: %B" inc3;
        let included' = inc0 && inc1 && inc2 && inc3 in
        Logger.debug ~level:2 "inclusion is %B" included';
        let included = included && included' in
        BR.Scalar.Result (included, acc, fun output ->
          let new_fe, output = deserialize_fe output in
          let new_index, output = deserialize_idx output in
          let ret =
            Ctypes.(ParamT ({descr=Ptr {pointed=pt;index=i_t}; const=new_const; pred=new_pred}, new_index, new_fe, t_ofs))
          in
          Logger.debug ~level:2 "serialize no. %d returns %a%!" count (pp ctx) ret;
          ret, output)
    (* Here we ignore the indices (should be zero) *)
    | ParamT (ptr_t, idx_t, fe_t, t_ofs), ParamT (ptr_u, idx_u, fe_u, u_ofs) ->
      begin try
        Logger.check "serialize";
        let inc0, new_pred = Ctypes.(join_pred ptr_t.pred ptr_u.pred) in
        Logger.debug ~level:2 "predicate inclusion: %B" inc0;
        let pointed_t, pointed_u = Ctypes.(match ptr_t.descr, ptr_u.descr with
        | Ptr {pointed=t;_}, Ptr {pointed=u;_} -> t, u
        | _ -> assert false) in
        (* The lattice {Ctypes.t} has a tree structure (once excluded
         * {ZeroT}), so all we need to do to find the lub is to find their
         * lowest common ancestor. We implement this algorithm in a naive
         * way: compute the list of parents up to the root for {t} and {u},
         * then reverse these two lists and take the last element of their
         * common prefix. *)
        (* Computes list of parents in descending order, including {x},
         * excluding {Top}. *)
        let parents (x : Ctypes.typ) idx fe ofs =
          let rec aux x ofs =
            try
              let parent_t, parent_ofs = type_at_offset x.Ctypes.descr ofs in
              (x,ofs) :: aux parent_t parent_ofs
            with Exit -> [(x, ofs)]
          in
          (try
            let parent_t, parent_ofs, alarm =
              type_at_offset_complex ctx x.Ctypes.descr idx fe ofs in
            let res = match alarm with
            | Some msg ->
                Logger.warning "Invalid pointer has no parent. Origin: %s" msg;
                [(x, ofs)]
            | None -> (x, ofs) :: aux parent_t parent_ofs
            in res
          with Exit -> [(x, ofs)])
        in
        let parents_t = parents pointed_t idx_t fe_t t_ofs in
        Logger.debug ~level:2 "parents_t = [%a]" (Format.pp_print_list (fun fmt (t,ofs) -> Format.fprintf fmt "(%a,%d)" Ctypes.pp t ofs)) parents_t;
        let parents_u = parents pointed_u idx_u fe_u u_ofs in
        Logger.debug ~level:2 "parents_t = [%a]" (Format.pp_print_list (fun fmt (t,ofs) -> Format.fprintf fmt "(%a,%d)" Ctypes.pp t ofs)) parents_u;
        let rec walk prev l1 l2 = match l1,l2 with
          | [], _ ->
              prev, LeftIncluded
          | _, [] ->
              prev, RightIncluded
          | (x,ox) :: r1, (y,oy) :: r2 ->
              if Ctypes.equal ~only_descr:false x y && ((=) : int -> int -> bool) ox oy
              then walk (Some (x,ox)) r1 r2
              else prev, NoInclusion
        in
        (match walk None (List.rev parents_t) (List.rev parents_u) with
        | Some (t,o), inclusion ->
            let inc1, new_const = Ctypes.(not pointed_u.const || pointed_t.const, pointed_t.const || pointed_u.const) in
            Logger.debug ~level:2 "const inclusion: %B" inc1;
            let res = ParamT (Ctypes.(
              {descr=Ptr {pointed=t;index=Zero (* XXX: is it ok? *)}; const=new_const;
               pred=new_pred}), index_zero ctx, index_minus_one ctx, o) in
            let inc2 = inclusion = LeftIncluded in
            Logger.debug ~level:2 "type tree inclusion: %B" inc1;
            let included = included && inc0 && inc1 && inc2 in
            Logger.debug ~level:2 "inclusion is %B" included;
            Logger.debug ~level:2 "serialize no. %d returns %a" count (pp ctx) res;
            BR.Scalar.Result (included, acc, fun output -> res, output)
        | None, _ ->
            Logger.debug ~level:2 "serialize no. %d: inclusion is false" count;
            Logger.debug ~level:2 "serialize no. %d returns Top" count;
            BR.Scalar.Result (false, acc, fun output -> Top, output)
        )
      with Type_error s ->
        Logger.alarm "serialize";
        Logger.error "Type error during serialize no. %d: %s" count s;
        Logger.debug ~level:2 "serialize no. %d: inclusion is false" count;
        BR.Scalar.Result (false, acc, fun output -> Top, output)
      end

  let join ctx t u =
    let BR.Scalar.Result (_, acc, deserialize) =
      serialize ctx t u (true, BR.Scalar.empty_tuple) in
    let out = BR.Scalar.typed_nondet2 ctx acc in
    fst @@ deserialize out

  (** Returns a (type, predicate) pair. The type is the result of
     dereferencing the type in argument. The predicate represents the possible
     numeric constraints associated with the type returned, i.e.
     invariants that must hold on the associated value. *)
  let deref ctx typ : t * Ctypes.Pred.t =
    match typ with
    | Bottom ->
        let msg = Format.asprintf "Dereferencing Bottom." in
        raise (Type_error msg)
    | ZeroT ->
        let msg = Format.asprintf "Dereferencing ZeroT." in
        raise (Type_error msg)
    | Top ->
        let msg = Format.asprintf "Dereferencing Top." in
        raise (Type_error msg)
    | ParamT (ptr, idx, fe, offset) ->
        (* Note that we do not consider the predicate of the argument. This
         * should probably be done elsewhere. *)
        (* XXX: consider the const qualifier *)
        (match ptr.Ctypes.descr with
        | Ctypes.(Ptr {pointed={descr=Void;_};_}) ->
            (* "pointer to void", i.e. to any type *)
            Logger.debug ~level:1 "Dereferencing pointer to unknown type (void*).";
            Top, Ctypes.Pred.True
        | Ctypes.Ptr {pointed=pointed_t;_} ->
            Logger.debug ~level:1 "Dereferencing type %a" (pp ctx) typ;
            (* If the pointed type is not itself a pointer, try to lose
             * precision to see if we can get a pointer. Otherwise, return
             * {Type.Top}. Consider e.g. if {pointed_t} is an array of
             * pointer types {p}, then this method enables to return a
             * {Type.ParamT} of {p}s rather than {Type.Top}. *)
            let rec loop (t : Ctypes.typ) ofs : t * Ctypes.Pred.t =
              loop_unless_ptr type_at_offset t ofs
            and loop_unless_ptr f t ofs =
              match Ctypes.(t.descr) with
              | Ctypes.(Ptr {pointed={descr=Array (_,ar_size);_};index}) ->
                  (* Here we extract the index from the type into a
                   * [BR.binary]. We "focus" it, so to say *)
                  let idx, fe = focus_idx ctx ar_size index in
                  let res = coalesce_bottom ctx @@ ParamT (t, idx, fe, ofs) in
                  Logger.debug ~level:1 "Dereferenced type: %a" (pp ctx) res;
                  res, t.Ctypes.pred
              | Ctypes.(Ptr _) ->
                  let idx = index_zero ctx in
                  let fe = index_minus_one ctx in
                  let res = ParamT (t, idx, fe, ofs) in
                  Logger.debug ~level:1 "Dereferenced type: %a" (pp ctx) res;
                  res, t.Ctypes.pred
              | _ -> begin try
                  let new_t, new_ofs = f t.Ctypes.descr ofs in
                  loop new_t new_ofs
                with Exit ->
                  (* Apply potential invariants for this non-pointer type *)
                  Top, t.Ctypes.pred
                end
            in
            begin match Ctypes.(pointed_t.descr) with
            | Ctypes.(Ptr {pointed={descr=Array (_,ar_size);_};index}) ->
                (* Here we extract the index from the type into a
                 * [BR.binary]. We "focus" it, so to say *)
                let idx, fe = focus_idx ctx ar_size index in
                let res = coalesce_bottom ctx @@ ParamT (pointed_t, idx, fe, offset) in
                Logger.debug ~level:1 "Dereferenced type: %a" (pp ctx) res;
                res, pointed_t.Ctypes.pred
            | Ctypes.(Ptr _) ->
                let idx = index_zero ctx in
                let fe = index_minus_one ctx in
                let res = ParamT (pointed_t, idx, fe, offset) in
                Logger.debug ~level:1 "Dereferenced type: %a" (pp ctx) res;
                res, pointed_t.Ctypes.pred
            | _ -> begin try
                let new_t, new_ofs, alarm =
                  type_at_offset_complex ctx pointed_t.Ctypes.descr
                  idx fe offset in
                let () = match alarm with
                | Some msg ->
                    Logger.alarm "array_offset_access";
                    Logger.error "%s" msg
                | None -> () in
                loop new_t new_ofs
              with Exit ->
                (* Apply potential invariants for this non-pointer type *)
                Top, pointed_t.Ctypes.pred
              end
            end
        | _ ->
            (* Invariant "only pointer types in {ParamT}" violated *)
            assert false
        )

  let subtype ctx t u =
    (* Careful to the order of t and u: [included] is true iff u includes t. *)
    let BR.Scalar.Result (included,_,_) =
      serialize ctx u t (true, BR.Scalar.empty_tuple) in
    included

  let may_subtype ctx t u =
    let open Ctypes in
    let zero = index_zero ctx in
    let exists_valid_index ctx idx fe =
      (* Is there at least one valid index in the concretization? *)
      let cond = BR.Binary_Forward.(BR.Scalar.Boolean_Forward.(&&) ctx
        (bisle ctx ~size:offset_size zero idx)
        (BR.Scalar.Boolean_Forward.not ctx @@ bisle ~size:offset_size ctx zero fe)
      ) in
      BR.Scalar.Query.(convert_to_quadrivalent @@ boolean ctx cond)
    in
    match t,u with
    | Bottom,_ -> true
    | _, Bottom -> false
    | (ZeroT | ParamT _ | Top), Top -> true
    | Top, (ZeroT | ParamT _) -> false
    | ParamT ({descr = Ptr{pointed = {descr=Array (elem_t, _);_};_};_} as t_descr, idx_t, fe_t, ofs_t),
      ParamT ({descr = Ptr{pointed = {descr=Array (elem_u, _);_};_};_}  as u_descr, idx_u, fe_u, ofs_u) ->
        (* Return true if there may be at least one valid index in each
         * concretization and the subtyping between pointers to elements
         * holds. *)
        let ret = match exists_valid_index ctx idx_t fe_t with
        | Quadrivalent_Lattice.False -> false
        | _ ->
            let ret2 = match exists_valid_index ctx idx_u fe_u with
            | Quadrivalent_Lattice.False -> false
            | _ ->
                let new_t = ParamT ({t_descr with
                    descr = Ptr{pointed = elem_t; index = Zero} },
                  zero, index_minus_one ctx, ofs_t) in
                let new_u = ParamT ({u_descr with
                    descr = Ptr{pointed = elem_u; index = Zero} },
                  zero, index_minus_one ctx, ofs_u) in
                subtype ctx new_t new_u
            in ret2
        in ret
    | ParamT ({descr = Ptr{pointed = {descr=Array (elem_typ, _);_};_}; pred; const},
          idx, fe, ofs),
      ParamT (_,_,_,_) ->
        let ret = match exists_valid_index ctx idx fe with
        | Quadrivalent_Lattice.False -> false
        | _ ->
            (* There may be at least one valid index. *)
            let new_t = ParamT ({descr = Ptr{pointed = elem_typ; index = Zero}; const;
              pred}, zero, index_minus_one ctx, ofs) in
            subtype ctx new_t u
        in
        ret
    | ParamT (_,_,_,_),
      ParamT ({descr = Ptr{pointed = {descr=Array (elem_typ, _);_};_}; pred; const},
          idx, fe, ofs) ->
        (* Is there at least one valid index in the concretization? *)
        let ret = match exists_valid_index ctx idx fe with
        | Quadrivalent_Lattice.False -> false
        | _ ->
            (* There may be at least one valid index. *)
            let new_u = ParamT ({descr = Ptr{pointed = elem_typ; index = Zero}; const;
              pred}, zero, index_minus_one ctx, ofs) in
            subtype ctx t new_u
        in
        ret
    | _ ->
        (* Both concretizations are singletons, so this case boils down to
         * the concrete operator (and {!subtype} acts as the concrete
         * operator on non-array types. *)
        subtype ctx t u
end

module EvalPred = struct

  module type Arg = sig
    include Domain_sig.Base
    val symbol : Context.t -> string -> binary
  end

  module type Sig = sig
    module Domain : Domain_sig.Base
    val check_invariant : size:int -> Domain.Context.t -> Ctypes.Pred.t
      -> Domain.binary -> bool
    val use_invariant : size:int -> Domain.Context.t -> Ctypes.Pred.t
      -> Domain.binary -> Domain.binary
  end

  module Make (A : Arg) = struct

    open Ctypes.Pred
    open A.Binary_Forward

    let binary_of_value ~size ctx v =
      let open Ctypes in
      match v with
      | Const x -> A.Binary_Forward.biconst ~size x ctx
      | Sym s -> A.symbol ctx s

    let cond_of_cmp ~size ctx cmpop v1 v2 =
      match cmpop with
      | Equal ->
          beq ~size ctx v1 v2
      | NotEqual ->
          A.Boolean_Forward.not ctx @@ beq ~size ctx v1 v2
      | ULt ->
          A.Boolean_Forward.not ctx @@ biule ~size ctx v2 v1
      | SLt ->
          A.Boolean_Forward.not ctx @@ bisle ~size ctx v2 v1
      | ULeq ->
          biule ~size ctx v1 v2
      | SLeq ->
          bisle ~size ctx v1 v2
      | UGeq ->
          biule ~size ctx v2 v1
      | SGeq ->
          bisle ~size ctx v2 v1
      | UGt ->
          A.Boolean_Forward.not ctx @@ biule ~size ctx v1 v2
      | SGt ->
          A.Boolean_Forward.not ctx @@ bisle ~size ctx v1 v2

    let lift_unop ~size ctx op =
      match op with
      | Extract (index,len) -> fun x ->
        bextract ~size:len ~index ~oldsize:size ctx x
        |> buext ~size ~oldsize:len ctx

    let lift_binop ~size ctx op =
      match op with
      | Add -> biadd ~size ctx ~nsw:false ~nuw:false
      | Sub -> bisub ~size ctx ~nsw:false ~nuw:false
      | Mul -> bimul ~size ctx ~nsw:false ~nuw:false
      | And -> band ~size ctx
      | Concat (size1,size2) -> bconcat ~size1 ~size2 ctx

    let rec binary_of_expr ~size ctx ~self e =
      match e with
      | Self -> self
      | Val v -> binary_of_value ~size ctx v
      | Unop (op, e) -> lift_unop ~size ctx op (binary_of_expr ~size ctx ~self e)
      | Binop (op, e1, e2) -> lift_binop ~size ctx op
          (binary_of_expr ~size ctx ~self e1) (binary_of_expr ~size ctx ~self e2)

    let rec cond_of_pred ~size ctx pred ~self =
      match pred with
      | True -> A.Boolean_Forward.true_ ctx
      | Cmp (op, e1, e2) ->
          cond_of_cmp ~size ctx op (binary_of_expr ~size ctx ~self e1)
          (binary_of_expr ~size ctx ~self e2)
      | And (p,q) ->
          let c1 = cond_of_pred ~size ctx p ~self in
          let c2 = cond_of_pred ~size ctx q ~self in
          A.Boolean_Forward.(&&) ctx c1 c2

    let check_invariant ~size ctx pred v =
      let c = cond_of_pred ~size ctx pred ~self:v in
      let is_true = A.Query.convert_to_quadrivalent (A.Query.boolean ctx c) in
      match is_true with
      | Quadrivalent_Lattice.True -> true
      | _ -> false (* Possibly false *)

    let use_invariant ~size ctx pred v =
      match pred with
      | True -> v
      | _ -> A.assume_binary ~size ctx (cond_of_pred ~size ctx pred ~self:v) v
  end

end
