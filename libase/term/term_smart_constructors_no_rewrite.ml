(**************************************************************************)
(*                                                                        *)
(*  This file is part of the Codex plugin of Frama-C.                     *)
(*                                                                        *)
(*  Copyright (C) 2007-2015                                               *)
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

let time_counter = ref 0;;
let fresh_time ~level =
  incr time_counter;
  (level,!time_counter)
;;

let intern = Term_hashcons.intern;;

let true_ = intern True
let false_ = intern False
let pretty = Term_pretty.pretty

let tuple_create array =
  let types = lazy (Codex_log.performance_warning "tuple_create: this map could be avoided";
                    Immutable_dynamic_array.map Term_type_inference.type_of array) in
  intern(Tuple_create(types, array));;

let tuple_get tuple i = intern(Tuple_get(tuple,i))

(* Appends a new type to a tuple_type given to a variable. This is the
   only case when the reference to the type of a variable may be
   safely updated. *)
let append_type var typ =
  match var.kind with
  | Var({contents=typ_ar} as ref,v,_) -> ref := Immutable_dynamic_array.append typ_ar typ
  | _ -> assert false           (* Should be a variable *)

let except tuple i value =
  let Term_types.Tuple types = Term_type_inference.type_of tuple in
  let length = Immutable_dynamic_array.length (Lazy.force_val types) in
  assert( 0 <= i);
  assert (i < length);
  let array = Immutable_dynamic_array.init length (fun j ->
    if j == i then value
    else tuple_get tuple j)
  in tuple_create array
;;

(* let bottom_world = intern(Worldvar(Var.create())) *)
let tuple_bottom types = intern(Nondet((-1,-1),types,Term_set.empty));;
  
let bottom typ = tuple_get (tuple_bottom (Immutable_dynamic_array.singleton typ)) 0;;

let if_not_bottom a b = intern(If_not_bottom(a,b));;

let big_int = Z.shift_left Z.one 128;;

let iconst k =
  if(Z.gt k big_int) then
    Codex_log.warning (* ~once:true *) "Constructing a big integer: %s " (Z.to_string k);
  (* assert(Integer.le k big_int); *)
  intern(Iconst k)

let itimes k a = intern(Itimes(k,a))
let iadd a b  = intern(Iadd(a,b));;

let zero = iconst Z.zero
let one = iconst Z.one

let imul a b = intern(Imul(a,b))
let idiv a b = intern(Idiv(a,b))
let imod a b = intern(Imod(a,b))
let ishl a b = intern(Ishl(a,b))
let ishr a b = intern(Ishr(a,b))
let ior a b = intern(Ior(a,b))
let ixor a b = intern(Ixor(a,b))
let iand a b = intern(Iand(a,b))
let isub a b = intern(Isub(a,b))
let ieq a b = intern(Ieq(a,b))
let ile a b = intern(Ile(a,b))            

(* Derived constructors. *)
let igt a b = intern(Not(ile a b))
let ige a b = ile b a
let ilt a b = igt b a    
let iunknown () = (* intern(Iunknown w) *) assert false
let iunknown_explicit ~level = intern(Iunknown (fresh_time ~level))  

let biconst ~size i = intern (Biconst( size, i))
let biadd ~size ~nsw ~nuw t1 t2 = intern(Biadd(size,nsw,nuw,t1,t2))

let bisub ~size ~nsw ~nuw t1 t2 = intern(Bisub(size,nsw,nuw,t1,t2))     

let bimul ~size ~nsw ~nuw t1 t2 =
  let size1 = Term_type_inference.size_of t1 in
  let size2 = Term_type_inference.size_of t2 in
  assert (size1 == size2);
  intern(Bimul(size,nsw,nuw,t1,t2))

let bashr ~size a b = intern (Bashr(size,a,b))
let blshr ~size a b = intern (Blshr(size,a,b))
let bshl ~size ~nsw ~nuw a b = intern (Bshl(size,nsw,nuw,a,b))

