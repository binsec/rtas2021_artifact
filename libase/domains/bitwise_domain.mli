module type Subdomain = Domain_sig.Base

(** A domain that abstracts a bitvector, and remembers what bitwise operations
   have been applied to it. Can be a very efficient way to simplify sequences
   of bitwise operations. *)
module Make (Sub : Subdomain) :
  Domain_sig.Base
  with module Context = Sub.Context
  and type boolean = Sub.boolean

module Make_with_types (D : With_focusing.Base_with_types) : With_focusing.S_with_types2
  with module Context = D.Context
  and type boolean = D.boolean
  and module Type = D.Type
