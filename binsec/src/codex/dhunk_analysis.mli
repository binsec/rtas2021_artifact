module V : sig
  include Graph.Sig.VERTEX with type t = Dba.id

  val hash : t -> int
  val pretty : Format.formatter -> t -> unit
end

module E : sig
  include Graph.Sig.EDGE
    with type vertex = V.t
    and type t = V.t * V.t
    and type label = unit
  val pretty : Format.formatter -> t -> unit
end

module G : sig
  include Graph.Sig.P with module V = V and module E = E
end

module Dhunk_regex : Regex.S
  with type letter = G.E.t
  and type t = G.E.t Regex.tagged_regex
module Reduce : module type of Reduce.Make(G)(Dhunk_regex)
module Wto_alg : module type of Wto.Make(G.V)

(** For each outer jump instruction in a dhunk, return the path expression of
    that DBA instruction in the dhunk, and optionnally the target address, if it
    statically known. *)
val exit_regexes : Dhunk.t -> (Dba.id * Dhunk_regex.t * Virtual_address.t option) list
