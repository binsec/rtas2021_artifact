module rec Common_piqi:
  sig
    type uint32 = int32
    type uint64 = int64
    type binary = string
    type smt_result =
      [
        | `unknown
        | `sat
        | `unsat
        | `timeout
      ]
    type tracing_action =
      [
        | `skip
        | `into
      ]
    type action =
      [
        | `default
        | `patch
        | `conc
        | `symb
        | `logic
        | `ignore
      ]
    type call_convention_t =
      [
        | `unknown_cvt
        | `cdecl
        | `fastcall
        | `stdcall
        | `thiscall
      ]
    type ir_kind_t =
      [
        | `dba
        | `bap
        | `miasm
      ]
    type solver_t =
      [
        | `z3
        | `boolector
        | `cvc4
        | `yices
      ]
    type analysis_direction_t =
      [
        | `forward
        | `backward
      ]
    type proto_size_t =
      [
        | `invalid_size
        | `bit8
        | `bit16
        | `bit32
        | `bit64
        | `bit80
        | `bit128
        | `bit256
      ]
    type register_value_t = Register_value_t.t
    type register_t = Register_t.t
    type memory_t = Memory_t.t
    type indirect_register_t = Indirect_register_t.t
    type memory_pol = Memory_pol.t
  end = Common_piqi
and Register_value_t:
  sig
    type t = {
      mutable typeid: Common_piqi.proto_size_t;
      mutable value_8: Common_piqi.uint32 option;
      mutable value_16: Common_piqi.uint32 option;
      mutable value_32: Common_piqi.uint32 option;
      mutable value_64: Common_piqi.uint64 option;
      mutable value_80: Common_piqi.binary option;
      mutable value_128: Common_piqi.binary option;
      mutable value_256: Common_piqi.binary option;
    }
  end = Register_value_t
and Register_t:
  sig
    type t = {
      mutable name: string;
      mutable value: Common_piqi.register_value_t;
    }
  end = Register_t
and Memory_t:
  sig
    type t = {
      mutable addr: Common_piqi.uint64;
      mutable value: Common_piqi.binary;
    }
  end = Memory_t
and Indirect_register_t:
  sig
    type t = {
      mutable name: string;
      mutable value: Common_piqi.binary;
    }
  end = Indirect_register_t
and Memory_pol:
  sig
    type t = {
      mutable addr: Common_piqi.action;
      mutable value: Common_piqi.action;
    }
  end = Memory_pol


let rec parse_uint32 x = Piqirun.int32_of_varint x
and packed_parse_uint32 x = Piqirun.int32_of_packed_varint x

and parse_uint64 x = Piqirun.int64_of_varint x
and packed_parse_uint64 x = Piqirun.int64_of_packed_varint x

and parse_binary x = Piqirun.string_of_block x

and parse_string x = Piqirun.string_of_block x

and parse_register_value_t x =
  let x = Piqirun.parse_record x in
  let _typeid, x = Piqirun.parse_required_field 1 parse_proto_size_t x in
  let _value_8, x = Piqirun.parse_optional_field 2 parse_uint32 x in
  let _value_16, x = Piqirun.parse_optional_field 3 parse_uint32 x in
  let _value_32, x = Piqirun.parse_optional_field 4 parse_uint32 x in
  let _value_64, x = Piqirun.parse_optional_field 5 parse_uint64 x in
  let _value_80, x = Piqirun.parse_optional_field 6 parse_binary x in
  let _value_128, x = Piqirun.parse_optional_field 7 parse_binary x in
  let _value_256, x = Piqirun.parse_optional_field 8 parse_binary x in
  Piqirun.check_unparsed_fields x;
  {
    Register_value_t.typeid = _typeid;
    Register_value_t.value_8 = _value_8;
    Register_value_t.value_16 = _value_16;
    Register_value_t.value_32 = _value_32;
    Register_value_t.value_64 = _value_64;
    Register_value_t.value_80 = _value_80;
    Register_value_t.value_128 = _value_128;
    Register_value_t.value_256 = _value_256;
  }

