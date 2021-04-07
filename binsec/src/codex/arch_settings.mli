(** This module signature describes two rather distinct components:
    - a description of the architecture registers, along with the initial abstract values that they should contain
    - other settings for the analysis such as a hook table.

    All these signatures are parameterized by an abstract domain.
*)

module type Registers = sig
  module Domain : Codex.Domain_sig.Base
  val registers : (string * int) list
  val initial_value : Domain.Context.t -> string * int -> Domain.binary
end

module type S = functor
  (Domain : Codex.With_focusing.Base_with_types2)
  (EvalPred : Codex.Type_domain.EvalPred.Sig with module Domain := Domain) -> sig

  module Registers : Registers with module Domain = Domain
end
