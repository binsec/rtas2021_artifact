module  Common = Common_piqi

module rec Syscall_piqi:
  sig
    type uint32 = int32
    type uint64 = int64
    type protobuf_int32 = int32
    type binary = string
    type syscall_ident =
      [
        | `nr_read
        | `nr_write
        | `nr_open
        | `generic
      ]
    type syscall_pol = Syscall_pol.t
    type syscall_t = Syscall_t.t
    type nr_read_pol = Nr_read_pol.t
    type nr_open_t = Nr_open_t.t
    type nr_read_t = Nr_read_t.t
    type nr_write_t = Nr_write_t.t
    type sys_generic_t = Sys_generic_t.t
  end = Syscall_piqi
and Syscall_pol:
  sig
    type t = {
      mutable id: Syscall_piqi.uint32;
      mutable name: string;
      mutable ident: Syscall_piqi.syscall_ident;
      mutable action: Common.tracing_action;
      mutable restrict_addresses: Syscall_piqi.uint64 list;
      mutable nr_read: Syscall_piqi.nr_read_pol option;
    }
  end = Syscall_pol
and Syscall_t:
  sig
    type t = {
      mutable id: Syscall_piqi.uint32;
      mutable ident: Syscall_piqi.syscall_ident;
      mutable open_syscall: Syscall_piqi.nr_open_t option;
      mutable read_syscall: Syscall_piqi.nr_read_t option;
      mutable write_sycall: Syscall_piqi.nr_write_t option;
      mutable generic_sycall: Syscall_piqi.sys_generic_t option;
    }
  end = Syscall_t
and Nr_read_pol:
  sig
    type t = {
      mutable name: string;
    }
  end = Nr_read_pol
and Nr_open_t:
  sig
    type t = {
      mutable file_name: string;
      mutable flags: int32;
      mutable mode: Syscall_piqi.uint32;
      mutable file_descriptor: Syscall_piqi.protobuf_int32;
    }
  end = Nr_open_t
and Nr_read_t:
  sig
    type t = {
      mutable file_descriptor: Syscall_piqi.uint32;
      mutable buffer_address: Syscall_piqi.uint64;
      mutable count: Syscall_piqi.uint32;
      mutable buffer_data: Syscall_piqi.binary;
      mutable count_effective: Syscall_piqi.uint32;
    }
  end = Nr_read_t
and Nr_write_t:
  sig
    type t = {
      mutable file_descriptor: Syscall_piqi.uint32;
      mutable buffer_address: Syscall_piqi.uint64;
      mutable count: Syscall_piqi.uint32;
      mutable buffer_data: Syscall_piqi.binary;
      mutable count_effective: Syscall_piqi.uint32;
    }
  end = Nr_write_t
and Sys_generic_t:
  sig
    type t = {
      mutable name: string;
      mutable address: Syscall_piqi.uint32;
    }
  end = Sys_generic_t


let rec parse_int32 x = Piqirun.int32_of_zigzag_varint x
and packed_parse_int32 x = Piqirun.int32_of_packed_zigzag_varint x

and parse_uint32 x = Piqirun.int32_of_varint x
and packed_parse_uint32 x = Piqirun.int32_of_packed_varint x

and parse_string x = Piqirun.string_of_block x

and parse_uint64 x = Piqirun.int64_of_varint x
and packed_parse_uint64 x = Piqirun.int64_of_packed_varint x

and parse_protobuf_int32 x = Piqirun.int32_of_signed_varint x
and packed_parse_protobuf_int32 x = Piqirun.int32_of_packed_signed_varint x

and parse_binary x = Piqirun.string_of_block x

