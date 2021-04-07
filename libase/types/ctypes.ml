module Int_option = Datatype_sig.Option(Datatype_sig.Int)
module String_option = Datatype_sig.Option(Datatype_sig.String)

type value =
  | Const of Z.t
  | Sym of string

let pp_value fmt =
  let open Format in function
  | Const x -> Z.pp_print fmt x
  | Sym s -> fprintf fmt "\"%s\"" s

module Pred = struct
  type unop =
  | Extract of int * int (** start index, length *)

  let pp_unop op pp_expr fmt expr =
    match op with
    | Extract (idx,len) ->
      Format.fprintf fmt "%a{%d, %d}" pp_expr expr idx (idx+len-1)

  type binop =
  | Add | Sub | Mul | And
  | Concat of int * int (** Size 1, size 2. First argument is the most significant *)

  let pp_binop fmt op =
    Format.pp_print_string fmt @@ match op with
    | Add -> "+"
    | Sub -> "-"
    | Mul -> "*"
    | And -> "&"
    | Concat (size1, size2) -> Format.sprintf "::<%d,%d>" size1 size2

  type expr =
    | Val of value
    | Self
    | Unop of unop * expr
    | Binop of binop * expr * expr

  type cmpop = Equal | NotEqual | ULt | SLt | ULeq | SLeq | UGeq | SGeq | UGt | SGt

  let pp_cmpop fmt op =
    Format.pp_print_string fmt @@ match op with
    | Equal -> "="
    | NotEqual -> "!="
    | ULt -> "<u"
    | SLt -> "<s"
    | ULeq -> "<=u"
    | SLeq -> "<=s"
    | UGeq -> ">=u"
    | SGeq -> ">=s"
    | UGt -> ">u"
    | SGt -> ">s"

  (** Predicate on a structure field or array element, which is supposed to be
     true at all times. *)
  type t =
    | True
    | Cmp of cmpop * expr * expr
    | And of t * t

  let compare : t -> t -> int = Stdlib.compare

  let equal t u = compare t u = 0

  let rec pp_expr fmt =
    let open Format in function
    | Val value -> pp_value fmt value
    | Self -> pp_print_string fmt "self"
    | Unop (op, e) ->
      fprintf fmt "@[<hov 2>%a@]" (pp_unop op pp_expr) e
    | Binop (op, e1, e2) ->
      fprintf fmt "@[<hov 2>(%a@ %a@ %a)@]" pp_expr e1 pp_binop op pp_expr e2

  let rec pp fmt =
    let open Format in function
    | True -> pp_print_string fmt "true"
    | Cmp (cop, e1, e2) -> fprintf fmt "@[<hov 2>(%a@ %a@ %a)@]" pp_expr e1 pp_cmpop cop pp_expr e2
    | And (p1, p2) -> fprintf fmt "@[<hov 2>%a@ &&@ %a@]" pp p1 pp p2

  let eq v =
    Cmp (Equal, Self, Val v)

  let neq v =
    Cmp (NotEqual, Self, Val v)

  let slt v =
    Cmp (SLt, Self, Val v)

  let sleq v =
    Cmp (SLeq, Self, Val v)

  let sgeq v =
    Cmp (SGeq, Self, Val v)

  let sgt v =
    Cmp (SGt, Self, Val v)

  let ult v =
    Cmp (ULt, Self, Val v)

  let uleq v =
    Cmp (ULeq, Self, Val v)

  let ugeq v =
    Cmp (UGeq, Self, Val v)

  let ugt v =
    Cmp (UGt, Self, Val v)

  exception Undefined_symbol of string

  let lift_unop : unop -> Z.t -> Z.t = function
  | Extract (idx, len) -> fun x -> Z.extract x idx len

  let lift_binop : binop -> Z.t -> Z.t -> Z.t = function
  | Add -> Z.add
  | Sub -> Z.sub
  | Mul -> Z.mul
  | And -> Z.logand
  | Concat (_,size2) -> fun x y ->
      Z.logor (Z.shift_left x size2) y

  let rec eval_expr ~symbols ~self = function
  | Val (Const c) -> c
  | Val (Sym s) ->
    begin try Datatype_sig.StringMap.find s symbols
    with Not_found -> raise (Undefined_symbol s) end
  | Self -> self
  | Unop (op, e) ->
      lift_unop op (eval_expr ~symbols ~self e)
  | Binop (op, e1, e2) ->
      lift_binop op (eval_expr ~symbols ~self e1) (eval_expr ~symbols ~self e2)

  let lift_cmpop : cmpop -> Z.t -> Z.t -> bool = function
  | Equal -> Z.equal | NotEqual -> (fun a b -> not (Z.equal a b))
  | ULt -> Z.lt | ULeq -> Z.leq | UGeq -> Z.geq | UGt -> Z.gt
  | SLt -> fun x y -> Int32.compare (Z.to_int32 x) (Z.to_int32 y) < 0
  | SLeq -> fun x y -> Int32.compare (Z.to_int32 x) (Z.to_int32 y) <= 0
  | SGeq -> fun x y -> Int32.compare (Z.to_int32 x) (Z.to_int32 y) >= 0
  | SGt -> fun x y -> Int32.compare (Z.to_int32 x) (Z.to_int32 y) > 0

  let rec eval ~symbols ~self p =
    let module SM = Datatype_sig.StringMap in
    match p with
    | True -> true
    | And (p1,p2) -> eval ~symbols ~self p1 && eval ~symbols ~self p2
    | Cmp (cmpop, e1, e2) ->
        lift_cmpop cmpop (eval_expr ~symbols ~self e1) (eval_expr ~symbols ~self e2)
