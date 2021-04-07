module Sentinel = Sentinel_basis

(** Reduction between sentinels and {Ival}s. *)
module Reduce = struct
  type t1 = Sentinel.binary
  type t2 = Ival_basis.binary

  let reduce ~size (sntl,ival) =
    let ival_zero = Ival_basis.Binary_Forward.biconst ~size Z.zero in
    (* Reduce {ival} using {sentinel} *)
    let ival = if Sentinel.is_zero sntl then ival_zero else ival in
    (* Reduce {sentinel} using {ival} *)
    let sntl = match Ival_basis.Binary_Forward.beq ~size ival ival_zero with
      | Quadrivalent_Lattice.True -> Sentinel.zero
      | Quadrivalent_Lattice.False -> Sentinel.nonzero
      | _ -> sntl
    in
    sntl,ival
end

module Ival_with_sentinel = Reduced_prod_basis.Make
  (Sentinel)
  (Ival_basis)
  (Reduce)

include Ival_with_sentinel
