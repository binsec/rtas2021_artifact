module type Subdomain = Codex.Domain_sig.Base

(** A domain that abstracts a bitvector, and remembers what bitwise operations
   have been applied to it. Can be a very efficient way to simplify sequences
   of bitwise operations. *)
module Make (Sub : Subdomain) :
  Codex.Domain_sig.Base
  with module Context = Sub.Context
  (*and type binary = D.binary*)
  and type boolean = Sub.boolean