and parse_syscall_pol x =
  let x = Piqirun.parse_record x in
  let _id, x = Piqirun.parse_required_field 1 parse_uint32 x in
  let _name, x = Piqirun.parse_required_field 2 parse_string x in
  let _ident, x = Piqirun.parse_required_field 3 parse_syscall_ident x in
  let _action, x = Piqirun.parse_required_field 4 Common.parse_tracing_action x in
  let _restrict_addresses, x = Piqirun.parse_repeated_field 5 parse_uint64 x in
  let _nr_read, x = Piqirun.parse_optional_field 6 parse_nr_read_pol x in
  Piqirun.check_unparsed_fields x;
  {
    Syscall_pol.id = _id;
    Syscall_pol.name = _name;
    Syscall_pol.ident = _ident;
    Syscall_pol.action = _action;
    Syscall_pol.restrict_addresses = _restrict_addresses;
    Syscall_pol.nr_read = _nr_read;
  }

and parse_syscall_t x =
  let x = Piqirun.parse_record x in
  let _id, x = Piqirun.parse_required_field 1 parse_uint32 x in
  let _ident, x = Piqirun.parse_required_field 2 parse_syscall_ident x in
  let _open_syscall, x = Piqirun.parse_optional_field 3 parse_nr_open_t x in
  let _read_syscall, x = Piqirun.parse_optional_field 4 parse_nr_read_t x in
  let _write_sycall, x = Piqirun.parse_optional_field 5 parse_nr_write_t x in
  let _generic_sycall, x = Piqirun.parse_optional_field 6 parse_sys_generic_t x in
  Piqirun.check_unparsed_fields x;
  {
    Syscall_t.id = _id;
    Syscall_t.ident = _ident;
    Syscall_t.open_syscall = _open_syscall;
    Syscall_t.read_syscall = _read_syscall;
    Syscall_t.write_sycall = _write_sycall;
    Syscall_t.generic_sycall = _generic_sycall;
  }

and parse_nr_read_pol x =
  let x = Piqirun.parse_record x in
  let _name, x = Piqirun.parse_required_field 1 parse_string x in
  Piqirun.check_unparsed_fields x;
  {
    Nr_read_pol.name = _name;
  }

and parse_nr_open_t x =
  let x = Piqirun.parse_record x in
  let _file_name, x = Piqirun.parse_required_field 1 parse_string x in
  let _flags, x = Piqirun.parse_required_field 2 parse_int32 x in
  let _mode, x = Piqirun.parse_required_field 3 parse_uint32 x in
  let _file_descriptor, x = Piqirun.parse_required_field 4 parse_protobuf_int32 x in
  Piqirun.check_unparsed_fields x;
  {
    Nr_open_t.file_name = _file_name;
    Nr_open_t.flags = _flags;
    Nr_open_t.mode = _mode;
    Nr_open_t.file_descriptor = _file_descriptor;
  }

and parse_nr_read_t x =
  let x = Piqirun.parse_record x in
  let _file_descriptor, x = Piqirun.parse_required_field 1 parse_uint32 x in
  let _buffer_address, x = Piqirun.parse_required_field 2 parse_uint64 x in
  let _count, x = Piqirun.parse_required_field 3 parse_uint32 x in
  let _buffer_data, x = Piqirun.parse_required_field 4 parse_binary x in
  let _count_effective, x = Piqirun.parse_required_field 5 parse_uint32 x in
  Piqirun.check_unparsed_fields x;
  {
    Nr_read_t.file_descriptor = _file_descriptor;
    Nr_read_t.buffer_address = _buffer_address;
    Nr_read_t.count = _count;
    Nr_read_t.buffer_data = _buffer_data;
    Nr_read_t.count_effective = _count_effective;
  }

and parse_nr_write_t x =
  let x = Piqirun.parse_record x in
  let _file_descriptor, x = Piqirun.parse_required_field 1 parse_uint32 x in
  let _buffer_address, x = Piqirun.parse_required_field 2 parse_uint64 x in
  let _count, x = Piqirun.parse_required_field 3 parse_uint32 x in
  let _buffer_data, x = Piqirun.parse_required_field 4 parse_binary x in
  let _count_effective, x = Piqirun.parse_required_field 5 parse_uint32 x in
  Piqirun.check_unparsed_fields x;
  {
    Nr_write_t.file_descriptor = _file_descriptor;
    Nr_write_t.buffer_address = _buffer_address;
    Nr_write_t.count = _count;
    Nr_write_t.buffer_data = _buffer_data;
    Nr_write_t.count_effective = _count_effective;
  }

