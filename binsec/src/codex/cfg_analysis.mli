(** A graph of (address, call_stack) pair that can be translated to regular
    expressions. It can be used to store a graph of instructions or a graph of
    basic blocks. *)

module V : sig
  type call_stack = Virtual_address.t list
  type t = Virtual_address.t * call_stack
  include Graph.Sig.VERTEX with type t := t

  val pretty : Format.formatter -> t -> unit
  val pp_stack : Format.formatter -> call_stack -> unit

  include Sigs.Collection with type t := t
end

module E : sig
  include Graph.Sig.EDGE
    with type vertex = V.t
    and type t = V.t * V.t
    and type label = unit
  val equal : t -> t -> bool
  val pretty : Format.formatter -> t -> unit
end

module Cfg : sig
  include Graph.Sig.I with module V = V and module E = E
end

module CfgRegex : Regex.S
  with type letter = Cfg.E.t
  and type t = Cfg.E.t Regex.tagged_regex
module Reduce : module type of Reduce.Make(Cfg)(CfgRegex)
module Wto : module type of Wto.Make(Cfg.V)
