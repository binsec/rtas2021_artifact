module type Letter = sig
  type t
  val equal : t -> t -> bool
  val pretty : Format.formatter -> t -> unit
  val hash : t -> int
end

(** Generic signature for a regex interface. *)
module type S = sig
  type letter

  type t

  val empty : t
  val epsilon : t

  val join : t -> t -> t
  val append : t -> letter -> t
  val append_star : t -> t -> t
  val equal : t -> t -> bool
  val hash : t -> int
  val pretty : Format.formatter -> t -> unit

  (** Print a regex in reverse. Useful sometimes when a regex is too big to be
   * fully printed, and you only want to see the end, you can start printing it
   * in reverse and interrupt the program. *)
  val pretty_reverse : Format.formatter -> t -> unit
end

(** Tagged regular expressions.  The tag is used for cons-hashing and share as
    many data structures as possibles. Tags should not be used outside of the
    implementation. *)
type 'l tagged_regex = private
  | Empty
  | Epsilon
  | Join of int * 'l tagged_regex * 'l tagged_regex
  | Append of int * 'l tagged_regex * 'l
  | AppendStar of int * 'l tagged_regex * 'l tagged_regex

module Make (L : Letter) : S
    with type letter = L.t
    and type t = L.t tagged_regex