let band ~size a b = intern (Band(size,a,b))
let bor ~size a b = intern (Bor(size,a,b))
let bxor ~size a b = intern (Bxor(size,a,b))

let beq ~size t1 t2 = 
  let size1 = Term_type_inference.size_of t1 in
  let size2 = Term_type_inference.size_of t2 in
  assert (size1 == size2);
  assert( size == size1);
  (intern (Beq( size1, t1, t2)))
;;

let bisle ~size t1 t2 =
  let size1 = Term_type_inference.size_of t1 in
  let size2 = Term_type_inference.size_of t2 in
  assert (size1 == size2);
  assert (size == size1);
  (intern (Bisle( size1, t1, t2)))
;;

let biule ~size t1 t2 =
  let size1 = Term_type_inference.size_of t1 in
  let size2 = Term_type_inference.size_of t2 in
  assert (size1 == size2);
  assert (size == size1);
  (intern (Biule( size1, t1, t2)))
;;

let bisdiv ~size t1 t2 =
  let size1 = Term_type_inference.size_of t1 in
  let size2 = Term_type_inference.size_of t2 in
  assert (size1 == size2);
  (intern (Bisdiv( size1, t1, t2)))
;;

let bismod ~size t1 t2 =
  let size1 = Term_type_inference.size_of t1 in
  let size2 = Term_type_inference.size_of t2 in
  assert (size1 == size2);
  (intern (Bismod( size1, t1, t2)))
;;

let biudiv ~size t1 t2 =
  let size1 = Term_type_inference.size_of t1 in
  let size2 = Term_type_inference.size_of t2 in
  assert (size1 == size2);
  (intern (Biudiv( size1, t1, t2)))
;;

let biumod ~size t1 t2 =
  let size1 = Term_type_inference.size_of t1 in
  let size2 = Term_type_inference.size_of t2 in
  assert (size1 == size2);
  (intern (Biumod( size1, t1, t2)))
;;

let bextract ~size ~index ~oldsize t =
  let tsize = Term_type_inference.size_of t in
  assert (tsize == oldsize);
  assert (index + size <= tsize);
  assert (index >= 0);
  assert (size > 0);
  intern( Bextract(size,index,t))

let bconcat ~size1 ~size2 a b =
  assert (size1 == Term_type_inference.size_of a);
  assert (size2 == Term_type_inference.size_of b);  
  intern(Bconcat(size1+size2,a,b))
;;


let buext ~size ~oldsize t =
  assert (oldsize = Term_type_inference.size_of t);
  assert (size > Term_type_inference.size_of t);
  intern( Buext(size,t))
;;

let bsext ~size ~oldsize t =
  assert (oldsize = Term_type_inference.size_of t);
  assert (size > Term_type_inference.size_of t);  
  intern( Bsext(size,t))
;;

let bofbool ~size x =
  assert (size >= 1);
  intern( Bofbool(size,x))
;;

let bchoose ~size _ = assert false

let separated a sizea b sizeb = 
  assert false


let load ~size mem addr = intern(Load(size,mem,addr));;
let malloc ~id ~malloc_size mem =
  let tuple = intern(Malloc(id,malloc_size,mem)) in
  tuple_get tuple 0, tuple_get tuple 1
;;

let free mem ptr = intern(Free(mem,ptr));;
  

let memcpy ~size mem from to_ =
  intern (Memcpy(size,mem,from,to_));;

let store ~size mem addr value =
  assert(size == Term_type_inference.size_of value);
  intern (Store(size,mem,addr,value));;

let real_nondet ~level typ s = intern(Nondet(fresh_time level,typ, s));;

let nondet ~level typ l =
  let s = (Term_set.of_list l) in
  real_nondet ~level typ s;;

let new_var ~level typ =  intern(Var(ref typ,Var.create(),level));;

let offset t = intern(Offset(t));;

