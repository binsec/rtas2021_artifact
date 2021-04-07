open Wto

module type Node = sig
  type t
  val equal: t -> t -> bool
  val hash: t -> int
  val pretty: Format.formatter -> t -> unit
end

module Make(N : Node) = struct

  let rec map_aux f acc = function
    | [] -> acc
    | Node n :: rest ->
        map_aux f (Node (f n false) :: acc) rest
    | Component (h, l) :: rest ->
        let new_head = f h true in
        let new_comp = map_aux f [] l in
        map_aux f (Component (new_head, new_comp) :: acc) rest

  let map f partition = map_aux f [] partition

  let rec iter f = function
    | [] -> ()
    | Node n :: rest ->
        f n false; iter f rest
    | Component (h, l) :: rest ->
        f h true; iter f l; iter f rest

  let rec flatten_aux acc partition =
    let r_flattened = List.fold_left
      (fun acc -> function
        | Node n -> n :: acc
        | Component (h, l) -> flatten_aux (h :: acc) l)
      acc
      partition
    in
    List.rev r_flattened

  let flatten partition = flatten_aux [] partition

  let rec is_head_of partition h n =
    match partition with
    | [] -> raise (Invalid_argument "Wto_utils.head_of")
    | Node n :: rest -> is_head_of rest h n
    | Component (h', l) as c :: rest ->
        if N.equal h h'
        then List.exists (N.equal n) @@ flatten [c]
        else is_head_of l h n || is_head_of rest h n

  let rec depth_aux acc = function
    | [] -> acc
    | Node _ :: rest -> depth_aux acc rest
    | Component (_,body) :: rest ->
        depth_aux (max acc (depth_aux acc body)) rest

  let depth partition = depth_aux 0 partition

end