end

type basic = int * string (** Byte size and name. *)

type enum =
  { en_name : string option;
    en_byte_size : int;
    en_values : (string * int) list; }

type index =
  | ValidUnknown
  | Zero
  | PastEnd

let pp_index fmt =
  let open Format in function
  | ValidUnknown -> pp_print_string fmt "ValidUnknown"
  | Zero -> pp_print_string fmt "idx 0"
  | PastEnd -> pp_print_string fmt "PastEnd"

let join_indices i1 i2 =
  match i1,i2 with
  | Zero,Zero -> Zero
  | _ -> ValidUnknown

type structure =
  { st_name : string option;
    st_byte_size : int option;
    st_members : (int * string * typ) list;
      (** Offset, field name, type, and predicate on type. *)
  }

and descr =
  | Void
  | Base of basic
  | Structure of structure
  | Ptr of { pointed : typ; index : index }
      (** Pointed type and index of pointed element in array. For non-array
         pointed types, the index should be [ConstIdx 0]. *)
  | Enum of enum
  | Array of typ * value option
      (** Element type and number of elements (if statically known). *)
  | Function of typ * typ list (** Return type and parameter types *)

and typ =
  { descr : descr;
    const : bool;
    pred : Pred.t;
  }

let pp_string fmt s =
  Format.fprintf fmt "\"%s\"" s

(* Prints a type's description as an OCaml value, or simply its name if {reuse}
 * is true. (output in OCaml) *)
let rec pp_descr_name precedence reuse fmt descr =
  let open Format in
  match descr with
  | Void -> pp_print_string fmt "void"
  | Base (sz,name) -> fprintf fmt "@[<hov 2>%s(%d)@]" name sz
  | Ptr {pointed = typ; index = Zero} ->
      fprintf fmt "@[<hov 2>%a*@]" (pp_type_name precedence true) typ
  | Ptr {pointed = typ; index} ->
      fprintf fmt "@[<hov 2>%a* at %a@]" (pp_type_name precedence true) typ pp_index index
  | Structure ({st_name = Some name;_}) when reuse ->
      (* Simply refer to a (normally) declared structure *)
      pp_print_string fmt name
  | Structure s ->
      fprintf fmt "@[<hov 2>struct {@,\
          st_name =@ %a;@ \
          st_byte_size =@ %a;@ \
          st_members =@ \
            @[<v 2>[@ %a@ ]@];@ \
        }@]"
          String_option.pretty s.st_name
          Int_option.pretty s.st_byte_size
          (pp_print_list
            (fun fmt (offset,name,typ) ->
              fprintf fmt "(@[<hov 2>%d,@ %a,@ %a@]);"
                offset pp_string name (pp_type_name 0 false) typ))
            s.st_members
  | Enum ({en_name = Some name;_}) when reuse ->
      (* Simply refer to a (normally) declared enum *)
      pp_print_string fmt name
  | Enum e ->
      fprintf fmt "@[<hov 2>enum {@ \
          en_name =@ %a;@ \
          en_byte_size =@ %d;@ \
          en_values =@ \
            @[<v 2>[ %a@ \
            ]@];@ \
        }@]"
          String_option.pretty e.en_name
          e.en_byte_size
          (pp_print_list
            (fun fmt (name,value) ->
              fprintf fmt "(%a, %d);" pp_string name value))
            e.en_values
  | Array (typ, Some size) ->
      (if precedence > 1 then
        fprintf fmt "@[<hov 2>(%a[%a])@]"
      else
        fprintf fmt "@[<hov 2>%a[%a]@]") (pp_type_name 1 true) typ pp_value size
  | Array (typ, None) ->
      (if precedence > 1 then
        fprintf fmt "@[<hov 2>(%a[])@]"
      else
        fprintf fmt "@[<hov 2>%a[]@]") (pp_type_name 1 true) typ
  | Function (ret_t, params) ->
      (if precedence > 0 then
        fprintf fmt "@[<hov 2>(Function (%a, [%a]))@]"
      else
        fprintf fmt "@[<hov 2>Function (%a, [%a])@]")
        (pp_type_name 0 reuse) ret_t
        (pp_print_list ~pp_sep:(fun fmt () -> fprintf fmt ";@ ") (pp_type_name 0 true)) params