and parse_register_t x =
  let x = Piqirun.parse_record x in
  let _name, x = Piqirun.parse_required_field 1 parse_string x in
  let _value, x = Piqirun.parse_required_field 2 parse_register_value_t x in
  Piqirun.check_unparsed_fields x;
  {
    Register_t.name = _name;
    Register_t.value = _value;
  }

and parse_memory_t x =
  let x = Piqirun.parse_record x in
  let _addr, x = Piqirun.parse_required_field 1 parse_uint64 x in
  let _value, x = Piqirun.parse_required_field 2 parse_binary x in
  Piqirun.check_unparsed_fields x;
  {
    Memory_t.addr = _addr;
    Memory_t.value = _value;
  }

and parse_indirect_register_t x =
  let x = Piqirun.parse_record x in
  let _name, x = Piqirun.parse_required_field 1 parse_string x in
  let _value, x = Piqirun.parse_required_field 2 parse_binary x in
  Piqirun.check_unparsed_fields x;
  {
    Indirect_register_t.name = _name;
    Indirect_register_t.value = _value;
  }

and parse_memory_pol x =
  let x = Piqirun.parse_record x in
  let _addr, x = Piqirun.parse_required_field 1 parse_action x ~default:"\b\000" in
  let _value, x = Piqirun.parse_required_field 2 parse_action x ~default:"\b\000" in
  Piqirun.check_unparsed_fields x;
  {
    Memory_pol.addr = _addr;
    Memory_pol.value = _value;
  }

and parse_smt_result x =
  match Piqirun.int32_of_signed_varint x with
    | 0l -> `unknown
    | 1l -> `sat
    | 2l -> `unsat
    | 3l -> `timeout
    | x -> Piqirun.error_enum_const x
and packed_parse_smt_result x =
  match Piqirun.int32_of_packed_signed_varint x with
    | 0l -> `unknown
    | 1l -> `sat
    | 2l -> `unsat
    | 3l -> `timeout
    | x -> Piqirun.error_enum_const x

and parse_tracing_action x =
  match Piqirun.int32_of_signed_varint x with
    | 0l -> `skip
    | 1l -> `into
    | x -> Piqirun.error_enum_const x
and packed_parse_tracing_action x =
  match Piqirun.int32_of_packed_signed_varint x with
    | 0l -> `skip
    | 1l -> `into
    | x -> Piqirun.error_enum_const x

and parse_action x =
  match Piqirun.int32_of_signed_varint x with
    | 0l -> `default
    | 1l -> `patch
    | 2l -> `conc
    | 3l -> `symb
    | 4l -> `logic
    | 5l -> `ignore
    | x -> Piqirun.error_enum_const x
and packed_parse_action x =
  match Piqirun.int32_of_packed_signed_varint x with
    | 0l -> `default
    | 1l -> `patch
    | 2l -> `conc
    | 3l -> `symb
    | 4l -> `logic
    | 5l -> `ignore
    | x -> Piqirun.error_enum_const x

and parse_call_convention_t x =
  match Piqirun.int32_of_signed_varint x with
    | 0l -> `unknown_cvt
    | 1l -> `cdecl
    | 2l -> `fastcall
    | 3l -> `stdcall
    | 4l -> `thiscall
    | x -> Piqirun.error_enum_const x
and packed_parse_call_convention_t x =
  match Piqirun.int32_of_packed_signed_varint x with
    | 0l -> `unknown_cvt
    | 1l -> `cdecl
    | 2l -> `fastcall
    | 3l -> `stdcall
    | 4l -> `thiscall
    | x -> Piqirun.error_enum_const x

and parse_ir_kind_t x =
  match Piqirun.int32_of_signed_varint x with
    | 1l -> `dba
    | 2l -> `bap
    | 3l -> `miasm
    | x -> Piqirun.error_enum_const x
and packed_parse_ir_kind_t x =
  match Piqirun.int32_of_packed_signed_varint x with
    | 1l -> `dba
    | 2l -> `bap
    | 3l -> `miasm
    | x -> Piqirun.error_enum_const x

