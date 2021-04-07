(** A domain that abstracts a bitvector, trying to track whether it is equal to
   zero or not. *)
include Basis_sig.Binary_Integer_Basis

val is_zero : Binary_Lattice.t -> bool
val zero : Binary_Lattice.t
val nonzero : Binary_Lattice.t
