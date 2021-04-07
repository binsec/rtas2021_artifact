(**************************************************************************)
(*                                                                        *)
(*  This file is part of Frama-C.                                         *)
(*                                                                        *)
(*  Copyright (C) 2007-2014                                               *)
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

open Term_type;;
open Term_constructors;;
open Term_types;;

let rec pretty_type fmt = function
  | Binary s -> Format.fprintf fmt "bin%d" s
  | Boolean -> Format.fprintf fmt "bool"
  | Memory -> Format.fprintf fmt "mem"
  | Tuple l -> Format.fprintf fmt "%a" pretty_types (Lazy.force_val l)
  | Integer -> Format.fprintf fmt "integer"
and pretty_types fmt types =
  (Immutable_dynamic_array.pp_dynarray ~pre:"[@[<hv>" ~suf:"@]]" ~sep:",@," pretty_type) fmt types
;;

let pretty_set fprint_term fmt l =
  let open Format in
  let card = Term_set.cardinal l in
  if card == 0
  then fprintf fmt "{}"
  else begin
    fprintf fmt "{ @[<hv>"; 
    if card < 10 then
      let elt = Term_set.choose l in
      fprintf fmt "@[<h>%a@]" fprint_term elt;
      let l = Term_set.remove elt l in
      (Term_set.iter (fun x ->
           fprintf fmt "@ ,@[<h>%a@]" fprint_term x) l)
    else fprintf fmt " ...";
    fprintf fmt " @]}"
  end
;;

let pretty_time fmt (i,j) = Format.fprintf fmt "<%d,%d>" i j

module Access_type = struct
  let pretty fmt t =
    let open Format in
    match t with
    | Transfer_functions_sig.Read -> fprintf fmt "read"
    | Transfer_functions_sig.Write -> fprintf fmt "write"
end