let mu_explicit ~var:vterm ~body:loop ~level  ~arg:init =
  intern(Mu(fresh_time level,vterm,loop,init));;

let valid ~size access_type bin =  intern(Valid(size, access_type, bin));;
let valid_ptr_arith ~size x y =  intern(Valid_ptr_arith(size,x,y));;

let (&&) a b = intern(And(a,b))
let (||) a b = intern(Or(a,b))
let not t = intern( Not(t));;
let assume cond expr = intern(Assume(cond,expr));;


let buninit ~size = intern (Buninit(size))
    
(**************** Organize the smart constructors. ****************)
(* TODO: We should define them directly here. *)

open Term_type
open Term_types

module Boolean =
struct
  type boolean = term
  let false_ = false_
  let true_ = true_
  let unknown ?level = assert false
  let assume = assume
  let (||) = (||)
  let (&&) = (&&)
  let not = not

  let bottom = bottom Boolean
end

module Integer =
struct
  type boolean = term
  type integer = term
  let ieq = ieq
  let ige = ige
  let iconst = iconst
  let itimes = itimes
  let iadd = iadd
  let isub = isub
  let imul = imul
  let idiv = idiv
  let imod = imod
  let ishl = ishl
  let ishr = ishr
  let ior = ior
  let ixor = ixor
  let iand = iand
  let assume = assume

  let isub = isub
  let ieq = ieq
  let zero = zero
  let one = one
  let ile = ile
  let ilt = ilt
  let iunknown () = assert false
  let iunknown_explicit = iunknown_explicit

  let bottom = bottom Integer
end


module Binary =
struct
  type boolean = term
  type binary = term
  let biconst = biconst
  let assume ~size = assume
  let bismod = bismod
  let bisdiv = bisdiv
  let biumod = biumod
  let biudiv = biudiv
  let bsext = bsext
  let buext = buext
  let bofbool = bofbool
  let bchoose = bchoose
  let bextract = bextract
  let bconcat = bconcat
  let biule = biule
  let bisle = bisle
  let beq = beq
  let biadd = biadd
  let bisub = bisub
  let bimul = bimul
  let bshl = bshl
  let blshr = blshr
  let bashr = bashr
  let band = band
  let bor = bor
  let bxor = bxor
  let valid = valid
  let bshift ~size ~offset ~max x = intern(Bshift(size,offset,max,x))
  let bindex ~size scale x ind = (* the expression should be converted to pointer size. *)
    let size1 = Term_type_inference.size_of x in
    let size2 = Term_type_inference.size_of ind in
    assert (size1 == size2);
    assert (size1 == size);
    intern(Bindex(size,x,scale,ind))      
  let valid ~size access_type bin =  intern(Valid(size, access_type, bin));;
  let valid_ptr_arith ~size x y =  intern(Valid_ptr_arith(size,x,y));;
  
  module Ref_Addr = Ref_Addr

  let bunknown ~size = assert false
  let bunknown_explicit ~size ~level = intern (Bunknown(size,fresh_time ~level))
  let buninit ~size  = intern (Buninit(size))
  let bottom ~size = bottom (Binary size);;
end

module Memory =
struct
  type memory = term
  type binary = term
  type boolean = term

  let assume = assume
  let store = store
  let memcpy = memcpy
  let load = load
  let malloc = malloc
  let free = free
  let builtin builtin mem = intern (Builtin(builtin,mem))
  let unknown ~level = intern(Memunknown(fresh_time ~level))

  let bottom  = bottom Memory
end

module Tuple = struct
  let var ~level = new_var ~level (Immutable_dynamic_array.empty())
  let append_type = append_type
  let except = except
  let tuple_create = tuple_create
  let tuple_get = tuple_get
  let mu_explicit = mu_explicit
  let mu _ = assert false
  let assume = assume
  let nondet = nondet
  let builtin b t = intern(Builtin(b,t))
end
