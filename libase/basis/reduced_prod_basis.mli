module type Reduce = sig
  type t1
  type t2
  val reduce : size:int -> t1 * t2 -> t1 * t2
end

(** Reduced product domain. The folding operation [binary_fold_crop] is
   forwarded as-is to the second domain, so the domain most precise for folding
   should be put second.  *)
module type Reduced_prod_functor = functor
  (B1 : Basis_sig.Binary_Integer_Basis)
  (B2 : Basis_sig.Binary_Integer_Basis)
  (R : Reduce with
    type t1 = B1.binary
    and type t2 = B2.binary)
  -> Basis_sig.Binary_Integer_Basis

(** A domain that abstracts a bitvector, trying to track whether it is equal to
   zero or not. *)
module Make : Reduced_prod_functor