and parse_solver_t x =
  match Piqirun.int32_of_signed_varint x with
    | 1l -> `z3
    | 2l -> `boolector
    | 3l -> `cvc4
    | 4l -> `yices
    | x -> Piqirun.error_enum_const x
and packed_parse_solver_t x =
  match Piqirun.int32_of_packed_signed_varint x with
    | 1l -> `z3
    | 2l -> `boolector
    | 3l -> `cvc4
    | 4l -> `yices
    | x -> Piqirun.error_enum_const x

and parse_analysis_direction_t x =
  match Piqirun.int32_of_signed_varint x with
    | 1l -> `forward
    | 2l -> `backward
    | x -> Piqirun.error_enum_const x
and packed_parse_analysis_direction_t x =
  match Piqirun.int32_of_packed_signed_varint x with
    | 1l -> `forward
    | 2l -> `backward
    | x -> Piqirun.error_enum_const x

and parse_proto_size_t x =
  match Piqirun.int32_of_signed_varint x with
    | 0l -> `invalid_size
    | 1l -> `bit8
    | 2l -> `bit16
    | 3l -> `bit32
    | 4l -> `bit64
    | 5l -> `bit80
    | 6l -> `bit128
    | 7l -> `bit256
    | x -> Piqirun.error_enum_const x
and packed_parse_proto_size_t x =
  match Piqirun.int32_of_packed_signed_varint x with
    | 0l -> `invalid_size
    | 1l -> `bit8
    | 2l -> `bit16
    | 3l -> `bit32
    | 4l -> `bit64
    | 5l -> `bit80
    | 6l -> `bit128
    | 7l -> `bit256
    | x -> Piqirun.error_enum_const x


let rec gen__uint32 code x = Piqirun.int32_to_varint code x
and packed_gen__uint32 x = Piqirun.int32_to_packed_varint x

and gen__uint64 code x = Piqirun.int64_to_varint code x
and packed_gen__uint64 x = Piqirun.int64_to_packed_varint x

and gen__binary code x = Piqirun.string_to_block code x

and gen__string code x = Piqirun.string_to_block code x

and gen__register_value_t code x =
  let _typeid = Piqirun.gen_required_field 1 gen__proto_size_t x.Register_value_t.typeid in
  let _value_8 = Piqirun.gen_optional_field 2 gen__uint32 x.Register_value_t.value_8 in
  let _value_16 = Piqirun.gen_optional_field 3 gen__uint32 x.Register_value_t.value_16 in
  let _value_32 = Piqirun.gen_optional_field 4 gen__uint32 x.Register_value_t.value_32 in
  let _value_64 = Piqirun.gen_optional_field 5 gen__uint64 x.Register_value_t.value_64 in
  let _value_80 = Piqirun.gen_optional_field 6 gen__binary x.Register_value_t.value_80 in
  let _value_128 = Piqirun.gen_optional_field 7 gen__binary x.Register_value_t.value_128 in
  let _value_256 = Piqirun.gen_optional_field 8 gen__binary x.Register_value_t.value_256 in
  Piqirun.gen_record code (_typeid :: _value_8 :: _value_16 :: _value_32 :: _value_64 :: _value_80 :: _value_128 :: _value_256 :: [])

and gen__register_t code x =
  let _name = Piqirun.gen_required_field 1 gen__string x.Register_t.name in
  let _value = Piqirun.gen_required_field 2 gen__register_value_t x.Register_t.value in
  Piqirun.gen_record code (_name :: _value :: [])

and gen__memory_t code x =
  let _addr = Piqirun.gen_required_field 1 gen__uint64 x.Memory_t.addr in
  let _value = Piqirun.gen_required_field 2 gen__binary x.Memory_t.value in
  Piqirun.gen_record code (_addr :: _value :: [])

and gen__indirect_register_t code x =
  let _name = Piqirun.gen_required_field 1 gen__string x.Indirect_register_t.name in
  let _value = Piqirun.gen_required_field 2 gen__binary x.Indirect_register_t.value in
  Piqirun.gen_record code (_name :: _value :: [])