and pp_type_name precedence reuse fmt typ =
  let open Format in
  (if precedence > 0 then
    fprintf fmt "@[<hov 2>(%a%a%a)@]" (pp_descr_name 0 reuse) typ.descr
  else
    fprintf fmt "@[<hov 2>%a%a%a@]" (pp_descr_name precedence reuse) typ.descr)
      (fun fmt c -> if c then fprintf fmt "@ const" else ()) typ.const
      (fun fmt p -> if p <> Pred.True then fprintf fmt "@ with %a" Pred.pp p else ()) typ.pred

(* Print the type's name. *)
let pp fmt typ =
  pp_type_name 0 true fmt typ

let pp_descr fmt descr =
  pp_descr_name 0 true fmt descr

let compare_list cmp_elt l1 l2 =
  let rec cmp = function
    | [],[] -> 0
    | [],_ -> -1
    | _,[] -> 1
    | (h1 :: t1), (h2 :: t2) ->
        let c = cmp_elt h1 h2 in
        if c = 0 then cmp (t1, t2) else 0
  in
  cmp (l1,l2)

let rec compare_descr ~only_descr x y =
  match x,y with
  | Void,Void -> 0
  | Void, _ -> -1
  | _,Void -> 1
  | Base (_,name1), Base (_,name2) -> Pervasives.compare name1 name2
  | Base _, _ -> -1
  | _, Base _ -> 1
  | Structure s1, Structure s2 -> Pervasives.compare s1.st_name s2.st_name
  | Structure _, _ -> -1
  | _, Structure _ -> 1
  | Ptr {pointed=t;index=ti}, Ptr {pointed=u;index=ui} ->
      let c = compare_typ ~only_descr t u in
      if only_descr || c <> 0 then c else (Pervasives.compare : index -> index -> int) ti ui
  | Ptr _, _ -> -1
  | _, Ptr _ -> 1
  | Enum { en_name = n1; _ }, Enum { en_name = n2; _ } -> Pervasives.compare n1 n2
  | Enum _, _ -> -1
  | _, Enum _ -> 1
  | Array (t,l), Array (t',l') ->
      let c = (Pervasives.compare : value option -> value option -> int) l l' in
      if c <> 0 then c else compare_typ ~only_descr t t'
  | Array _, _ -> -1
  | _, Array _ -> 1
  | Function (t,ts), Function (u,us) ->
      compare_list (compare_typ ~only_descr) (t :: ts) (u :: us)

and compare_typ ~only_descr t u =
  if only_descr then compare_descr ~only_descr t.descr u.descr
  else
    let c1 = compare_descr ~only_descr t.descr u.descr in
    if c1 <> 0 then c1 else
      let c2 = (Pervasives.compare : bool -> bool -> int) t.const u.const in
      if c2 <> 0 then c2
      else (Pervasives.compare : Pred.t -> Pred.t -> int) t.pred u.pred

let compare = compare_typ

let equal_descr ~only_descr t u =
  compare_descr ~only_descr t u = 0

let equal ~only_descr t u =
  equal_descr ~only_descr t.descr u.descr
  && t.const = u.const && ((=) : Pred.t -> Pred.t -> bool) t.pred u.pred

let pointer t =
  match t.descr with
  | Ptr _ -> true
  | _ -> false

module type Type_settings = sig
  val root : string * typ
  val special_data_addrs : (int * (typ * int)) list
  val unique_array_types : typ list
  val non_modifiable_types : typ list
  val global_symbols : (string * int * Pred.t) list
end
