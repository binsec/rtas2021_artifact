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

(****************************************************************)

(* Return the list of immediate subterms of a repr.  *)
(* Note that as "world" is used solely to discriminate terms, they
   are not counted as terms. *)
let subterms_kind = function
  | True | False | Var _ | Biconst _ | Buninit _
  | Iconst _ | Bunknown _ | Iunknown _ | Memunknown _ -> []
  | Not t | Itimes(_,t) | Lambda(_,t) | Builtin(_,t) | Offset t
  | Bextract(_,_,t) |  Bsext(_,t) | Buext(_,t) | Valid(_,_,t) | Bofbool(_,t)
  | Bshift(_,_,_,t) -> [t]
  | Idiv(x,y) | Imod(x,y) | Imul(x,y) | Iadd(x,y) | Isub(x,y)
  | Ishl (x,y) | Ishr (x,y) | Iand (x,y) | Ior (x,y) | Ixor (x,y)
  | And(x,y) | Or(x,y)
  | Ieq(x,y) | Ile(x,y)
  | Free(x,y)
  | Load(_,x,y) | Separated(x,_,y,_) | Beq(_,x,y) | Assume(x,y) | Bindex(_,x,_,y)
  | Bisle(_,x,y) | Biule(_,x,y) | Bconcat(_,x,y)
  | Biadd(_,_,_,x,y)  | Bisub(_,_,_,x,y)  | Bimul(_,_,_,x,y) | Band(_,x,y) | Bor(_,x,y) | Bxor(_,x,y)
  | Bisdiv(_,x,y) | Biudiv(_,x,y) | Bismod(_,x,y) | Biumod(_,x,y)
  | Bashr(_,x,y) | Blshr(_,x,y) | Bshl(_,_,_,x,y) -> [x;y]
  | Nondet(_,_,l) -> (Term_set.elements l)
  | If_not_bottom(l,x) -> x::(Term_set.elements l)
  | Store(_size,store,addr,value) | Memcpy(_size,store,addr,value) -> [store;addr;value]
  (* Note: the "loop" part of the mu is not a subterm of mu; at least
     it does not need to be evaluated before the mu term. *)
  | Mu(_time,_var,_loop,init) -> [init]
  | Malloc(_,_,mem) -> [mem]
  | Tuple_get(t,i) -> [t]
  | Tuple_create(_, l) -> Immutable_dynamic_array.to_list l
;;

let subterms term = subterms_kind term.kind;;

(* Dfs iteration. *)
module Dfs = Graph.Traverse.Dfs(struct
  let is_directed = true
  type t = term
  module V = Term_type.Term_datatype
  let iter_vertex f graph = f graph
  let fold_vertex f graph accu = f graph accu
  let iter_succ f _ term = List.iter f (subterms term)
  let fold_succ f _ term accu = List.fold_right f (subterms term) accu
end)

let postfix_dfs = Dfs.postfix;;



(****************************************************************)
module Worklist(O:Binary_heap.Ordered) = struct

  (* XXX: Attention, ce binary heap peut rajouter un meme terme
     plusieurs fois donc ca va pas bien pour une worklist; ca peut
     meme etre tres couteux.  *)
  (* include Binary_heap.Make(O);; *)
  module Set = Set.Make(O);;

  (* Emulate the Binary_heap api *)
  let create _ = ref @@ Set.empty;;
  let add s x = s := Set.add x !s;;
  let pop_maximum s =
    try
      let elt = Set.max_elt !s in
      s := Set.remove elt !s;
      elt
    with _ -> raise Binary_heap.Empty
  ;;
  

    (* f: elt -> list of elts; init: list of elements.  *)
    let worklist init f =
      let heap = create 17 in
      List.iter (fun elt -> add heap elt) init;
      try
        while true do
          let next = pop_maximum heap in
          let new_ = f next in
          List.iter (fun elt -> add heap elt) new_
        done
      with Binary_heap.Empty -> ()
end

  
(* From a term to its child node; topological sort. *)
module To_Subterms_Queue = Worklist(struct
  include Term_datatype;;
  let compare a b = a.id - b.id
end)

(* From a term to its parent node; reverse topological order. *)
module From_Subterms_Queue = Worklist(struct
  include Term_datatype;;
  let compare a b = b.id - a.id
end)

(* From a term to the children node, but remain in a loop until it is
   finished (this prevents having to redo the part before the loop
   several time).  *)
module To_Subterms_Queue_With_Level = Worklist(struct
    include Term_datatype
    let compare a b =
      let lvla = Term_utils.max_level a and lvlb = Term_utils.max_level b in
      let sub = lvla - lvlb in
      if sub != 0 then sub
      else a.id - b.id
  end
)

(* let to_subterms_queue_worklist = To_Subterms_Queue_With_Level.worklist *)