and gen__memory_pol code x =
  let _addr = Piqirun.gen_required_field 1 gen__action x.Memory_pol.addr in
  let _value = Piqirun.gen_required_field 2 gen__action x.Memory_pol.value in
  Piqirun.gen_record code (_addr :: _value :: [])

and gen__smt_result code x =
  Piqirun.int32_to_signed_varint code (match x with
    | `unknown -> 0l
    | `sat -> 1l
    | `unsat -> 2l
    | `timeout -> 3l
  )
and packed_gen__smt_result x =
  Piqirun.int32_to_packed_signed_varint (match x with
    | `unknown -> 0l
    | `sat -> 1l
    | `unsat -> 2l
    | `timeout -> 3l
  )

and gen__tracing_action code x =
  Piqirun.int32_to_signed_varint code (match x with
    | `skip -> 0l
    | `into -> 1l
  )
and packed_gen__tracing_action x =
  Piqirun.int32_to_packed_signed_varint (match x with
    | `skip -> 0l
    | `into -> 1l
  )

and gen__action code x =
  Piqirun.int32_to_signed_varint code (match x with
    | `default -> 0l
    | `patch -> 1l
    | `conc -> 2l
    | `symb -> 3l
    | `logic -> 4l
    | `ignore -> 5l
  )
and packed_gen__action x =
  Piqirun.int32_to_packed_signed_varint (match x with
    | `default -> 0l
    | `patch -> 1l
    | `conc -> 2l
    | `symb -> 3l
    | `logic -> 4l
    | `ignore -> 5l
  )

and gen__call_convention_t code x =
  Piqirun.int32_to_signed_varint code (match x with
    | `unknown_cvt -> 0l
    | `cdecl -> 1l
    | `fastcall -> 2l
    | `stdcall -> 3l
    | `thiscall -> 4l
  )
and packed_gen__call_convention_t x =
  Piqirun.int32_to_packed_signed_varint (match x with
    | `unknown_cvt -> 0l
    | `cdecl -> 1l
    | `fastcall -> 2l
    | `stdcall -> 3l
    | `thiscall -> 4l
  )

and gen__ir_kind_t code x =
  Piqirun.int32_to_signed_varint code (match x with
    | `dba -> 1l
    | `bap -> 2l
    | `miasm -> 3l
  )
and packed_gen__ir_kind_t x =
  Piqirun.int32_to_packed_signed_varint (match x with
    | `dba -> 1l
    | `bap -> 2l
    | `miasm -> 3l
  )

and gen__solver_t code x =
  Piqirun.int32_to_signed_varint code (match x with
    | `z3 -> 1l
    | `boolector -> 2l
    | `cvc4 -> 3l
    | `yices -> 4l
  )
and packed_gen__solver_t x =
  Piqirun.int32_to_packed_signed_varint (match x with
    | `z3 -> 1l
    | `boolector -> 2l
    | `cvc4 -> 3l
    | `yices -> 4l
  )

and gen__analysis_direction_t code x =
  Piqirun.int32_to_signed_varint code (match x with
    | `forward -> 1l
    | `backward -> 2l
  )
and packed_gen__analysis_direction_t x =
  Piqirun.int32_to_packed_signed_varint (match x with
    | `forward -> 1l
    | `backward -> 2l
  )

and gen__proto_size_t code x =
  Piqirun.int32_to_signed_varint code (match x with
    | `invalid_size -> 0l
    | `bit8 -> 1l
    | `bit16 -> 2l
    | `bit32 -> 3l
    | `bit64 -> 4l
    | `bit80 -> 5l
    | `bit128 -> 6l
    | `bit256 -> 7l
  )
and packed_gen__proto_size_t x =
  Piqirun.int32_to_packed_signed_varint (match x with
    | `invalid_size -> 0l
    | `bit8 -> 1l
    | `bit16 -> 2l
    | `bit32 -> 3l
    | `bit64 -> 4l
    | `bit80 -> 5l
    | `bit128 -> 6l
    | `bit256 -> 7l
  )


