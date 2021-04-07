(** Interface for directed graphs with labeled vertices. The reduction
 * algorithm reduces graphs into a regex over the alphabet of edge values (be
 * it edge labels, or pairs of vertices if the edges are unlabeled).  The input
 * graph must be without self-loops, i.e. edges of the form (n,n). *)
module type GraphI = sig
  type t
    (** CFG type *)

  module V : sig
    type t
      (** Vertex labels *)

    val equal : t -> t -> bool
    val hash : t -> int
    val pretty : Format.formatter -> t -> unit
  end

  module E : sig
    type t
    val src : t -> V.t
    val dst : t -> V.t
  end

  (** Fold over the incoming edges of a node *)
  val fold_pred_e : (E.t -> 'a -> 'a) -> t -> V.t -> 'a -> 'a
end

module Make (G : GraphI) (R : Regex.S with type letter = G.E.t) : sig
  type state = (G.V.t, R.t) Hashtbl.t

  val compute_exprs : G.t -> G.V.t Wto.partition -> state
end