(* Note: no need for a with_level version for the From_Subterms_Queue, as 
   construction order (the ids) guarantee that the inner of a loop was built 
   before any term that use the result of a loop. *)




(* Iterate in reverse topological order, i.e. from subterms to their parents. *)
(* XXX: Remplacer ca par un simple DFS. *)
(* XXX: Mais filter est utile; si on appelle pour plusieurs assertiosn a la suite,
   la plupart du code sera commun. *)
let reverse_topological_iter term_list filter f =

  (* Find the set of nodes siblings of the nodes in [term_list], using
     a DFS traversal. Note: DFS postfix ordering is equivalent to
     topological iteration when there is a single root, so if
     term_list only has one element (common case), we can optimize
     this operation. *)
  let module G = struct
      type t = Term_type.term list
      let is_directed = true
      module V = Term_type.Term_datatype
      let iter_vertex = List.iter
      let fold_vertex = List.fold_right
      let iter_succ f _ t =
	let subterms = subterms t in
	List.iter f (List.filter filter subterms);;
      let fold_succ _f _ _t _acc = assert false
    end in

  (* Note: Ocamlgraph.Dfs works well when only the roots of the graph
       are given. But this is undocumented. *)
  let module Dfs = Graph.Traverse.Dfs(G) in

  (* Note: a queue is not needed here, and a simple set would suffice. *)
  let queue = From_Subterms_Queue.create 17 in
  Dfs.postfix (fun t -> From_Subterms_Queue.add queue t) (List.filter filter term_list);
  try while true do
      let elt = From_Subterms_Queue.pop_maximum queue in
      (* Codex_log.feedback "doing term %d" elt.id; *)
        f elt
      done
  with Binary_heap.Empty -> ()

;;


let topological_worklist = To_Subterms_Queue_With_Level.worklist;;
let reverse_topological_worklist = From_Subterms_Queue.worklist;;

module Term_Hashtbl = Term_type.Term_hashtbl;;

(* Guarantees that all children are called before the parent is called. *)
let dfs_postfix_iter dag f =
  let visited = Term_Hashtbl.create 17 in
  let rec loop node =
    Term_Hashtbl.replace visited node ();
    let succs = subterms node in
    succs |> List.iter (fun succ ->
        if not (Term_Hashtbl.mem visited succ) then loop succ);
    f node
  in
  loop dag
;;

(* Guarantees that all parents are called before the children is called. *)
let topological_iter dag f =
  let l =
    let stack = ref [] in
    dfs_postfix_iter dag (fun x -> stack := x::!stack);
    !stack
  in
  List.iter f l
;;

module To_Subterms_Dataflow(L:sig
    type t
    val join: t -> t -> t                        (* Used when a term is the subterm of two terms. *)
    (* val widen: previous:t -> t -> t              (\* Used in mus. *\) *)
    val includes: t -> t -> bool                 (* Used in mus. *)
    val pretty: Format.formatter -> t -> unit    (* Pretty-printer *)
    val transfer: term -> t -> (term * t) list   (* Returns the new value for the subterms. *)
  end) = struct

  module Term_hashtbl = Term_type.Term_hashtbl


  let compute term value =
    let result = Term_hashtbl.create 17 in
    let add n l =
      let new_ = try
          let old =  Term_Hashtbl.find result n in
          L.join old l
        with Not_found -> l
      in
      Term_Hashtbl.replace result n new_
    in
    let mu_stack = ref [] in
    Term_Hashtbl.replace result term value;
    topological_worklist [term] (fun term ->
        match term.kind with
        | Mu(_,var,body,init) ->
          let get = Term_Hashtbl.find result term in
          Term_Hashtbl.replace result body get;
          mu_stack := (var,body,init,term)::!mu_stack;
          [body]
        | Var(_,_,lvl) -> begin
            match !mu_stack with
            | [] -> assert (lvl == 0); []
            | (var,body,init,mu)::rest ->
              let var_get = Term_Hashtbl.find result term in
              let body_get = Term_Hashtbl.find result body in
              let mu_get = Term_Hashtbl.find result mu in
              let join = L.join var_get mu_get in
              if L.includes body_get join 
              then begin
                Term_Hashtbl.replace result init join;
                mu_stack := rest;
                [init] (* Fixpoint over *)
              end
              else begin
                Term_Hashtbl.replace result body join;
                [body] (* Another round. *)
              end
          end
        | _ ->
          let get = Term_Hashtbl.find result term in
          let res = L.transfer term get in
          res |> List.iter (fun (subterm,res) -> add subterm res);
          List.map fst res
      );
    (* Kernel.feedback "Result is"; *)
    (* result |> Term_Hashtbl.iter (fun term v -> *)
    (*     Kernel.feedback "%d -> %a" term.id L.pretty v *)
    (*   ); *)
    result
  ;;
  
end

  
