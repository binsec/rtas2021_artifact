(**************************************************************************)
(*                                                                        *)
(*  This file is part of Frama-C.                                         *)
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

(* The term language is typed; here is its type. Not to be confused with the [Term_type] module,
   which defines the OCaml (host) type for terms. *)


(* Sizes are always in bits. *)
type size = int


module Term_types_ = struct
  (* Terms are strongly typed. *)
  type t =
    | Boolean
    | Binary of size
    | Integer
    | Memory
    | Tuple of t Immutable_dynamic_array.t Lazy.t (* Can be slow to compute. *)

  let rec equal a b = match (a,b) with
    | Tuple a, Tuple b -> Immutable_dynamic_array.for_all2 equal
                            (Lazy.force_val a) (Lazy.force_val b)
    | _ -> a = b
  
  let hash = function
    | Boolean -> 0
    | Integer -> 1
    | Memory -> 2
    | Binary x -> 3 + x
    | Tuple _ -> assert false

  let compare a b = (hash a) - (hash b)
  
end

include Term_types_

type ('boolean,'integer,'binary,'memory) prod =
  { boolean: 'boolean;
    integer: 'integer;
    binary: size:int -> 'binary;
    memory: 'memory;
  }
;;

module Prod = struct


  (* Note that this requires that all the fields have the same type,
     which is often not what you want. *)
  let select typ x = match typ with
    | Boolean -> x.boolean
    | Binary(size) -> x.binary ~size
    | Memory -> x.memory
    | Integer -> x.integer
    | Tuple _ -> assert false

  let prod x y = { boolean = x.boolean, y.boolean;
                   integer = x.integer, y.integer;
                   binary = (fun ~size -> x.binary ~size, y.binary ~size);
                   memory = x.memory, y.memory;
                 };;

  let dup f = {
    boolean = f;
    integer = f;
    binary = (fun ~size -> f);
    memory = f;
  }

  (* Compose functions are the best way to implement per-type
     functions. Unfortunately, the restrictions on OCaml polymorphism
     get in the way. *)
  
  (* The best way to get around the absence of polymorphic argument is
     to call the f function several times. The duplication has to be
     done at the call site, else _ variables appear (e.g. let f x =
     (x,x) in f id creates underscore type variables). So passing a
     record or a tuple, or using dup, would not work here. *)
  let compose2 typ rec1 rec2 f1 f2 f3 f4 f5 =
    match typ with
    | Boolean -> f1 rec1.boolean rec2.boolean
    | Binary(size) -> f2 (rec1.binary ~size) (rec2.binary ~size)
    | Memory -> f3 rec1.memory rec2.memory
    | Integer -> f5 rec1.integer rec2.integer
    | Tuple _ -> assert false

  (* How to use it: do let f a b c = ... in compose3 typ a b c f f f f f; 
     f will be instantiated with the correct elements of the tuple. *)
  let compose3 typ rec1 rec2 rec3 f1 f2 f3 f4 f5 =
    match typ with
    | Boolean -> f1 rec1.boolean rec2.boolean rec3.boolean
    | Binary(size) -> f2 (rec1.binary ~size) (rec2.binary ~size) (rec3.binary ~size)
    | Memory -> f3 rec1.memory rec2.memory rec3.memory
    | Integer -> f5 rec1.integer rec2.integer rec3.integer
    | Tuple _ -> assert false

  
  (* This solution also works, but having to declare every type is
     heavy. Perhaps this will become better with modular implicits. *)
  module Compose2(F:sig
      type 'a arg1
      type 'a arg2
      type result
      val compose2: 'a arg1 -> 'a arg2 -> result
    end) = struct
    let f typ rec1 rec2 =
      match typ with
      | Boolean -> F.compose2 rec1.boolean rec2.boolean
      | Binary(size) -> F.compose2 (rec1.binary ~size) (rec2.binary ~size)
      | Memory -> F.compose2 rec1.memory rec2.memory
      | Integer -> F.compose2 rec1.integer rec2.integer
      | Tuple _ -> assert false
  end

  (* Yet another solution would be to have compose2 defined as a
     macro, i.e. to have macro-expansion done before type
     inference. This would be simple and type-safe; but macros are not
     supported by OCaml. Perhaps a ppx extension could be done. *)

  (* The last solution would be to use Obj.magic... And just make sure
     that the function works for booleans. *)
  
end

open Prod


module Sum = struct

  type ('boolean,'integer,'binary,'memory) t =
    | Boolean of 'boolean
    | Integer of 'integer
    | Binary of int * 'binary
    | Memory of 'memory

  (* Similar to Extlib.the *)
  let the = {
    boolean = (function Boolean x -> x | _ -> assert false);
    integer = (function Integer x -> x | _ -> assert false);
    memory = (function Memory x -> x | _ -> assert false);
    binary = (fun ~size x -> match x with
        | Binary(s,x) when s == size -> x 
        | _ -> assert false)
  }

  let id = {
    boolean = (fun x -> x);
    integer = (fun x -> x);
    memory = (fun x -> x);
    binary = (fun ~size x -> x);        
  }

  let id_not_memory = {id with memory = (fun _ -> assert false)}
  let id_not_binary = {id with binary = (fun ~size _ -> assert false)}                      

  let inj = {
    boolean = (fun x -> Boolean x);
    integer = (fun x -> Integer x);
    memory = (fun x -> Memory x);
    binary = (fun ~size x -> Binary(size,x));
  };;

  let apply f = function
    | Boolean x -> f.boolean x
    | Integer x -> f.integer x
    | Binary(size,x) -> f.binary ~size x
    | Memory x -> f.memory x
  ;;

  let apply_inj f = function
    | Boolean x -> Boolean(f.boolean x)
    | Integer x -> Integer(f.integer x)
    | Binary(size,x) -> Binary(size,f.binary ~size x)
    | Memory x -> Memory(f.memory x)
  ;;

  let to_type = function
    | Boolean _ -> Term_types_.Boolean
    | Binary(size,_) -> Term_types_.Binary size
    | Integer _ -> Term_types_.Integer
    | Memory _ -> Term_types_.Memory

  let get x = apply id x

end
