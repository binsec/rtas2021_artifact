open Codex

let pretty_detailed = true

module L = Codex_logger

module type Subdomain = Domain_sig.Base

module Make (Sub : Subdomain) :
  Codex.Domain_sig.Base
  with module Context = Sub.Context
= struct

  let name = "Bitwise(" ^ Sub.name ^ ")";;
  let unique_id = Domain_sig.Fresh_id.fresh name;;

  type bits =
    | Zeroes
    | Ones
    | Unknown of Sub.binary

  let extract_from_bits ctx bits ~idx ~size ~oldsize = match bits with
  | Ones -> Ones
  | Zeroes -> Zeroes
  | Unknown x -> Unknown (Sub.Binary_Forward.bextract ~size ~index:idx ~oldsize ctx x)

  let sub_of_bits ~size ctx = function
  | Ones -> Sub.Binary_Forward.biconst ~size (Z.sub (Z.shift_left Z.one size) Z.one) ctx
  | Zeroes -> Sub.Binary_Forward.biconst ~size Z.zero ctx
  | Unknown x -> x

  module Imap0 = Interval_map.With_Extract(struct type t = bits end)

  module Imap : sig
    include module type of Imap0
    val to_list : size:int -> Sub.Context.t -> t -> (int * bits) list

    (** Particular case of [fold_on_diff] for folding a binary function with
       two [bits] arguments. In that case, it is not necessary to have an
       initial element, only a function to call in case the map is empty. This
       is useful when there is no practical initial element. *)
    (*
    val reduce_on_diff : size:int -> Sub.Context.t -> t ->
      (size1:int -> size2:int -> bits -> bits -> 'a) -> if_empty:(unit -> 'a) ->
       'a
    *)
  end = struct
    include Imap0

    let to_list ~size ctx map =
      fold_between ~size 0 map [] ~extract:(extract_from_bits ctx)
        (fun ~size _ofs elt acc -> (size,elt) :: acc)

    let reduce_on_diff ~size ctx map f ~if_empty =
      match to_list ~size ctx map with
      | (size_fst,fst) :: tl ->
          let _, res = List.fold_left (fun (size1,x) (size2,y) ->
            size1+size2, f ~size1 ~size2 x y
          ) (size_fst, fst) tl in
          res
      | [] -> if_empty ()
  end

  module Binary = struct
    type t = Imap.t
    let equal _ = assert false
    let compare _ = assert false
    let hash _ = assert false

    let pretty _ = assert false

    let of_sub ~size x =
      Imap.create ~size (Unknown x)

    let to_sub ~size ctx x =
      match Imap.to_list ~size ctx x with
      | (size_x,x) :: tl ->
          let _, sub = List.fold_left (fun (size1,x) (size2,y) ->
            let y = sub_of_bits ~size:size2 ctx y in
            size1+size2, Sub.Binary_Forward.bconcat ~size1 ~size2 ctx x y
          ) (size_x, sub_of_bits ~size:size_x ctx x) tl in
          sub
      | [] -> Sub.binary_empty ~size ctx
  end

  module Types = struct
    type binary = Binary.t
    type memory = ()
    type integer = Sub.integer
    type boolean = Sub.boolean
  end
  include Types

  module Integer = Sub.Integer
  module Boolean = Sub.Boolean

  module Context = Sub.Context

  type root_context = Sub.root_context
  let root_context_upcast = Sub.root_context_upcast
  let root_context = Sub.root_context

  include Builtin.Make(Types)(Context)

  type mu_context = Sub.mu_context
  let mu_context_upcast = Sub.mu_context_upcast
  let mu_context_open = Sub.mu_context_open

  module Boolean_Forward = Sub.Boolean_Forward

  module Integer_Forward = Sub.Integer_Forward

  let [@inline always] ar0 ~size f = fun ctx ->
    Binary.of_sub ~size @@ f ctx
  let [@inline always] ar1 ~size f = fun ctx a ->
    Binary.of_sub ~size @@ f ctx @@ Binary.to_sub ~size ctx a
  let [@inline always] ar2 ~size f = fun ctx a b ->
    Binary.of_sub ~size @@ f ctx
      (Binary.to_sub ~size ctx a) (Binary.to_sub ~size ctx b)
  let [@inline always] pred2 ~size f = fun ctx a b ->
    f ctx (Binary.to_sub ~size ctx a) (Binary.to_sub ~size ctx b)
  let [@inline always] ar3 f = fun ctx a b c -> match a,b,c with
    | Some a, Some b,  Some c -> Some (f ctx a b c)
    | _ -> None

  let to_ival ~signed ~size ctx x =
    Binary.to_sub ~size ctx x |> Sub.Query.binary ~size ctx
    |> Sub.Query.binary_to_ival ~signed ~size

  let to_integer ~signed ~size ctx x =
    to_ival ~signed ~size ctx x |> Framac_ival.Ival.project_int

  module Binary_Forward = struct
    (*
    let beq   ~size = pred2 ~size (Sub.Binary_Forward.beq   ~size)
    let assume ~size = assert false
    let bsext ~size ~oldsize = assert false
    let buext ~size ~oldsize = assert false
    let bofbool ~size = assert false
    let bashr ~size = ar2 ~size (Sub.Binary_Forward.bashr ~size)
    let blshr ~size = ar2 ~size (Sub.Binary_Forward.blshr ~size)
    let bshl ~size ~nsw ~nuw = ar2 ~size (Sub.Binary_Forward.bshl ~size ~nsw ~nuw)
    let bisdiv ~size = ar2 ~size (Sub.Binary_Forward.bisdiv ~size)
    let biudiv ~size = ar2 ~size (Sub.Binary_Forward.biudiv ~size)
    let bconcat ~size1 ~size2 = assert false
    let bismod ~size = ar2 ~size (Sub.Binary_Forward.bismod ~size)
    let biumod ~size = ar2 ~size (Sub.Binary_Forward.biumod ~size)
    let bextract ~size ~index ~oldsize = assert false
    let valid ~size = assert false
    let bunknown ~size = ar0 ~size @@ Sub.Binary_Forward.bunknown ~size
    let biconst ~size k = ar0 ~size @@ Sub.Binary_Forward.biconst ~size k
    let buninit ~size = ar0 ~size @@ Sub.Binary_Forward.buninit ~size
    let bshift ~size ~offset ~max = ar1 ~size @@ Sub.Binary_Forward.bshift ~size ~offset ~max
    let bindex ~size k = ar2 ~size @@ Sub.Binary_Forward.bindex ~size k

    let biadd ~size ~nsw ~nuw ctx a b =
      match a,b with
      | Const c1, Const c2 -> Const (Z.add c1 c2)
      | Offset (b, c1), Const c2
      | Const c2, Offset (b, c1) -> Offset (b, Z.add c1 c2)
      | _ -> biadd ~size ~nsw ~nuw ctx a b

    let bisub ~size ~nsw ~nuw ctx a b =
      match a,b with
      | Const c1, Const c2 -> Const (Z.sub c1 c2)
      | Offset (b, c1), Const c2 -> Offset (b, Z.sub c1 c2)
      | _ -> bisub ~size ~nsw ~nuw ctx a b
    *)
    module SB = Sub.Binary_Forward

    let beq ~size ctx x y =
      Imap.fold_on_diff x y (Boolean_Forward.true_ ctx) ~extract:(extract_from_bits ctx)
      @@ fun ~size ofs bit1 bit2 acc ->
        match bit1,bit2 with
        | Zeroes,Zeroes -> acc
        | Ones,Ones -> acc
        | Unknown x, y | y, Unknown x ->
            let sub_y = sub_of_bits ~size ctx y in
            Boolean_Forward.(&&) ctx acc (Sub.Binary_Forward.beq ~size ctx x sub_y)
        | Zeroes,Ones | Ones,Zeroes -> Boolean_Forward.false_ ctx

    let biule ~size = pred2 ~size (Sub.Binary_Forward.biule ~size)
    let bisle ~size = pred2 ~size (Sub.Binary_Forward.bisle ~size)
    let biadd ~size ~nsw ~nuw = ar2 ~size (Sub.Binary_Forward.biadd ~size ~nsw ~nuw)
    let bisub ~size ~nsw ~nuw = ar2 ~size (Sub.Binary_Forward.bisub ~size ~nsw ~nuw)
    let bimul ~size ~nsw ~nuw = ar2 ~size (Sub.Binary_Forward.bimul ~size ~nsw ~nuw)
    let bxor ~size = ar2 ~size (Sub.Binary_Forward.bxor ~size)
    let band ~size = ar2 ~size (Sub.Binary_Forward.band ~size)
    let bor ~size = ar2 ~size (Sub.Binary_Forward.bor ~size)

    let assume ~size ctx c b =
      Sub.Binary_Forward.assume ~size ctx c (Binary.to_sub ~size ctx b)
      |> Binary.of_sub ~size

    let bofbool ~size ctx bool =
      Sub.Binary_Forward.bofbool ~size ctx bool |> Binary.of_sub ~size

    let bsext ~size ~oldsize = assert false
    let buext ~size ~oldsize = assert false
    let bashr ~size = assert false
    let blshr ~size = assert false

    let bshl ~size ~nsw ~nuw ctx x shift =
      (* Try to concretize the shift precisely *)
      match to_integer ~signed:false ~size ctx shift with
      | shift ->
          let shift = Z.to_int shift in
          let init = Imap.create ~size Zeroes in
          Imap.fold_between ~size 0 x ~extract:(extract_from_bits ctx) init
          (fun ~size ofs bits acc ->
            assert false
          )
      | exception Framac_ival.Ival.Not_Singleton_Int ->
          ar2 ~size (Sub.Binary_Forward.bshl ~size ~nsw ~nuw) ctx x shift

    let bisdiv ~size = assert false
    let biudiv ~size = assert false
    let bconcat ~size1 ~size2 = assert false
    let bismod ~size = assert false
    let biumod ~size = assert false
    let bextract ~size ~index ~oldsize = assert false

    let valid ~size:_ = assert false

    let bunknown ~size ctx =
      Imap.create ~size @@ Unknown (SB.bunknown ~size ctx)

    let biconst ~size k ctx =
      let bits = Z.format "b" k in
      let init = Imap.create ~size Zeroes in
      if Z.equal k Z.zero then init
      else
        (* ofs = offset in the interval map. *)
        let rec aux nb_ones acc ofs =
          if ofs <= 0 then
            if nb_ones <> 0 then Imap.store ~size:nb_ones 0 acc Ones
            else acc
          else
            match bits.[ofs] with
            | '1' -> aux (nb_ones+1) acc (ofs-1)
            | '0' ->
                if nb_ones <> 0 then
                  aux 0 (Imap.store ~size:nb_ones (ofs+1) acc Ones) (ofs-1)
                else aux 0 acc (ofs-1)
            | _ -> assert false
        in
        aux 0 init (String.length bits - 1)

    let buninit ~size = assert false
    let bshift ~size ~offset ~max = assert false
    let bindex ~size k = assert false
    let biadd ~size ~nsw ~nuw ctx a b = assert false
    let bisub ~size ~nsw ~nuw ctx a b = assert false
  end

  module Memory_Forward = Assert_False_Transfer_Functions.Memory.Memory_Forward

  (**************** Serialization, fixpoind and nondet. ****************)

  type 'c in_tuple = 'c Sub.in_tuple

  type 'a in_acc = bool * 'a in_tuple

  type empty_tuple = Sub.empty_tuple
  let empty_tuple = Sub.empty_tuple

  (* The resulting computation: have we computed something, or should
     we juste take one of the arguments (or none). *)
  type 'c out_tuple = 'c Sub.out_tuple

  type ('a,'b) result = Result: bool * 'some in_tuple * ('some out_tuple -> 'a * 'b out_tuple) -> ('a,'b) result

  (* Higher-ranked polymorphism is required here, and we need a record for that. *)
  type 'elt higher = {subf: 'tl. Sub.Context.t -> 'elt -> 'elt -> 'tl
    Sub.in_acc -> ('elt,'tl) Sub.result  } [@@unboxed]

  (* Note: OCaml infers the wrong type (no principal type), we have to help it here. *)
  let serialize (type elt) (type c)
      {subf} ctx a b (included, (acc : c in_tuple)) : (elt,c) result =
    let Sub.Result (included, in_tup, deserialize) = subf ctx a b (included, acc) in
    Result (included, in_tup, deserialize)

  (*let serialize_memory  ctx a b acc = serialize {subf = Sub.serialize_memory}  ctx a b acc*)
  let serialize_memory _ _ _ = assert false (* Memory-related *)

  let serialize_integer ctx a b acc = serialize {subf=Sub.serialize_integer} ctx a b acc
  let serialize_boolean ctx a b acc = serialize {subf=Sub.serialize_boolean} ctx a b acc

  let serialize_binary ~size ctx a b acc =
    assert false

  (* Note: OCaml infers the wrong type (no principal type), we have to help it here. *)
  let typed_nondet2 (type c) ctx (acc : c in_tuple) : c out_tuple =
      Sub.typed_nondet2 ctx acc

  (* Note: OCaml infers the wrong type (no principal type), we have to help it here. *)
  let typed_fixpoint_step (type c) ctx (included, (acc : c in_tuple)) : bool * (close:bool -> (c out_tuple)) =
    let bool,continuef = Sub.typed_fixpoint_step ctx (included, acc) in
    bool,fun ~close -> continuef ~close

  (**************** Queries ****************)

   module Query = struct
    include Sub.Query

    let reachable _ = assert false (* Memory-related *)

    let binary ~size ctx (x : binary) =
      Binary.to_sub ~size ctx x |> Sub.Query.binary ~size ctx
  end

 (**************** Pretty printing ****************)

  let memory_pretty _ _ _ = assert false

  let pretty_bits ~size ctx pp = let open Format in function
  | Zeroes -> for i = 1 to size do pp_print_char pp '0' done
  | Ones -> for i = 1 to size do pp_print_char pp '1' done
  | Unknown x -> Sub.binary_pretty ~size ctx pp x

  let binary_pretty ~size ctx pp (x : binary) =
    let basis = Query.binary ~size ctx x in
    Query.Binary_Lattice.pretty ~size pp basis;
    if pretty_detailed then begin
      Format.pp_print_string pp "(= [";
      match Imap.to_list ~size ctx x with
      | [] -> ()
      | (size_fst, fst) :: tl ->
          pretty_bits ~size:size_fst ctx pp fst;
          List.iter (fun (size,bits) ->
            Format.fprintf pp ":%a" (pretty_bits ~size ctx) bits
          ) tl;
          Format.pp_print_string pp "]";
    end

  let boolean_pretty = Sub.boolean_pretty

  let integer_pretty = Sub.integer_pretty

  let memory_is_empty _ = assert false
  let binary_is_empty ~size:_ _ = assert false
  let integer_is_empty _ = assert false
  let boolean_is_empty _ = assert false

  let builtin_show_each_in string ctx args memory =
    assert false

  let binary_empty ~size = assert false
  let integer_empty = Sub.integer_empty
  let boolean_empty = Sub.boolean_empty

  let reachable _ = assert false

  let satisfiable = Sub.satisfiable

end