and parse_sys_generic_t x =
  let x = Piqirun.parse_record x in
  let _name, x = Piqirun.parse_required_field 1 parse_string x in
  let _address, x = Piqirun.parse_required_field 2 parse_uint32 x in
  Piqirun.check_unparsed_fields x;
  {
    Sys_generic_t.name = _name;
    Sys_generic_t.address = _address;
  }

and parse_syscall_ident x =
  match Piqirun.int32_of_signed_varint x with
    | 0l -> `nr_read
    | 1l -> `nr_write
    | 2l -> `nr_open
    | 3l -> `generic
    | x -> Piqirun.error_enum_const x
and packed_parse_syscall_ident x =
  match Piqirun.int32_of_packed_signed_varint x with
    | 0l -> `nr_read
    | 1l -> `nr_write
    | 2l -> `nr_open
    | 3l -> `generic
    | x -> Piqirun.error_enum_const x


let rec gen__int32 code x = Piqirun.int32_to_zigzag_varint code x
and packed_gen__int32 x = Piqirun.int32_to_packed_zigzag_varint x

and gen__uint32 code x = Piqirun.int32_to_varint code x
and packed_gen__uint32 x = Piqirun.int32_to_packed_varint x

and gen__string code x = Piqirun.string_to_block code x

and gen__uint64 code x = Piqirun.int64_to_varint code x
and packed_gen__uint64 x = Piqirun.int64_to_packed_varint x

and gen__protobuf_int32 code x = Piqirun.int32_to_signed_varint code x
and packed_gen__protobuf_int32 x = Piqirun.int32_to_packed_signed_varint x

and gen__binary code x = Piqirun.string_to_block code x

and gen__syscall_pol code x =
  let _id = Piqirun.gen_required_field 1 gen__uint32 x.Syscall_pol.id in
  let _name = Piqirun.gen_required_field 2 gen__string x.Syscall_pol.name in
  let _ident = Piqirun.gen_required_field 3 gen__syscall_ident x.Syscall_pol.ident in
  let _action = Piqirun.gen_required_field 4 Common.gen__tracing_action x.Syscall_pol.action in
  let _restrict_addresses = Piqirun.gen_repeated_field 5 gen__uint64 x.Syscall_pol.restrict_addresses in
  let _nr_read = Piqirun.gen_optional_field 6 gen__nr_read_pol x.Syscall_pol.nr_read in
  Piqirun.gen_record code (_id :: _name :: _ident :: _action :: _restrict_addresses :: _nr_read :: [])

and gen__syscall_t code x =
  let _id = Piqirun.gen_required_field 1 gen__uint32 x.Syscall_t.id in
  let _ident = Piqirun.gen_required_field 2 gen__syscall_ident x.Syscall_t.ident in
  let _open_syscall = Piqirun.gen_optional_field 3 gen__nr_open_t x.Syscall_t.open_syscall in
  let _read_syscall = Piqirun.gen_optional_field 4 gen__nr_read_t x.Syscall_t.read_syscall in
  let _write_sycall = Piqirun.gen_optional_field 5 gen__nr_write_t x.Syscall_t.write_sycall in
  let _generic_sycall = Piqirun.gen_optional_field 6 gen__sys_generic_t x.Syscall_t.generic_sycall in
  Piqirun.gen_record code (_id :: _ident :: _open_syscall :: _read_syscall :: _write_sycall :: _generic_sycall :: [])

and gen__nr_read_pol code x =
  let _name = Piqirun.gen_required_field 1 gen__string x.Nr_read_pol.name in
  Piqirun.gen_record code (_name :: [])