(* Parametric wrt. how each term is printed. *)
let pretty_kind ~fprint_mu fprint_term fmt kind =
  let open Format  in
  match kind with
    | True -> fprintf fmt "true"
    | False -> fprintf fmt "false"
    (* Print size of constants. *)
    | Biconst(size,k) -> fprintf fmt "(bin%d %s)" size @@ (if Z.numbits k < 8 then Z.to_string k else "0x" ^ Z.format "x" k)
    (* | Biconst(_size,k) -> Integer.pretty fmt ~hexa:false k *)
    | Beq(_,a,b) -> fprintf fmt "(%a =b= %a)" fprint_term a fprint_term b
    | Bisle(_,a,b) -> fprintf fmt "(%a <=bs %a)" fprint_term a fprint_term b
    | Biule(_,a,b) -> fprintf fmt "(%a <=bu %a)" fprint_term a fprint_term b
    | Bshift(_,k,_,b) -> fprintf fmt "(%a . %d)" fprint_term b k
    | Bindex(_,a,k,b) -> fprintf fmt "(%a . [%d * %a])" fprint_term a k fprint_term b
    | Biadd(_,_,_,a,b) -> fprintf fmt "(%a +b %a)" fprint_term a fprint_term b
    | Bisub(_,_,_,a,b) -> fprintf fmt "(%a -b %a)" fprint_term a fprint_term b
    | Bisdiv(_,a,b) -> fprintf fmt "(%a /bs %a)" fprint_term a fprint_term b
    | Biudiv(_,a,b) -> fprintf fmt "(%a /bu %a)" fprint_term a fprint_term b       
    | Bshl(_,_,_,a,b) -> fprintf fmt "(%a << %a)" fprint_term a fprint_term b
    | Blshr(_,a,b) -> fprintf fmt "(%a >>l %a)" fprint_term a fprint_term b
    | Bashr(_,a,b) -> fprintf fmt "(%a >>a %a)" fprint_term a fprint_term b
    | Bismod(_,a,b) -> fprintf fmt "(%a %%bs %a)" fprint_term a fprint_term b
    | Biumod(_,a,b) -> fprintf fmt "(%a %%bu %a)" fprint_term a fprint_term b
    | Bextract(size,index,t) -> fprintf fmt "(%a)[%d:%d]" fprint_term t index (index+size)
    | Buext(size,t) -> fprintf fmt "buext%d(%a)" size fprint_term t
    | Bofbool(size,t) -> fprintf fmt "bofbool%d(%a)" size fprint_term t                     
    | Valid(size,acctyp,t) -> fprintf fmt "\\valid_%a%d(%a)" Access_type.pretty acctyp size fprint_term t
    | Valid_ptr_arith(size,a,b) -> fprintf fmt "\\valid_ptr_arith%d(%a +b %a)" size fprint_term a fprint_term b
    | Bsext(size,t) -> fprintf fmt "bsext%d(%a)" size fprint_term t
    | Bunknown(size,t) -> fprintf fmt "bunknown%d_%a" size pretty_time t 
    | Buninit(size) -> fprintf fmt "buninit%d" size
    | Bconcat(size,a,b) -> fprintf fmt "(%a::%a)" fprint_term a fprint_term b
    | Bimul(_size,_,_,a,b) -> fprintf fmt "(%a *b %a)" fprint_term a fprint_term b
    | Bxor(_,a,b) -> fprintf fmt "(%a ^ %a)" fprint_term a fprint_term b
    | Bor(_,a,b) -> fprintf fmt "(%a | %a)" fprint_term a fprint_term b
    | Band(_,a,b) -> fprintf fmt "(%a & %a)" fprint_term a fprint_term b
    | And(a,b) -> fprintf fmt "(%a && %a)" fprint_term a fprint_term b
    | Or(a,b) -> fprintf fmt "(%a || %a)" fprint_term a fprint_term b
    | Not t -> fprintf fmt "(!%a)" fprint_term t
    | Offset t -> fprintf fmt "\\offset(%a)" fprint_term t
    | Itimes(n,t) -> fprintf fmt "(%s . %a)" (Z.to_string n) fprint_term t
    | Iconst(k) -> fprintf fmt "%s" @@ (if Z.numbits k < 8 then Z.to_string k else "0x" ^ Z.format "x" k)
    | Idiv(x,y) -> fprintf fmt "(%a / %a)" fprint_term x fprint_term y
    | Imod(x,y) -> fprintf fmt "(%a %% %a)" fprint_term x fprint_term y
    | Iadd(x,y) -> fprintf fmt "(%a + %a)" fprint_term x fprint_term y
    | Isub(x,y) -> fprintf fmt "(%a - %a)" fprint_term x fprint_term y
    | Ieq(x,y) -> fprintf fmt "(%a = %a)" fprint_term x fprint_term y
    | Ile(x,y) -> fprintf fmt "(%a <= %a)" fprint_term x fprint_term y
    | Imul(x,y) -> fprintf fmt "(%a * %a)" fprint_term x fprint_term y
    | Ishl(x,y) ->  fprintf fmt "(%a << %a)" fprint_term x fprint_term y
    | Ishr(x,y) ->  fprintf fmt "(%a >> %a)" fprint_term x fprint_term y
    | Iand(x,y) ->  fprintf fmt "(%a & %a)" fprint_term x fprint_term y
    | Ior(x,y) ->  fprintf fmt "(%a | %a)" fprint_term x fprint_term y
    | Ixor(x,y) ->  fprintf fmt "(%a ^ %a)" fprint_term x fprint_term y
    | Iunknown w -> fprintf fmt "iunknown_%a" pretty_time w
    | Memunknown w -> fprintf fmt "memunknown_%a" pretty_time w                      
    | Var (_, x,_) -> Var.pretty fmt x
    | Store(size,store,addr,value) -> 
      fprintf fmt "store%d( %a, %a, %a)"
        size fprint_term store fprint_term addr fprint_term value
    | Memcpy(size,store,from,to_) -> 
      fprintf fmt "memcpy%d( %a, %a -> %a)"
        size fprint_term store fprint_term from fprint_term to_
    | Malloc(id,size,mem) -> 
      fprintf fmt "malloc_%s( %d, %a)"
        (Transfer_functions.Malloc_id.to_string id) size fprint_term mem
    | Free(mem,ptr) ->
      fprintf fmt "free( %a, %a)"
        fprint_term mem fprint_term ptr
    | Load(size,store,addr) -> 
       fprintf fmt "load%d( %a, %a)" size fprint_term store fprint_term addr
    | Lambda(_var,_body) -> assert false
    | Separated(a,sizea,b,sizeb) -> 
      fprintf fmt "\\separated(%a, %d, %a, %d)"
        fprint_term a sizea fprint_term b sizeb
    | Mu(time,v,loop,init) -> fprint_mu fmt time v loop init
    | Assume(a,b) -> fprintf fmt "assume %a in %a" fprint_term a fprint_term b
    | If_not_bottom(s,b) ->
      fprintf fmt "if_not_bottom(%a,%a)" (pretty_set fprint_term) s fprint_term b
    | Nondet(w,types,l) ->
       let card = Term_set.cardinal l in
       if card == 0 then fprintf fmt "<bottom%a_%a>" pretty_time w pretty_types types
       else begin
         (* ? are like bullet points that show each choice. *)
         fprintf fmt "nondet_%a_%a{ @[<hv>" pretty_time w pretty_types types;
         if card < 10 then
           let elt = Term_set.choose l in
           fprintf fmt "?@[<h>%a@]" fprint_term elt;
           let l = Term_set.remove elt l in
           (Term_set.iter (fun mem ->
             fprintf fmt "@ ?@[<h>%a@]" fprint_term mem) l)
         else fprintf fmt " ...";
         fprintf fmt " @]}"
       end
    | Tuple_get( t, i) -> fprintf fmt "(%a)[%d]" fprint_term t i
    | Tuple_create(_, a) -> fprintf fmt "%a" (Immutable_dynamic_array.pp_dynarray ~pre:"[@[<hv>" ~suf:"@]]" ~sep:",@," fprint_term) a
    | Builtin(b,t) -> fprintf fmt "%a(%a)" Builtin.pretty b fprint_term t
    (* | k -> Term_utils.identify_kind "Term_pretty.pretty_kind" k *)
