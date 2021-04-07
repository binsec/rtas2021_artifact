open Wto

module type Node = sig
  type t
  val equal: t -> t -> bool
  val hash: t -> int
  val pretty: Format.formatter -> t -> unit
end

module Make(N : Node) : sig
  (** [map f wto] applies [f] to every element of [wto] while conserving the
      w.t.o. structure. The second argument of [f] is [true] if the argument of
      [f] is a head. *)
  val map : (N.t -> bool -> 'a) -> N.t partition -> 'a partition

  (** [iter f wto] applies [f] to every element of [wto]. The second argument
      of [f] is true if the argument of [f] is a head. *)
  val iter : (N.t -> bool -> unit) -> N.t partition -> unit

  (** [head_of wto h n] returns whether [n] is inside the component of head
      [h]. [h] must be a head. Returns true also if [n = h].
      Complexity: linear in the number of nodes. *)
  val is_head_of : N.t partition -> N.t -> N.t -> bool

  (** Maximum number of heads among all nodes, i.e. maximum number of nested
      components. *)
  val depth : N.t partition -> int
end