let gen_uint32 x = gen__uint32 (-1) x
let gen_uint64 x = gen__uint64 (-1) x
let gen_binary x = gen__binary (-1) x
let gen_string x = gen__string (-1) x
let gen_register_value_t x = gen__register_value_t (-1) x
let gen_register_t x = gen__register_t (-1) x
let gen_memory_t x = gen__memory_t (-1) x
let gen_indirect_register_t x = gen__indirect_register_t (-1) x
let gen_memory_pol x = gen__memory_pol (-1) x
let gen_smt_result x = gen__smt_result (-1) x
let gen_tracing_action x = gen__tracing_action (-1) x
let gen_action x = gen__action (-1) x
let gen_call_convention_t x = gen__call_convention_t (-1) x
let gen_ir_kind_t x = gen__ir_kind_t (-1) x
let gen_solver_t x = gen__solver_t (-1) x
let gen_analysis_direction_t x = gen__analysis_direction_t (-1) x
let gen_proto_size_t x = gen__proto_size_t (-1) x


let rec default_uint32 () = 0l
and default_uint64 () = 0L
and default_binary () = ""
and default_string () = ""
and default_register_value_t () =
  {
    Register_value_t.typeid = default_proto_size_t ();
    Register_value_t.value_8 = None;
    Register_value_t.value_16 = None;
    Register_value_t.value_32 = None;
    Register_value_t.value_64 = None;
    Register_value_t.value_80 = None;
    Register_value_t.value_128 = None;
    Register_value_t.value_256 = None;
  }
and default_register_t () =
  {
    Register_t.name = default_string ();
    Register_t.value = default_register_value_t ();
  }
and default_memory_t () =
  {
    Memory_t.addr = default_uint64 ();
    Memory_t.value = default_binary ();
  }
and default_indirect_register_t () =
  {
    Indirect_register_t.name = default_string ();
    Indirect_register_t.value = default_binary ();
  }
and default_memory_pol () =
  {
    Memory_pol.addr = parse_action (Piqirun.parse_default "\b\000");
    Memory_pol.value = parse_action (Piqirun.parse_default "\b\000");
  }
and default_smt_result () = `unknown
and default_tracing_action () = `skip
and default_action () = `default
and default_call_convention_t () = `unknown_cvt
and default_ir_kind_t () = `dba
and default_solver_t () = `z3
and default_analysis_direction_t () = `forward
and default_proto_size_t () = `invalid_size