and gen__nr_open_t code x =
  let _file_name = Piqirun.gen_required_field 1 gen__string x.Nr_open_t.file_name in
  let _flags = Piqirun.gen_required_field 2 gen__int32 x.Nr_open_t.flags in
  let _mode = Piqirun.gen_required_field 3 gen__uint32 x.Nr_open_t.mode in
  let _file_descriptor = Piqirun.gen_required_field 4 gen__protobuf_int32 x.Nr_open_t.file_descriptor in
  Piqirun.gen_record code (_file_name :: _flags :: _mode :: _file_descriptor :: [])

and gen__nr_read_t code x =
  let _file_descriptor = Piqirun.gen_required_field 1 gen__uint32 x.Nr_read_t.file_descriptor in
  let _buffer_address = Piqirun.gen_required_field 2 gen__uint64 x.Nr_read_t.buffer_address in
  let _count = Piqirun.gen_required_field 3 gen__uint32 x.Nr_read_t.count in
  let _buffer_data = Piqirun.gen_required_field 4 gen__binary x.Nr_read_t.buffer_data in
  let _count_effective = Piqirun.gen_required_field 5 gen__uint32 x.Nr_read_t.count_effective in
  Piqirun.gen_record code (_file_descriptor :: _buffer_address :: _count :: _buffer_data :: _count_effective :: [])

and gen__nr_write_t code x =
  let _file_descriptor = Piqirun.gen_required_field 1 gen__uint32 x.Nr_write_t.file_descriptor in
  let _buffer_address = Piqirun.gen_required_field 2 gen__uint64 x.Nr_write_t.buffer_address in
  let _count = Piqirun.gen_required_field 3 gen__uint32 x.Nr_write_t.count in
  let _buffer_data = Piqirun.gen_required_field 4 gen__binary x.Nr_write_t.buffer_data in
  let _count_effective = Piqirun.gen_required_field 5 gen__uint32 x.Nr_write_t.count_effective in
  Piqirun.gen_record code (_file_descriptor :: _buffer_address :: _count :: _buffer_data :: _count_effective :: [])

and gen__sys_generic_t code x =
  let _name = Piqirun.gen_required_field 1 gen__string x.Sys_generic_t.name in
  let _address = Piqirun.gen_required_field 2 gen__uint32 x.Sys_generic_t.address in
  Piqirun.gen_record code (_name :: _address :: [])

and gen__syscall_ident code x =
  Piqirun.int32_to_signed_varint code (match x with
    | `nr_read -> 0l
    | `nr_write -> 1l
    | `nr_open -> 2l
    | `generic -> 3l
  )
and packed_gen__syscall_ident x =
  Piqirun.int32_to_packed_signed_varint (match x with
    | `nr_read -> 0l
    | `nr_write -> 1l
    | `nr_open -> 2l
    | `generic -> 3l
  )


let gen_int32 x = gen__int32 (-1) x
let gen_uint32 x = gen__uint32 (-1) x
let gen_string x = gen__string (-1) x
let gen_uint64 x = gen__uint64 (-1) x
let gen_protobuf_int32 x = gen__protobuf_int32 (-1) x
let gen_binary x = gen__binary (-1) x
let gen_syscall_pol x = gen__syscall_pol (-1) x
let gen_syscall_t x = gen__syscall_t (-1) x
let gen_nr_read_pol x = gen__nr_read_pol (-1) x
let gen_nr_open_t x = gen__nr_open_t (-1) x
let gen_nr_read_t x = gen__nr_read_t (-1) x
let gen_nr_write_t x = gen__nr_write_t (-1) x
let gen_sys_generic_t x = gen__sys_generic_t (-1) x
let gen_syscall_ident x = gen__syscall_ident (-1) x


let rec default_int32 () = 0l
and default_uint32 () = 0l
and default_string () = ""
and default_uint64 () = 0L
and default_protobuf_int32 () = default_int32 ()
and default_binary () = ""
and default_syscall_pol () =
  {
    Syscall_pol.id = default_uint32 ();
    Syscall_pol.name = default_string ();
    Syscall_pol.ident = default_syscall_ident ();
    Syscall_pol.action = Common.default_tracing_action ();
    Syscall_pol.restrict_addresses = [];
    Syscall_pol.nr_read = None;
  }
