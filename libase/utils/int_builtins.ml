(* Note that we directly pass the tagged value here. *)
external log2: int -> (int[@untagged]) =
  "caml_int_builtin_log2_byte" "caml_int_builtin_log2" [@@noalloc]

external log2_untagged: (int [@untagged]) -> (int[@untagged]) =
  "caml_int_builtin_log2_byte" "caml_int_builtin_log2_untagged" [@@noalloc]

external highest_bit: int -> (int[@untagged]) =
  "caml_int_builtin_highest_bit_byte" "caml_int_builtin_highest_bit" [@@noalloc]

external highest_bit_untagged: (int [@untagged]) -> (int[@untagged]) =
  "caml_int_builtin_highest_bit_byte" "caml_int_builtin_highest_bit_untagged" [@@noalloc]


