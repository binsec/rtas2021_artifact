module type Base_with_types = sig
  include Domain_sig.Base
  module Type : Type_domain.S
    with module BR.Scalar.Context = Context

  (** Returns [None] if the argument is bottom. *)
  val typ : size:int -> Context.t -> binary -> Type.t option

  val with_type : Type.t -> size:int -> Context.t -> binary -> binary

  (** Get a global symbol from its name in the definition of the interface
      types. *)
  val global_symbol : Context.t -> string -> int * binary
end

(** Same signature as above, but the subdomain also has types. *)
module type Base_with_types2 = sig
  include Base_with_types
end

module type S = sig
  include Domain_sig.Base

  (** Exception thrown by {!Memory_Forward.load} when the load fails because
      the pointed region needs to be focused first. Contains the load result, as
      well as the new memory with the focusing performed. *)
  exception Needs_focusing of binary * memory
end

module type S_with_types2 = sig
  include Base_with_types2
  exception Needs_focusing of binary * memory
end

module Make (D : Domain_sig.Base) (L : Alarm_logger.S) = struct

  let name = "With_focusing(" ^ D.name ^ ")";;
  let unique_id = Domain_sig.Fresh_id.fresh name;;

  type binary =
    | Const of Z.t
    | Offset of D.binary * Z.t
        (** Addition of a subdomain value and a precisely known number. *)

  module Binary = struct
    type t = binary

    let compare x y =
      match x,y with
      | Const i, Const j -> Z.compare i j
      | Offset (b1, o1), Offset (b2, o2) ->
          let c = D.Binary.compare b1 b2 in
          if c <> 0 then c else Z.compare o1 o2
      | Const _, Offset _ -> -1
      | Offset _, Const _ -> 1

    let equal x y =
      match x,y with
      | Const i, Const j -> Z.equal i j
      | Offset (b1, o1), Offset (b2, o2) ->
          D.Binary.equal b1 b2 && Z.equal o1 o2
      | Const _, Offset _ | Offset _, Const _ -> false

    let hash _ = assert false

    let to_sub ~size ctx = function
    | Const c ->
        D.Binary_Forward.biconst ~size c ctx
    | Offset (b, c) ->
        L.debug ~level:4 "before to_sub: Offset (%a, %s)" (D.binary_pretty ~size ctx) b (Z.format "%X" c);
        let res = D.Binary_Forward.(biadd ~size ~nsw:false ~nuw:false ctx b @@ biconst
          ~size c ctx)
        in
        L.debug ~level:4 "after to_sub: %a" (D.binary_pretty ~size ctx) res;
        res

    let of_sub ~size:_ _ctx b =
      (* XXX: optimize const case *)
      Offset (b, Z.zero)

    let pretty _ = assert false
  end

  module Binary_op = struct
    type t = Unfetched | B of Binary.t

    let lift f = function
    | Unfetched -> Unfetched
    | B b -> B (f b)

    let concat f ~size1 ~size2 b1 b2 =
      match b1,b2 with
      | Unfetched,_ | _,Unfetched -> Unfetched
      | B b1, B b2 -> B (f ~size1 ~size2 b1 b2)
  end

  module Bin_map = Extstdlib.Map.Make(D.Binary)
  (* Interval map. Offsets are in bits, not bytes. *)
  module Region : sig
    include module type of Interval_map.With_Extract(Binary_op)
    val load : size:int
      -> extract:(Binary_op.t -> idx:int -> size:int -> oldsize:int -> Binary_op.t)
      -> t -> Z.t -> (int * int * Binary_op.t) list

    (** Same as load, but if the region to load contains several values,
        concatenate them. *)
    val load_oneval : size:int
      -> extract:(Binary_op.t -> idx:int -> size:int -> oldsize:int -> Binary_op.t)
      -> concat:(size1:int -> Binary_op.t -> size2:int -> Binary_op.t -> Binary_op.t)
      -> t -> Z.t -> Binary_op.t
  end = struct
    include Interval_map.With_Extract(Binary_op)

    let load ~size ~extract map key =
      let key = Z.to_int key in
      assert (size > 0);
      assert (key >= 0);
      let region_size = get_size map in
      assert (key + size <= region_size);
      fold_between ~size key map [] ~extract (fun ~size key v acc -> (size,key,v)::acc)

    let load_oneval ~size ~extract ~concat map key =
      let key = Z.to_int key in
      assert(size > 0);
      assert(key >= 0);
      let region_size = get_size map in
      (* Codex_log.feedback "loading offset %d size %d region_size %d %a" key size region_size (pretty ctx) {map}; *)                
      assert(key + size <= region_size);
      (* Kernel.feedback "load idx %d size %d" key size; *)
      let l = fold_between ~size key map [] ~extract
          (fun ~size _key value acc -> (size,value)::acc)
      in match l with
      | [] -> assert false    (* The address is correct, we must load something. *)
      | [_,v] -> v
      | (sizea,a)::rest ->
          (* TODO: this depends on endiannees *)
          let f (size,acc) (newsize,newv) =
            let newsize = newsize in
            let acc = concat ~size1:size acc ~size2:newsize newv in
            (size+newsize,acc)
          in
          let fsize,v = List.fold_left f (sizea,a) rest in
          assert (fsize == size);
          v
  end
  type cache = Region.t Bin_map.t

  module Types = struct
    type nonrec binary = binary

    type memory =
      { sub_mem : D.memory; mutable cache : cache }

    type integer = D.integer
    type boolean = D.boolean
  end
  include (Types : module type of Types with type binary := binary)

  module Integer = D.Integer
  module Boolean = D.Boolean

  module Context = D.Context

  type root_context = D.root_context
  let root_context_upcast = D.root_context_upcast
  let root_context = D.root_context

  include Builtin.Make(Types)(Context)

  exception Needs_focusing of binary * memory

  type mu_context = D.mu_context
  let mu_context_upcast = D.mu_context_upcast
  let mu_context_open = D.mu_context_open

  module Boolean_Forward = struct
    let (||) = D.Boolean_Forward.(||)
    let (&&) = D.Boolean_Forward.(&&)
    let not = D.Boolean_Forward.not
    let true_ = D.Boolean_Forward.true_
    let false_ = D.Boolean_Forward.false_
  end

  let assume_boolean = D.assume_boolean
  let boolean_unknown = D.boolean_unknown

  module Integer_Forward = struct
    let one = D.Integer_Forward.one
    let zero = D.Integer_Forward.zero
    let iconst k = D.Integer_Forward.iconst k
    let ile = D.Integer_Forward.ile
    let ieq = D.Integer_Forward.ieq
    let isub = D.Integer_Forward.isub
    let ixor = D.Integer_Forward.ixor
    let ior = D.Integer_Forward.ior
    let iand = D.Integer_Forward.iand
    let ishr = D.Integer_Forward.ishr
    let ishl = D.Integer_Forward.ishl
    let imod = D.Integer_Forward.imod
    let idiv = D.Integer_Forward.idiv
    let imul = D.Integer_Forward.imul
    let iadd = D.Integer_Forward.iadd
    let itimes k = D.Integer_Forward.itimes k
  end

  let assume_integer = D.assume_integer
  let integer_unknown = D.integer_unknown

  let [@inline always] ar0 ~size f = fun ctx ->
    Binary.of_sub ~size ctx @@ f ctx
  let [@inline always] ar1 ~size f = fun ctx a ->
    Binary.of_sub ~size ctx @@ f ctx @@ Binary.to_sub ~size ctx a
  let [@inline always] ar2 ~size f = fun ctx a b ->
    Binary.of_sub ~size ctx @@ f ctx
      (Binary.to_sub ~size ctx a) (Binary.to_sub ~size ctx b)
  let [@inline always] pred2 ~size f = fun ctx a b ->
    f ctx (Binary.to_sub ~size ctx a) (Binary.to_sub ~size ctx b)
  let [@inline always] _ar3 f = fun ctx a b c -> match a,b,c with
    | Some a, Some b,  Some c -> Some (f ctx a b c)
    | _ -> None

  (**************** Pretty printing ****************)

  let memory_pretty ctx pp mem =
    let open Format in
    fprintf pp "@[<hov 2>{ sub_mem =@ %a;@ cache =@ %a@ }@]"
      (D.memory_pretty ctx) mem.sub_mem (Bin_map.mk_pretty
        Binary.pretty
        (fun pp reg -> fprintf pp "@[<hov 2><region of size %d bits>@]" (Region.get_size reg))) mem.cache

  let binary_pretty ~size ctx fmt x =
    let open Format in
    match x with
    | Const i -> fprintf fmt "{0x%s}" @@ Z.format "%X" i
    | Offset (base, ofs) ->
        if Z.equal ofs Z.zero then
          D.binary_pretty ~size ctx fmt base
        else
          fprintf fmt "@[<hov 2>%a + 0x%s@]" (D.binary_pretty ~size ctx) base
            (Z.format "%X" ofs)

  module Binary_Forward = struct
    let beq   ~size = pred2 ~size (D.Binary_Forward.beq   ~size)
    let biule ~size = pred2 ~size (D.Binary_Forward.biule ~size)
    let bisle ~size = pred2 ~size (D.Binary_Forward.bisle ~size)

    let two_pow_32 = Z.shift_left Z.one 32
    let add_32bits x y = Z.erem (Z.add x y) two_pow_32
    let sub_32bits x y = Z.erem (Z.sub x y) two_pow_32

    let biadd ~size ~nsw ~nuw ctx x y =
      match x,y with
      | Const i, Const j -> Const (add_32bits i j) (* XXX hardcoded 32-bit offset size *)
      | Offset (b,ofs), Const i | Const i, Offset (b,ofs) -> Offset (b, add_32bits ofs i)
      | Offset _, Offset _ ->
          (* Let's not try to simplify this. The case happened where it
             prevented simplifications in the subdomain. The DBA was:
             b := a - 1
             c := b + 4*b
             We expected the subdomain to simplify c into 5*b, but with the
             rewrite below uncommented, the subdomain would see:
             c := Offset (a + 4*b, -1)
             which it was unable to simplify.
          *)
          (*Offset(D.Binary_Forward.biadd ~size ~nsw ~nuw ctx b1 b2, add_32bits o1 o2)*)
          ar2 ~size (D.Binary_Forward.biadd ~size ~nsw ~nuw) ctx x y

    let bisub ~size ~nsw ~nuw ctx x y =
      match x,y with
      | Const i, Const j -> Const (sub_32bits i j)
      | Offset (b,ofs), Const i -> Offset (b, sub_32bits ofs i)
      | Const _, Offset (_,_) ->
          (* Let's just give up on this. *)
          Offset (D.Binary_Forward.bisub ~size ~nsw ~nuw ctx
              (Binary.to_sub ~size ctx x) (Binary.to_sub ~size ctx y),
            Z.zero)
      | Offset (b1,o1), Offset (b2,o2) ->
          (* This is weird. *)
          Offset(D.Binary_Forward.bisub ~size ~nsw ~nuw ctx b1 b2, sub_32bits o1 o2)

    let bimul ~size ~nsw ~nuw = ar2 ~size (D.Binary_Forward.bimul ~size ~nsw ~nuw)
    let bxor ~size = ar2 ~size (D.Binary_Forward.bxor ~size)
    let band ~size = ar2 ~size (D.Binary_Forward.band ~size)
    let bor ~size = ar2 ~size (D.Binary_Forward.bor ~size)

    let bsext ~size ~oldsize ctx x =
      D.Binary_Forward.bsext ~size ~oldsize ctx (Binary.to_sub ~size:oldsize ctx x)
      |> Binary.of_sub ~size ctx

    let buext ~size ~oldsize ctx x =
      D.Binary_Forward.buext ~size ~oldsize ctx (Binary.to_sub ~size:oldsize ctx x)
      |> Binary.of_sub ~size ctx

    let bofbool ~size ctx bool =
      D.Binary_Forward.bofbool ~size ctx bool |> Binary.of_sub ~size ctx

    let bashr ~size = ar2 ~size (D.Binary_Forward.bashr ~size)
    let blshr ~size = ar2 ~size (D.Binary_Forward.blshr ~size)
    let bshl ~size ~nsw ~nuw = ar2 ~size (D.Binary_Forward.bshl ~size ~nsw ~nuw)
    let bisdiv ~size = ar2 ~size (D.Binary_Forward.bisdiv ~size)
    let biudiv ~size = ar2 ~size (D.Binary_Forward.biudiv ~size)
    let bconcat ~size1 ~size2 ctx x1 x2 =
      Binary.(of_sub ~size:(size1+size2) ctx @@ D.Binary_Forward.bconcat ctx
      ~size1 ~size2 (to_sub ctx ~size:size1 x1) (to_sub ctx ~size:size2 x2))
    let bismod ~size = ar2 ~size (D.Binary_Forward.bismod ~size)
    let biumod ~size = ar2 ~size (D.Binary_Forward.biumod ~size)

    let bextract ~size ~index ~oldsize ctx x =
      D.Binary_Forward.bextract ~size ~index ~oldsize ctx (Binary.to_sub ~size:oldsize ctx x)
      |> Binary.of_sub ~size ctx

    let valid ~size access_type ctx x =
      D.Binary_Forward.valid ~size access_type ctx @@ Binary.to_sub ~size ctx x

    let valid_ptr_arith ~size ctx x y =
      D.Binary_Forward.valid_ptr_arith ~size ctx (Binary.to_sub ~size ctx x)
        (Binary.to_sub ~size ctx y)

    let biconst ~size:_ k _ctx = Const k

    let buninit ~size = ar0 ~size @@ D.Binary_Forward.buninit ~size
    let bshift ~size ~offset ~max = ar1 ~size @@ D.Binary_Forward.bshift ~size ~offset ~max
    let bindex ~size k = ar2 ~size @@ D.Binary_Forward.bindex ~size k

    let bchoose ~size:_ _ = assert false
  end

  let binary_unknown ~size ~level =
    ar0 ~size @@ D.binary_unknown ~size ~level

  let assume_binary ~size ctx c b =
    D.assume_binary ~size ctx c (Binary.to_sub ~size ctx b)
    |> Binary.of_sub ~size ctx

  module Memory_Forward = struct
    (* For (probably) unused [free], [malloc] and [memcpy]. *)
    include Assert_False_Transfer_Functions.Memory.Memory_Forward

    (* Focus a region, after unfocusing any aliased one, if any. *)
    let do_focus ctx focus_base focus_size ofs_in_focus mem =
      let ptr_size = Codex_config.ptr_size () in
      L.debug ~level:3 "focusing. @[<v 2>base = %a@.focus size = %d@.offset in \
        focus = %d@]" (D.binary_pretty ~size:ptr_size ctx) focus_base
        focus_size ofs_in_focus;
      (* We must unfocus the currently focused
       * instance (if any) and focus this one. *)
      (* XXX: this is unsound: we would need to test aliasing of regions,
       * not individual memory cells. *)
      let pred other_addr = D.may_alias ~size:ptr_size ctx other_addr focus_base in
      let mem_unfocused = match Bin_map.find_first_opt pred mem.cache with
      | Some (other_b, region) ->
          L.debug ~level:3 "Aliasing region found. Unfocusing. @[<v 2> base = \
            %a" (D.binary_pretty ~size:ptr_size ctx) other_b;
          let size_reg = Region.get_size region in
          let to_store = Region.load ~size:size_reg
              ~extract:(fun b ~idx ~size ~oldsize ->
                Binary_op.lift (Binary_Forward.bextract ~size ~index:idx
                ~oldsize ctx) b) region Z.zero
          in
          let sub_mem = List.fold_left (fun sub_mem (size_v, ofs, v) ->
            match v with
            | Binary_op.Unfetched -> sub_mem
            | Binary_op.B v ->
              let v = Binary.to_sub ~size:size_v ctx v in
              L.debug ~level:3 "@[<hov 2>Storing at ofs %d w/ size %d value: %a@]" size_v ofs (D.binary_pretty ~size:size_v ctx) v;
              assert (ofs mod 8 = 0);
              let ofs_bytes = Z.(ediv (of_int ofs) (of_int 8)) in
              let addr = D.Binary_Forward.(biadd ~size:ptr_size ~nsw:true ~nuw:true ctx other_b @@
              (* We know that there will be no overflow, all stores in a focused
               * region are guarded by alarms. So we can take it for granted
               * and set the [nsw] and [nuw] flags. This avoids some incorrect
               * writes to address zero, which can generate horrible
               * fixpoint-finding bugs.  *)
                biconst ~size:ptr_size ofs_bytes ctx) in
              D.Memory_Forward.store ~size:size_v ctx sub_mem addr v
          ) mem.sub_mem to_store in
          { sub_mem;
            cache = Bin_map.remove other_b mem.cache;
          }
      | None -> mem
      in
      let region = Region.create ~size:focus_size Binary_op.Unfetched in
      (* TODO: apply type predicates to every field of the structure. *)
      mem_unfocused, region

    let load ~size ctx mem address =
      let ptr_size = Codex_config.ptr_size () in
      L.debug ~level:3 "@[<v 2>With_focusing.load ~size:%d@.address %a" size (binary_pretty ~size:ptr_size ctx) address;
      let address_sub = Binary.to_sub ~size:ptr_size ctx address in
      L.debug ~level:3 "address_sub = %a" (D.binary_pretty ~size:ptr_size ctx) address_sub;
      (* Is the address focused? *)
      match address with
      | Const _ -> D.Memory_Forward.load ~size ctx mem.sub_mem address_sub
          |> Binary.of_sub ~size ctx
      | Offset (b, c) ->
          if Bin_map.mem b mem.cache then begin
            let ofs_bits = Z.mul c (Z.of_int 8) in
            L.result "@[<v 2>focused.@.base = %a@.offset = %a@]" (D.binary_pretty ~size:(Codex_config.ptr_size ()) ctx) b Z.pp_print ofs_bits;
            let region = Bin_map.find b mem.cache in
            let load_res = Region.load_oneval ~size ~extract:(fun b ~idx ~size ~oldsize ->
                Binary_op.lift (Binary_Forward.bextract ~size ~index:idx ~oldsize ctx) b)
              ~concat:(fun ~size1 b1 ~size2 b2 ->
                Binary_op.concat (Binary_Forward.bconcat ctx) ~size1 ~size2 b1 b2) region ofs_bits
            in
            match load_res with
            | Binary_op.Unfetched ->
                (* Actually do the load in the subdomain's memory. *)
                let result = D.Memory_Forward.load ~size ctx mem.sub_mem address_sub in
                let result = Binary.of_sub ~size ctx result in
                (* Now update the cache with this new information. *)
                let new_region = Region.store ~size (Z.to_int ofs_bits) region (Binary_op.B result) in
                let cache = Bin_map.add b new_region mem.cache in
                raise (Needs_focusing (result, {mem with cache}))
            | Binary_op.B result -> result
          end
          else begin
            L.result "Not focused.";
            (* Not focused yet. Should we focus it? *)
            match D.should_focus ~size ctx mem.sub_mem address_sub with
            | Some (focus_base, focus_size, ofs_in_focus) ->
                let mem_unfocused, region = do_focus ctx focus_base focus_size
                  ofs_in_focus mem in
                (* Load the required value from the subdomain's memory. *)
                let result = D.Memory_Forward.load ~size ctx mem.sub_mem address_sub in
                let result = Binary.of_sub ~size ctx result in
                (* Now update the cache with this new information. *)
                let new_region = Region.store ~size ofs_in_focus region (Binary_op.B result) in
                let cache = Bin_map.add focus_base new_region mem.cache in
                raise (Needs_focusing (result, {mem_unfocused with cache}))
            | None ->
                D.Memory_Forward.load ~size ctx mem.sub_mem address_sub
                |> Binary.of_sub ~size ctx
          end

    let store ~size ctx mem address value =
      let ptr_size = Codex_config.ptr_size () in
      L.debug ~level:3 "@[<v 2>With_focusing.store ~size:%d@.address %a@.value %a" size (binary_pretty ~size:ptr_size ctx) address (binary_pretty ~size ctx) value;
      (* If the address is already focused? *)
      match address with
      | Offset (b, c) ->
        begin try
          let region = Bin_map.find b mem.cache in
          L.result "@[<v 2>focused.@.base = %a@.offset = %a@]" (D.binary_pretty ~size:ptr_size ctx) b Z.pp_print c;
          (* XXX: size or size/8? Document it *)
          if Z.geq c Z.zero && Z.leq (Z.add c (Z.of_int size)) (Z.of_int @@ Region.get_size region) then begin
            let new_region = Region.store ~size (Z.to_int c) region (Binary_op.B value) in
            let new_cache = Bin_map.add b new_region mem.cache in
            (* No mutation of the cache field here, but a functional update *)
            { mem with cache = new_cache }
          end
          else begin
            L.alarm "bad_focused_store";
            L.error "size %d does not respect 0 <= size < %d. Focus not performed, direct store" size (Region.get_size region);
            { mem with sub_mem = D.Memory_Forward.store ~size ctx
                mem.sub_mem (Binary.to_sub ctx ~size:ptr_size
                address) (Binary.to_sub ~size ctx value) }
          end
        with Not_found ->
          (* Not focused yet. Should we focus it? *)
          match D.should_focus ~size ctx mem.sub_mem b with
          | Some (focus_base, focus_size, ofs_in_focus) ->
            let mem_unfocused, region = do_focus ctx focus_base focus_size
              ofs_in_focus mem in
            let region = Region.store ~size ofs_in_focus region (Binary_op.B value) in
            { mem_unfocused with cache = Bin_map.add focus_base region mem.cache }
          | None ->
            { mem with sub_mem = D.Memory_Forward.store ~size ctx
                mem.sub_mem (Binary.to_sub ctx ~size:(Codex_config.ptr_size ()) address)
                (Binary.to_sub ~size ctx value) }
          end
        | Const _ ->
            { mem with sub_mem = D.Memory_Forward.store ~size ctx mem.sub_mem
                (Binary.to_sub ctx ~size:(Codex_config.ptr_size ()) address)
                (Binary.to_sub ~size ctx value) }


    let unknown ~level ctx =
      {sub_mem = D.Memory_Forward.unknown ~level ctx; cache = Bin_map.empty}
  end

  let assume_memory ctx cond mem =
    let sub_mem = D.assume_memory ctx cond mem.sub_mem in
    let cache = Bin_map.fold (fun var region acc_new_cache ->
      let new_var = D.assume_binary ~size:(Codex_config.ptr_size ()) ctx
        cond var in
      let focus_size = Region.get_size region in
      let init_new_region = Region.create ~size:focus_size Binary_op.Unfetched in
      let new_region = Region.fold_between 0 region ~size:focus_size
        ~extract:(fun b ~idx ~size ~oldsize ->
          Binary_op.lift (Binary_Forward.bextract ~size ~index:idx ~oldsize ctx) b
        ) init_new_region (fun ~size idx x acc_new_region ->
          let new_x = Binary_op.lift (assume_binary ~size ctx cond) x in
          Region.store ~size idx acc_new_region new_x
        )
      in
      Bin_map.add new_var new_region acc_new_cache
    ) mem.cache Bin_map.empty
    in
    { sub_mem; cache }
  (*let memcpy ~size = D.Memory_Forward.memcpy ~size*)

  let boolean_pretty = D.boolean_pretty

  let integer_pretty = D.integer_pretty

  (**************** Serialization, fixpoind and nondet. ****************)

  type 'c in_tuple = 'c D.in_tuple

  type 'a in_acc = bool * 'a in_tuple

  type empty_tuple = D.empty_tuple
  let empty_tuple = D.empty_tuple

  (* The resulting computation: have we computed something, or should
     we juste take one of the arguments (or none). *)
  type 'c out_tuple = 'c D.out_tuple

  type ('a,'b) result = Result: bool * 'some in_tuple * ('some out_tuple -> 'a * 'b out_tuple) -> ('a,'b) result

  (* Higher-ranked polymorphism is required here, and we need a record for that. *)
  type 'elt higher = {subf: 'tl. D.Context.t -> 'elt -> 'elt -> 'tl D.in_acc -> ('elt,'tl) D.result  } [@@unboxed]

  (* Note: OCaml infers the wrong type (no principal type), we have to help it here. *)
  let serialize (type elt) (type c)
      {subf} ctx a b (included, (acc : c in_tuple)) : (elt,c) result =
    let D.Result (included, in_tup, deserialize) = subf ctx a b (included, acc) in
    Result (included, in_tup, deserialize)

  let serialize_memory ctx m1 m2 acc =
    let D.Result (included, in_tup, deserialize) =
      D.serialize_memory ctx m1.sub_mem m2.sub_mem acc in
    Result (included, in_tup, fun out ->
      let sub_mem, out = deserialize out in
      {sub_mem; cache = Bin_map.empty}, out)

  let serialize_integer ctx a b acc = serialize {subf=D.serialize_integer} ctx a b acc
  let serialize_boolean ctx a b acc = serialize {subf=D.serialize_boolean} ctx a b acc

  let serialize_binary ~size ctx a b acc =
    let xa = Binary.to_sub ~size ctx a in
    let xb = Binary.to_sub ~size ctx b in
    let D.Result (included, acc, deserialize) = D.serialize_binary ~size ctx xa xb acc in
    Result (included, acc, fun out ->
      let r, out = deserialize out in
      Binary.of_sub ~size ctx r, out)

  (* Note: OCaml infers the wrong type (no principal type), we have to help it here. *)
  let typed_nondet2 (type c) ctx (acc : c in_tuple) : c out_tuple =
      D.typed_nondet2 ctx acc

  (* Note: OCaml infers the wrong type (no principal type), we have to help it here. *)
  let typed_fixpoint_step (type c) ctx (included, (acc : c in_tuple)) : bool * (close:bool -> (c out_tuple)) =
    let bool,continuef = D.typed_fixpoint_step ctx (included, acc) in
    bool,fun ~close -> continuef ~close

  (**************** Queries ****************)

   module Query = struct
    include D.Query
    let reachable ctx mem = D.Query.reachable ctx mem.sub_mem

    let boolean = D.Query.boolean

    let binary ~size ctx b =
      D.Query.binary ~size ctx @@ Binary.to_sub ~size ctx b

    let integer = D.Query.integer
  end

  let memory_is_empty _ = assert false
  let binary_is_empty ~size:_ _ = assert false
  let integer_is_empty _ = assert false
  let boolean_is_empty _ = assert false

  let builtin_show_each_in _string _ctx _args _memory =
    assert false

  let binary_empty ~size = ar0 ~size (D.binary_empty ~size)
  let integer_empty = D.integer_empty
  let boolean_empty = D.boolean_empty

  let reachable ctx mem =
    D.reachable ctx mem.sub_mem

  let satisfiable = D.satisfiable

  let union _cond _ctx _tuple = assert false

  let should_focus : size:int -> Context.t -> memory -> binary ->
      (binary * int * int) option = fun ~size:_ _ctx _memory _address ->
    assert false

  let may_alias : size:int -> Context.t -> binary -> binary -> bool = fun ~size:_ _ctx _addr1 _addr2 ->
    assert false

end

module Make_with_types (D : Base_with_types) (L : Alarm_logger.S) : S_with_types2
  with module Context = D.Context
  and type boolean = D.boolean
  and module Type = D.Type
= struct

  include Make (D) (L)

  module Type = D.Type

  let typ ~size ctx x = D.typ ~size ctx @@ Binary.to_sub ~size ctx x

  let with_type typ ~size ctx x =
    Binary.of_sub ~size ctx @@ D.with_type typ ~size ctx @@
    Binary.to_sub ~size ctx x

  let global_symbol ctx name =
    let size,v = D.global_symbol ctx name in
    size, Binary.of_sub ~size ctx v
end