and default_syscall_t () =
  {
    Syscall_t.id = default_uint32 ();
    Syscall_t.ident = default_syscall_ident ();
    Syscall_t.open_syscall = None;
    Syscall_t.read_syscall = None;
    Syscall_t.write_sycall = None;
    Syscall_t.generic_sycall = None;
  }
and default_nr_read_pol () =
  {
    Nr_read_pol.name = default_string ();
  }
and default_nr_open_t () =
  {
    Nr_open_t.file_name = default_string ();
    Nr_open_t.flags = default_int32 ();
    Nr_open_t.mode = default_uint32 ();
    Nr_open_t.file_descriptor = default_protobuf_int32 ();
  }
and default_nr_read_t () =
  {
    Nr_read_t.file_descriptor = default_uint32 ();
    Nr_read_t.buffer_address = default_uint64 ();
    Nr_read_t.count = default_uint32 ();
    Nr_read_t.buffer_data = default_binary ();
    Nr_read_t.count_effective = default_uint32 ();
  }
and default_nr_write_t () =
  {
    Nr_write_t.file_descriptor = default_uint32 ();
    Nr_write_t.buffer_address = default_uint64 ();
    Nr_write_t.count = default_uint32 ();
    Nr_write_t.buffer_data = default_binary ();
    Nr_write_t.count_effective = default_uint32 ();
  }
and default_sys_generic_t () =
  {
    Sys_generic_t.name = default_string ();
    Sys_generic_t.address = default_uint32 ();
  }