let piqi = "\226\202\2304\006common\226\231\249\238\001\016piqi/common.piqi\162\244\146\155\011\006common\218\244\134\182\012\130\005\138\233\142\251\014\251\004\210\203\242$K\232\146\150q\002\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\006typeid\208\215\133\174\005\000\210\171\158\194\006\012proto-size-t\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$F\232\146\150q\004\152\247\223\136\002\000\152\182\154\152\004\160\223\186\243\001\232\243\204\157\004\000\218\164\238\191\004\007value-8\208\215\133\174\005\000\210\171\158\194\006\006uint32\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$G\232\146\150q\006\152\247\223\136\002\000\152\182\154\152\004\160\223\186\243\001\232\243\204\157\004\000\218\164\238\191\004\bvalue-16\208\215\133\174\005\000\210\171\158\194\006\006uint32\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$G\232\146\150q\b\152\247\223\136\002\000\152\182\154\152\004\160\223\186\243\001\232\243\204\157\004\000\218\164\238\191\004\bvalue-32\208\215\133\174\005\000\210\171\158\194\006\006uint32\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$G\232\146\150q\n\152\247\223\136\002\000\152\182\154\152\004\160\223\186\243\001\232\243\204\157\004\000\218\164\238\191\004\bvalue-64\208\215\133\174\005\000\210\171\158\194\006\006uint64\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$G\232\146\150q\012\152\247\223\136\002\000\152\182\154\152\004\160\223\186\243\001\232\243\204\157\004\000\218\164\238\191\004\bvalue-80\208\215\133\174\005\000\210\171\158\194\006\006binary\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$H\232\146\150q\014\152\247\223\136\002\000\152\182\154\152\004\160\223\186\243\001\232\243\204\157\004\000\218\164\238\191\004\tvalue-128\208\215\133\174\005\000\210\171\158\194\006\006binary\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$H\232\146\150q\016\152\247\223\136\002\000\152\182\154\152\004\160\223\186\243\001\232\243\204\157\004\000\218\164\238\191\004\tvalue-256\208\215\133\174\005\000\210\171\158\194\006\006binary\192\139\160\247\t\000\136\158\147\199\014\000\218\164\238\191\004\016register-value-t\218\244\134\182\012\178\001\138\233\142\251\014\171\001\210\203\242$C\232\146\150q\002\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\004name\208\215\133\174\005\000\210\171\158\194\006\006string\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$N\232\146\150q\004\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\005value\208\215\133\174\005\000\210\171\158\194\006\016register-value-t\192\139\160\247\t\000\136\158\147\199\014\000\218\164\238\191\004\nregister-t\218\244\134\182\012\166\001\138\233\142\251\014\159\001\210\203\242$C\232\146\150q\002\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\004addr\208\215\133\174\005\000\210\171\158\194\006\006uint64\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$D\232\146\150q\004\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\005value\208\215\133\174\005\000\210\171\158\194\006\006binary\192\139\160\247\t\000\136\158\147\199\014\000\218\164\238\191\004\bmemory-t\218\244\134\182\012\177\001\138\233\142\251\014\170\001\210\203\242$C\232\146\150q\002\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\004name\208\215\133\174\005\000\210\171\158\194\006\006string\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$D\232\146\150q\004\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\005value\208\215\133\174\005\000\210\171\158\194\006\006binary\192\139\160\247\t\000\136\158\147\199\014\000\218\164\238\191\004\019indirect-register-t\218\244\134\182\012\194\001\138\233\142\251\014\187\001\210\203\242$P\232\146\150q\002\152\247\223\136\002\000\152\182\154\152\004\160\223\186\243\001\232\243\204\157\004\000\218\164\238\191\004\004addr\208\215\133\174\005\000\210\171\158\194\006\006action\192\139\160\247\t\000\138\140\251\240\r\007\218\148\211\024\002\b\000\136\158\147\199\014\000\210\203\242$Q\232\146\150q\004\152\247\223\136\002\000\152\182\154\152\004\160\223\186\243\001\232\243\204\157\004\000\218\164\238\191\004\005value\208\215\133\174\005\000\210\171\158\194\006\006action\192\139\160\247\t\000\138\140\251\240\r\007\218\148\211\024\002\b\000\136\158\147\199\014\000\218\164\238\191\004\nmemory-pol\218\244\134\182\012\137\001\138\176\205\197\001\130\001\218\164\238\191\004\nsmt-result\170\183\218\222\005\024\232\146\150q\000\152\247\223\136\002\000\218\164\238\191\004\007UNKNOWN\170\183\218\222\005\020\232\146\150q\002\152\247\223\136\002\000\218\164\238\191\004\003SAT\170\183\218\222\005\022\232\146\150q\004\152\247\223\136\002\000\218\164\238\191\004\005UNSAT\170\183\218\222\005\024\232\146\150q\006\152\247\223\136\002\000\218\164\238\191\004\007TIMEOUT\218\244\134\182\012P\138\176\205\197\001J\218\164\238\191\004\014tracing-action\170\183\218\222\005\021\232\146\150q\000\152\247\223\136\002\000\218\164\238\191\004\004SKIP\170\183\218\222\005\021\232\146\150q\002\152\247\223\136\002\000\218\164\238\191\004\004INTO\218\244\134\182\012\188\001\138\176\205\197\001\181\001\218\164\238\191\004\006action\170\183\218\222\005\024\232\146\150q\000\152\247\223\136\002\000\218\164\238\191\004\007DEFAULT\170\183\218\222\005\022\232\146\150q\002\152\247\223\136\002\000\218\164\238\191\004\005PATCH\170\183\218\222\005\021\232\146\150q\004\152\247\223\136\002\000\218\164\238\191\004\004CONC\170\183\218\222\005\021\232\146\150q\006\152\247\223\136\002\000\218\164\238\191\004\004SYMB\170\183\218\222\005\022\232\146\150q\b\152\247\223\136\002\000\218\164\238\191\004\005LOGIC\170\183\218\222\005\023\232\146\150q\n\152\247\223\136\002\000\218\164\238\191\004\006IGNORE\218\244\134\182\012\184\001\138\176\205\197\001\177\001\218\164\238\191\004\017call-convention-t\170\183\218\222\005\028\232\146\150q\000\152\247\223\136\002\000\218\164\238\191\004\011UNKNOWN-CVT\170\183\218\222\005\022\232\146\150q\002\152\247\223\136\002\000\218\164\238\191\004\005CDECL\170\183\218\222\005\025\232\146\150q\004\152\247\223\136\002\000\218\164\238\191\004\bFASTCALL\170\183\218\222\005\024\232\146\150q\006\152\247\223\136\002\000\218\164\238\191\004\007STDCALL\170\183\218\222\005\025\232\146\150q\b\152\247\223\136\002\000\218\164\238\191\004\bTHISCALL\218\244\134\182\012e\138\176\205\197\001_\218\164\238\191\004\tir-kind-t\170\183\218\222\005\020\232\146\150q\002\152\247\223\136\002\000\218\164\238\191\004\003DBA\170\183\218\222\005\020\232\146\150q\004\152\247\223\136\002\000\218\164\238\191\004\003BAP\170\183\218\222\005\022\232\146\150q\006\152\247\223\136\002\000\218\164\238\191\004\005MIASM\218\244\134\182\012\132\001\138\176\205\197\001~\218\164\238\191\004\bsolver-t\170\183\218\222\005\019\232\146\150q\002\152\247\223\136\002\000\218\164\238\191\004\002Z3\170\183\218\222\005\026\232\146\150q\004\152\247\223\136\002\000\218\164\238\191\004\tBOOLECTOR\170\183\218\222\005\021\232\146\150q\006\152\247\223\136\002\000\218\164\238\191\004\004CVC4\170\183\218\222\005\022\232\146\150q\b\152\247\223\136\002\000\218\164\238\191\004\005YICES\218\244\134\182\012]\138\176\205\197\001W\218\164\238\191\004\020analysis-direction-t\170\183\218\222\005\024\232\146\150q\002\152\247\223\136\002\000\218\164\238\191\004\007FORWARD\170\183\218\222\005\025\232\146\150q\004\152\247\223\136\002\000\218\164\238\191\004\bBACKWARD\218\244\134\182\012\129\002\138\176\205\197\001\250\001\218\164\238\191\004\012proto-size-t\170\183\218\222\005\029\232\146\150q\000\152\247\223\136\002\000\218\164\238\191\004\012INVALID-SIZE\170\183\218\222\005\021\232\146\150q\002\152\247\223\136\002\000\218\164\238\191\004\004BIT8\170\183\218\222\005\022\232\146\150q\004\152\247\223\136\002\000\218\164\238\191\004\005BIT16\170\183\218\222\005\022\232\146\150q\006\152\247\223\136\002\000\218\164\238\191\004\005BIT32\170\183\218\222\005\022\232\146\150q\b\152\247\223\136\002\000\218\164\238\191\004\005BIT64\170\183\218\222\005\022\232\146\150q\n\152\247\223\136\002\000\218\164\238\191\004\005BIT80\170\183\218\222\005\023\232\146\150q\012\152\247\223\136\002\000\218\164\238\191\004\006BIT128\170\183\218\222\005\023\232\146\150q\014\152\247\223\136\002\000\218\164\238\191\004\006BIT256"
include Common_piqi
