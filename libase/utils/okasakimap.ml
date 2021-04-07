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


module type S = sig
  type 'a t
  type key
  val empty: 'a t
  val singleton: key -> 'a -> 'a t
  val find: key -> 'a t -> 'a
  val insert : (old:'a -> value:'a -> 'a) -> key -> 'a -> 'a t -> 'a t
  val add : key -> 'a -> 'a t -> 'a t

  val unioni : (int -> 'a -> 'a -> 'a) -> 'a t -> 'a t -> 'a t
  val interi : (int -> 'a -> 'a -> 'a) -> 'a t -> 'a t -> 'a t  

  val union : ('a -> 'a -> 'a) -> 'a t -> 'a t -> 'a t
  val inter : ('a -> 'a -> 'a) -> 'a t -> 'a t -> 'a t

  val iter: 'a t -> (int -> 'a -> unit) -> unit
  (* val union_inter:
   *   funion:('a -> 'a -> 'a) ->
   *   finter:('a -> 'a -> 'a) -> 'a t -> 'a t -> 'a t -> 'a t *)
end

(* A mask is an integer with a single bit set (i.e. a power of 2). *)
type mask = int

(* Optimized computation, but does not work for values too high. *)
let _highest_bit v =
  (* compute highest bit. 
     First, set all bits with weight less than
     the highest set bit *)
  let v1 = v lsr 1 in
  let v2 = v lsr 2 in
  let v = v lor v1 in
  let v = v lor v2 in
  let v1 = v lsr 3 in
  let v2 = v lsr 6 in
  let v = v lor v1 in
  let v = v lor v2 in
  let v1 = v lsr 9 in
  let v2 = v lsr 18 in
  let v = v lor v1 in
  let v = v lor v2 in
  (* then get highest bit *)
  (succ v) lsr 1
;;

let lowest_bit x =
  x land (-x)

let rec highest_bit x =
  let m = lowest_bit x in
  if x = m then
    m
  else
    highest_bit (x - m)
;;

let highest_bit x =
  1 lsl (Z.log2 @@ Z.of_int x)

(* Note: in the original version, okasaki give the masks as arguments
   to optimize the computation of highest_bit. *)
let branching_bit a b = highest_bit (a lxor b);;

let mask i m =
  i land (lnot (2*m-1));;

type key = int

(* We provide two versions: with or without hash-consing. Hash-consing
   allows faster implementations for the fold_on_diff* operations.
   Benchmarks seems to indicate that hashconsing and the more complex
   fold_on_diff are not very useful in practice (perhaps they would on
   huge structures?) *)

module TNoHashCons:sig 
  type 'a t = private
    | Empty
    | Leaf of {key:key; value:'a}            (* The entire key. *)
    (* Branching bit contains only one bit set; the corresponding mask is
       (branching_bit - 1).  The prefixes are normalized: the bits
       below the branching bit are set to zero (i.e. prefix &
       (branching_bit - 1) = 0). *)
    | Branch of {prefix:key;branching_bit:mask;tree0:'a t;tree1:'a t}
  val empty: 'a t
  val leaf: key -> 'a -> 'a t
  val branch: prefix:key -> branching_bit:mask -> tree0:'a t -> tree1:'a t -> 'a t
  
end = struct

  type 'a t =
    | Empty
    | Leaf of {key:key;value: 'a}            (* The entire key. *)
    | Branch of {prefix:key;branching_bit:mask;tree0:'a t;tree1:'a t}

  let empty = Empty
  let leaf key value  = Leaf {key;value}
  let branch ~prefix ~branching_bit ~tree0 ~tree1 =
    match tree0,tree1 with
    | Empty, x -> x
    | x, Empty -> x
    | _ -> Branch{prefix;branching_bit;tree0;tree1}

end

(* With hash-consing of interior nodes: slower node construction, but
   faster comparison with fold_on_diff. *)
module THashCons:sig

  type 'a t = private
    | Empty
    | Leaf of {id:int;key:key; value:'a }            (* The entire key. *)
    (* Branching bit contains only one bit set; the corresponding mask is
       (branching_bit - 1).  The prefixes are normalized: the bits
       below the branching bit are set to zero (i.e. prefix &
       (branching_bit - 1) = 0). *)
    | Branch of {id:int;prefix:key;branching_bit:mask;tree0:'a t;tree1:'a t}

  val empty: 'a t
  val leaf: key -> 'a -> 'a t
  val branch: prefix:key -> branching_bit:mask -> tree0:'a t -> tree1:'a t -> 'a t
  
end = struct

  let id_count = ref 1;;
  
  type 'a t =
    | Empty
    | Leaf of {id:int;key:key;value: 'a}            (* The entire key. *)
    | Branch of {id:int;prefix:key;branching_bit:mask;tree0:'a t;tree1:'a t}
    (* Prefixes are normalized: the bits below the branching bit are
       set to zero. *)


  module WeakH = Weak.Make(struct
      type tmp = int list t         (* We cheat a bit: a polymorphic argument could be used here.. *)
      type t = tmp
      let equal a b = match a,b with
        | Branch{prefix=prefixa;branching_bit=branching_bita;tree0=tree0a;tree1=tree1a},
          Branch{prefix=prefixb;branching_bit=branching_bitb;tree0=tree0b;tree1=tree1b} ->
          prefixa == prefixb && branching_bita == branching_bitb && tree0a == tree0b && tree1a == tree1b
        | _ -> assert false

      let sdbm x y = y + (x lsl 16) + (x lsl 6) - x;;

      let get_id = function
        | Empty -> 0
        | Leaf{id} -> id
        | Branch{id} -> id
      
      let hash (Branch{prefix;branching_bit;tree0;tree1}) =
        sdbm (prefix lor branching_bit) @@ sdbm (get_id tree0) (get_id tree1)
      ;;
      
    end
    );;


  let weakh = WeakH.create 120;;

  let empty = Empty
  
  let leaf key value  =
    let id = !id_count in
    incr id_count;
    Leaf {id;key;value};;
  let branch ~prefix ~branching_bit ~tree0 ~tree1 =
    match tree0,tree1 with
    | Empty, x | x, Empty -> x
    | _ ->
      let id = !id_count in
      let tentative = Branch{id;prefix;branching_bit;tree0;tree1} in
      let v = WeakH.merge weakh (Obj.magic tentative) in
      let v = Obj.magic v in
      if v == tentative then ((* Codex_log.debug "HASHCONS WORKS"; *) incr id_count);
      v
  ;;
    
end

include TNoHashCons;;
(* include THashCons;; *)



(* Merge trees whose prefix disagree. *)
let join pa treea pb treeb =
  (* Printf.printf "join %d %d\n" pa pb; *)
  let m = branching_bit pa pb in
  let p = mask pa (* for instance *) m in
  if (pa land m) = 0 then
    branch ~prefix:p ~branching_bit:m ~tree0:treea ~tree1:treeb
  else
    branch ~prefix:p ~branching_bit:m ~tree0:treeb ~tree1:treea
;;

let create ~size value = leaf size value;;


(* [match_prefix k p m] returns [true] if and only if the key [k] has prefix [p] up to bit [m]. *)
let match_prefix k p m =
  mask k m = p
;;

let singleton = leaf 

let rec find searched = function
  | Leaf{key;value} when key == searched -> value
  | Empty | Leaf _ -> raise Not_found
  | Branch{prefix;branching_bit;tree0;tree1} ->
    (* Optional if not (match_prefix searched prefix branching_bit) then raise Not_found
    else *) if (branching_bit land searched == 0)
    then find searched tree0
    else find searched tree1
;;

let insert f thekey value t =
  (* Preserve physical equality whenever possible. *)
  let exception Unmodified in
  try 
    let rec loop = function
      | Empty -> leaf thekey value
      | Leaf{key;value=old} as t ->
        if thekey == key
        then
          let newv = f ~old ~value in
          if newv == old then raise Unmodified
          else leaf key newv
        else join thekey (leaf thekey value) key t
      | Branch{prefix;branching_bit;tree0;tree1} as t ->
        if match_prefix thekey prefix branching_bit then
          if (thekey land branching_bit) == 0
          then branch ~prefix ~branching_bit ~tree0:(loop tree0) ~tree1
          else branch ~prefix ~branching_bit ~tree0 ~tree1:(loop tree1)
        else join thekey (leaf thekey value) prefix t
    in loop t
  with Unmodified -> t
;;

let add key value t = insert (fun ~old ~value -> value) key value t;;

(* Requires f to be symetric, at least for now *)
let rec unioni f ta tb =
  match ta,tb with
  | _ when ta == tb -> ta
  | Empty, x | x, Empty -> x
  | Leaf{key;value},t -> insert (fun ~old ~value -> f key old value) key value t 
  | t,Leaf{key;value} -> insert (fun ~old ~value -> f key value old) key value t
  | Branch{prefix=pa;branching_bit=ma;tree0=ta0;tree1=ta1},
    Branch{prefix=pb;branching_bit=mb;tree0=tb0;tree1=tb1} ->
    if ma == mb && pa == pb
    (* Same prefix: merge the subtrees *)
    then
      (* MAYBE: if ta0 == tb0 and ta1 == tb1, we can return ta (or
         tb). Probably not useful. *)
      branch ~prefix:pa ~branching_bit:ma ~tree0:(unioni f ta0 tb0) ~tree1:(unioni f ta1 tb1)
    else if ma > mb && match_prefix pb pa ma
    then if ma land pb == 0
      then branch ~prefix:pa ~branching_bit:ma ~tree0:(unioni f ta0 tb) ~tree1:ta1
      else branch ~prefix:pa ~branching_bit:ma ~tree0:ta0 ~tree1:(unioni f ta1 tb)
    else if ma < mb && match_prefix pa pb mb
    then if mb land pa == 0
      then branch ~prefix:pb ~branching_bit:mb ~tree0:(unioni f ta tb0) ~tree1:tb1
      else branch ~prefix:pb ~branching_bit:mb ~tree0:tb0 ~tree1:(unioni f ta tb1)
    else join pa ta pb tb
;;

let rec interi f ta tb =
  match ta,tb with
  | _ when ta == tb -> ta
  | Empty, _ | _, Empty -> empty
  | Leaf{key;value},t ->
    (try let res = find key t in
      leaf key (f key value res)
     with Not_found -> empty)
  | t,Leaf{key;value} ->
    (try let res = find key t in
      leaf key (f key res value)
    with Not_found -> empty)
  | Branch{prefix=pa;branching_bit=ma;tree0=ta0;tree1=ta1},
    Branch{prefix=pb;branching_bit=mb;tree0=tb0;tree1=tb1} ->
    if ma == mb && pa == pb
    (* Same prefix: merge the subtrees *)
    then branch ~prefix:pa ~branching_bit:ma ~tree0:(interi f ta0 tb0) ~tree1:(interi f ta1 tb1)
    else if ma > mb && match_prefix pb pa ma
    then if ma land pb == 0
      then interi f ta0 tb
      else interi f ta1 tb
    else if ma < mb && match_prefix pa pb mb
    then if mb land pa == 0
      then interi f ta tb0
      else interi f ta tb1
    else empty
;;


let union f = unioni (fun _ a b -> f a b)
let inter f = interi (fun _ a b -> f a b)    

let rec iter x f = match x with
  | Empty -> ()
  | Leaf{key;value} -> f key value
  | Branch{tree0;tree1} -> iter tree0 f; iter tree1 f


(**************** Tests ****************)
let _m1 = singleton 7 1;;
let _m1 = add 3 2 _m1;;

let _m2 = singleton 4 3;;
let _m2 = add 10 4 _m2;;

let _m3 = union (Obj.magic 0) _m1 _m2;;

(* let m8 = m1;; *)
let _m4 = inter (fun a b -> a) _m1 _m3;;
let _m5 = inter (fun a b -> a) _m2 _m3;;

let _m6 = inter (fun a b -> a) _m1 _m2;;

module Make(Key:sig
  type t
  val to_int: t -> int
end) = struct
  type key = Key.t
  type 'a map = 'a t
  type 'a t = 'a map
  let empty = empty
  let singleton x v = singleton (Key.to_int x) v
  let find x m = find (Key.to_int x) m
  let insert f k v m = insert f (Key.to_int k) v m
  let add k v m = add (Key.to_int k) v m
  let union = union
  let inter = inter
  let unioni = unioni
  let interi = interi
  let iter = iter
  (* let union_inter = union_inter *)
end




(* (\* Union of ta with the intersection of tb and tc. Avoids double iteration on the tree
 *    and creating an intermediate tree. *\)
 * (\* XXX: Finir ca, en esperant que ca resolve mon probleme. Ca pourrait. *\)
 * let rec union_inter ~funion ~finter ta tb tc =
 *   match ta,tb,tc with
 *   | _ when tb == tc -> union funion ta tb
 *   (\* XXX: Do something if ta == tb or ta == tc? *\)
 *   (\* ta union (tb inter tc) = (ta inter tb) join (ta inter tc) *\)
 *   | Empty, _, _ -> inter finter tb tc
 *   | x, _, Empty -> x
 *   | x, Empty, _ -> x
 *   | Leaf{key;value},tb,tc -> insert (fun ~old ~value -> funion old value) key value (inter finter tb tc)
 *   | Branch _, Leaf{key=keyb;value=valueb}, Leaf{key=keyc;value=valuec} ->
 *     if keyb != keyc then ta
 *     else insert (fun ~old ~value -> funion value old) keyb (finter valueb valuec) ta
 *   | Branch _, Leaf{key;value},t | Branch _, t, Leaf{key;value} ->
 *     (try let res = find key t in
 *        let value = finter res value in
 *        insert (fun ~old ~value -> funion old value) key value ta
 *      with Not_found -> ta)
 *   | Branch{prefix=pa;branching_bit=ma;tree0=ta0;tree1=ta1},
 *     Branch{prefix=pb;branching_bit=mb;tree0=tb0;tree1=tb1},
 *     Branch{prefix=pc;branching_bit=mc;tree0=tc0;tree1=tc1} ->
 *     (\* Same prefix: merge the subtrees. *\)
 *     (\* if ma == mb && mb == mc && pa == pb && pb == pc
 *      * then branch ~prefix:pa ~branching_bit:ma ~tree0:(union_inter ~funion ~finter ta0 tb0 tc0) ~tree1:(union_inter ~funion ~finter ta1 tb1 tc1)
 *      * else
 *      *   union funion ta (inter finter tb tc) *\)
 *     if mb == mc then begin
 *       if pb != pc then ta
 *       else
 *       if ma == mb && pa == pb then (\* All equal prefixes. *\)
 *         branch ~prefix:pa ~branching_bit:ma
 *           ~tree0:(union_inter ~funion ~finter ta0 tb0 tc0)
 *           ~tree1:(union_inter ~funion ~finter ta1 tb1 tc1)
 *       else if ma > mb && match_prefix pb pa ma then (\* ma higher *\)
 *         if pb land ma == 0 then
 *           branch ~prefix:pa ~branching_bit:ma 
 *             ~tree0:(union_inter ~funion ~finter ta0 tb tc)
 *             ~tree1:ta1
 *         else
 *           branch ~prefix:pa ~branching_bit:ma
 *             ~tree0:ta0            
 *             ~tree1:(union_inter ~funion ~finter ta1 tb tc)
 *       else if match_prefix pa pb mb then (\* (mb == mc) > ma  *\)
 *         if pa land mb == 0
 *         then  branch ~prefix:pb ~branching_bit:mb ~tree0:(union_inter ~funion ~finter ta tb0 tc0) ~tree1:(inter finter tb1 tc1)
 *         else branch ~prefix:pb ~branching_bit:mb ~tree0:(inter finter tb0 tc0) ~tree1:(union_inter ~funion ~finter ta tb1 tc1)
 *       else (\* prefixes do not match. *\)            
 *         join pa ta pb (inter finter tb tc)
 *     end
 *     else union funion ta @@ inter finter tb tc
 *     (\* else if mb > mc then
 *      *   if ma == mb then
 *      *     assert false            (\\* ma and mb higher *\\)
 *      *   else if ma > mb then
 *      *     assert false            (\\* ma higher *\\)
 *      *   else
 *      *     assert false            (\\* mb higher. *\\)
 *      * else (\\* mb < mc *\\)
 *      *   if ma == mc then
 *      *     assert false            (\\* ma and mc higher *\\)
 *      *   else if ma > mc then
 *      *     assert false            (\\* ma higher *\\)
 *      *   else
 *      *     assert false            (\\* mc higher *\\) *\)
 *         
 *     (\* if ma > mb then
 *      *   if mb == mc && pb == pc then
 *      *   if mb > mc then assert false
 *      *     
 *      * 
 *      *   assert false *\)
 * ;;
 * 
*)