and default_syscall_ident () = `nr_read


let piqi = "\226\202\2304\007syscall\226\231\249\238\001\017piqi/syscall.piqi\170\150\212\160\004\011\226\202\2304\006common\162\244\146\155\011\rsyscall_types\218\244\134\182\012\245\003\138\233\142\251\014\238\003\210\203\242$A\232\146\150q\002\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\002id\208\215\133\174\005\000\210\171\158\194\006\006uint32\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$C\232\146\150q\004\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\004name\208\215\133\174\005\000\210\171\158\194\006\006string\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$K\232\146\150q\006\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\005ident\208\215\133\174\005\000\210\171\158\194\006\rsyscall-ident\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$T\232\146\150q\b\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\006action\208\215\133\174\005\000\210\171\158\194\006\021common/tracing-action\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$Q\232\146\150q\n\152\247\223\136\002\000\152\182\154\152\004\250\248\214\130\001\232\243\204\157\004\000\218\164\238\191\004\018restrict-addresses\208\215\133\174\005\000\210\171\158\194\006\006uint64\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$K\232\146\150q\012\152\247\223\136\002\000\152\182\154\152\004\160\223\186\243\001\232\243\204\157\004\000\218\164\238\191\004\007nr-read\208\215\133\174\005\000\210\171\158\194\006\011nr-read-pol\192\139\160\247\t\000\136\158\147\199\014\000\218\164\238\191\004\011syscall-pol\218\244\134\182\012\255\003\138\233\142\251\014\248\003\210\203\242$A\232\146\150q\002\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\002id\208\215\133\174\005\000\210\171\158\194\006\006uint32\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$K\232\146\150q\004\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\005ident\208\215\133\174\005\000\210\171\158\194\006\rsyscall-ident\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$N\232\146\150q\006\152\247\223\136\002\000\152\182\154\152\004\160\223\186\243\001\232\243\204\157\004\000\218\164\238\191\004\012open-syscall\208\215\133\174\005\000\210\171\158\194\006\tnr-open-t\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$N\232\146\150q\b\152\247\223\136\002\000\152\182\154\152\004\160\223\186\243\001\232\243\204\157\004\000\218\164\238\191\004\012read-syscall\208\215\133\174\005\000\210\171\158\194\006\tnr-read-t\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$O\232\146\150q\n\152\247\223\136\002\000\152\182\154\152\004\160\223\186\243\001\232\243\204\157\004\000\218\164\238\191\004\012write-sycall\208\215\133\174\005\000\210\171\158\194\006\nnr-write-t\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$T\232\146\150q\012\152\247\223\136\002\000\152\182\154\152\004\160\223\186\243\001\232\243\204\157\004\000\218\164\238\191\004\014generic-sycall\208\215\133\174\005\000\210\171\158\194\006\rsys-generic-t\192\139\160\247\t\000\136\158\147\199\014\000\218\164\238\191\004\tsyscall-t\218\244\134\182\012_\138\233\142\251\014Y\210\203\242$C\232\146\150q\002\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\004name\208\215\133\174\005\000\210\171\158\194\006\006string\192\139\160\247\t\000\136\158\147\199\014\000\218\164\238\191\004\011nr-read-pol\218\244\134\182\012\206\002\138\233\142\251\014\199\002\210\203\242$H\232\146\150q\002\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\tfile-name\208\215\133\174\005\000\210\171\158\194\006\006string\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$C\232\146\150q\004\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\005flags\208\215\133\174\005\000\210\171\158\194\006\005int32\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$C\232\146\150q\006\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\004mode\208\215\133\174\005\000\210\171\158\194\006\006uint32\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$V\232\146\150q\b\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\015file-descriptor\208\215\133\174\005\000\210\171\158\194\006\014protobuf-int32\192\139\160\247\t\000\136\158\147\199\014\000\218\164\238\191\004\tnr-open-t\218\244\134\182\012\166\003\138\233\142\251\014\159\003\210\203\242$N\232\146\150q\002\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\015file-descriptor\208\215\133\174\005\000\210\171\158\194\006\006uint32\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$M\232\146\150q\004\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\014buffer-address\208\215\133\174\005\000\210\171\158\194\006\006uint64\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$D\232\146\150q\006\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\005count\208\215\133\174\005\000\210\171\158\194\006\006uint32\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$J\232\146\150q\b\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\011buffer-data\208\215\133\174\005\000\210\171\158\194\006\006binary\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$N\232\146\150q\n\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\015count-effective\208\215\133\174\005\000\210\171\158\194\006\006uint32\192\139\160\247\t\000\136\158\147\199\014\000\218\164\238\191\004\tnr-read-t\218\244\134\182\012\167\003\138\233\142\251\014\160\003\210\203\242$N\232\146\150q\002\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\015file-descriptor\208\215\133\174\005\000\210\171\158\194\006\006uint32\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$M\232\146\150q\004\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\014buffer-address\208\215\133\174\005\000\210\171\158\194\006\006uint64\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$D\232\146\150q\006\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\005count\208\215\133\174\005\000\210\171\158\194\006\006uint32\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$J\232\146\150q\b\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\011buffer-data\208\215\133\174\005\000\210\171\158\194\006\006binary\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$N\232\146\150q\n\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\015count-effective\208\215\133\174\005\000\210\171\158\194\006\006uint32\192\139\160\247\t\000\136\158\147\199\014\000\218\164\238\191\004\nnr-write-t\218\244\134\182\012\173\001\138\233\142\251\014\166\001\210\203\242$C\232\146\150q\002\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\004name\208\215\133\174\005\000\210\171\158\194\006\006string\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$F\232\146\150q\004\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\007address\208\215\133\174\005\000\210\171\158\194\006\006uint32\192\139\160\247\t\000\136\158\147\199\014\000\218\164\238\191\004\rsys-generic-t\218\244\134\182\012\147\001\138\176\205\197\001\140\001\218\164\238\191\004\rsyscall-ident\170\183\218\222\005\024\232\146\150q\000\152\247\223\136\002\000\218\164\238\191\004\007NR-READ\170\183\218\222\005\025\232\146\150q\002\152\247\223\136\002\000\218\164\238\191\004\bNR-WRITE\170\183\218\222\005\024\232\146\150q\004\152\247\223\136\002\000\218\164\238\191\004\007NR-OPEN\170\183\218\222\005\024\232\146\150q\006\152\247\223\136\002\000\218\164\238\191\004\007GENERIC"
include Syscall_piqi
