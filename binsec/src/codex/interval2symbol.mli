type 'a t
val empty: 'a t

(* Insert; and tell what to do when there are several values at the
   same address. *)
val insert: merge:(old:'a -> 'a -> 'a) -> int -> 'a -> 'a t -> 'a t
val find: int -> 'a t -> 'a