;;

(****************************************************************)

(* Simple printer: just print each term recursively, i.e. terms used
   twice are printed twice, rather than being shared. This can lead to
   exponential blowup, but does not depend on things like hashconsing
   (and is thus useful for debuging these functions). *)
let rec pretty_simple fmt t = pretty_kind ~fprint_mu:pretty_simple_mu pretty_simple fmt t.kind
and pretty_simple_mu fmt time v loop init =
  Format.fprintf fmt "(\\mu%a %a. %a)(%a)"
    pretty_time time pretty_simple v pretty_simple loop pretty_simple init

(****************************************************************)

(* Default printing strategy: gives a name only to stores and
   conditions. This strategy allows a close correspondance with the
   original source code (let ~= statements).

   TODO: Other strategies are possible; in particular, it would be
   interesting to use let when there is some sharing.
 *)

(* TODO: test type instead. *)
let default_named term = match term.kind with
  | Store(_,_,_,_) -> true
  | Memcpy(_,_,_,_) -> true    
  | Mu _ -> true
  | Assume(_,x) -> true (* (match x.kind with Var(_,_) -> true | _ -> default_named x) *)
  | Nondet(_,_,_) -> true
  | Builtin _ -> true
  | _ when Term_type_inference.type_of term == Memory -> true
  (* Do not introduce variable names for constants and simple unary operations. *)
  | Var _ -> false
  | True | False -> false
  | Not _ -> false    
  | Tuple_get _ -> false
  | Bextract _ -> false
  | Biconst(_,_) -> false
  | Buninit _ -> false
  | Iconst _ -> false
  | Itimes _ -> false
  (* Minimum condition to avoid combinatorial explosion in print. *)
  (* | _ when (Term_type.Parents.length_more_than term.parents 2) -> *)
  (* This is more predictable, and eventually readable. *)
  | _ -> true ;;

let default_print_name fmt term =
  match Term_type_inference.type_of term with
  | Memory -> Format.fprintf fmt "M%d" term.id
  | Binary _ -> Format.fprintf fmt "B%d" term.id
  | Boolean -> Format.fprintf fmt "b%d" term.id
  | Integer -> Format.fprintf fmt "i%d" term.id
  | Tuple _ -> Format.fprintf fmt "T%d" term.id

let pretty_custom
    ?(print_defs=true)
    ?(named=default_named)
    ?(print_name=default_print_name)
    ?(repl=fun x -> x)
    fmt term  = 
  let open Format in 

  (* We use two printers: one that substitute terms with their names,
     and one that does not. *)
  let rec print_term_or_name fmt term = 
    let term = repl term in
    if named term 
    then print_name fmt term
    else pretty_kind ~fprint_mu print_term_or_name fmt term.kind

  and print_term_no_name fmt term =
    pretty_kind ~fprint_mu print_term_or_name fmt (repl term).kind

  (* First traverse the initial term in DFS post-order to print the
     let-definitions of all named variables in order. *)
  and print_let_def fmt term =
    let term = repl term in
    if named term
    then fprintf fmt "let %a = %a@\n" print_name term print_term_no_name term

  and print_let_defs fmt term =
    Term_dag.postfix_dfs (print_let_def fmt) term

  and fprint_mu fmt time var loop init =
    fprintf fmt "(\\mu%a %a.@\n  @[%a%a@])(%a)"
      pretty_time time print_term_no_name var print_let_defs loop print_term_or_name loop print_term_or_name init
  in

  if print_defs then print_let_defs fmt term;
  print_term_or_name fmt term
;;

let pretty fmt = pretty_custom fmt;;
let pretty_short fmt = pretty_custom ~print_defs:false fmt;;

let pretty_kind fmt = pretty_kind fmt ~fprint_mu:(fun fmt time v loop init -> Format.fprintf fmt "mu");;

Term_type.ref_fprint_term := pretty;;

   (* TODO: Une fonction pour printer les let d'un ensemble de
      terme. Important pour printer les valeurs memoires correctement. *)

   (* TODO: Le nombre de parents, et le numero, pourraient etre determine
      lors d'une premiere passe; ca permettrait d'avoir une sortie sans 
      avoir tous les numeros qui changent *)
