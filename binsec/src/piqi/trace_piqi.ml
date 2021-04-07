module  Dba = Dba_piqi

module  Common = Common_piqi

module  Syscall = Syscall_piqi

module  Libcall = Libcall_piqi

module rec Trace_piqi:
  sig
    type uint32 = int32
    type uint64 = int64
    type binary = string
    type header_t_architecture_t =
      [
        | `x86
        | `x86_64
      ]
    type metadata_t_typeid_metadata_t =
      [
        | `invalid_metadata
        | `exception_type
        | `module_type
        | `wave_type
      ]
    type ins_con_info_t_typeid_con_info_t =
      [
        | `invalid
        | `regread
        | `regwrite
        | `memload
        | `memstore
        | `call
        | `syscall
        | `not_retrieved
        | `next_address
        | `comment
        | `wave
      ]
    type body_t_typeid_body_t =
      [
        | `metadata
        | `instruction
      ]
    type header_t = Header_t.t
    type metadata_t = Metadata_t.t
    type metadata_t_exception_t = Metadata_t_exception_t.t
    type ins_con_info_t = Ins_con_info_t.t
    type instruction_t = Instruction_t.t
    type body_t = Body_t.t
    type chunk_t = Chunk_t.t
    type trace_t = Trace_t.t
  end = Trace_piqi
and Header_t:
  sig
    type t = {
      mutable architecture: Trace_piqi.header_t_architecture_t;
      mutable address_size: Common.proto_size_t;
      mutable kind: Common.ir_kind_t;
    }
  end = Header_t
and Metadata_t:
  sig
    type t = {
      mutable typeid: Trace_piqi.metadata_t_typeid_metadata_t;
      mutable module_metadata: string option;
      mutable exception_metadata: Trace_piqi.metadata_t_exception_t option;
      mutable wave_metadata: Trace_piqi.uint32 option;
    }
  end = Metadata_t
and Metadata_t_exception_t:
  sig
    type t = {
      mutable type_exception: Trace_piqi.uint32;
      mutable handler: Trace_piqi.uint64;
    }
  end = Metadata_t_exception_t
and Ins_con_info_t:
  sig
    type t = {
      mutable typeid: Trace_piqi.ins_con_info_t_typeid_con_info_t;
      mutable read_register: Common.register_t option;
      mutable write_register: Common.register_t option;
      mutable load_memory: Common.memory_t option;
      mutable store_memory: Common.memory_t option;
      mutable call: Libcall.libcall_t option;
      mutable system_call: Syscall.syscall_t option;
      mutable next_address: Trace_piqi.uint64 option;
      mutable reserved_comment: string option;
      mutable wave: Trace_piqi.uint32 option;
    }
  end = Ins_con_info_t
and Instruction_t:
  sig
    type t = {
      mutable thread_id: Trace_piqi.uint32;
      mutable address: Trace_piqi.uint64;
      mutable opcode: Trace_piqi.binary;
      mutable concrete_infos: Trace_piqi.ins_con_info_t list;
      mutable dba_instrs: Dba.dba_list option;
    }
  end = Instruction_t
and Body_t:
  sig
    type t = {
      mutable typeid: Trace_piqi.body_t_typeid_body_t;
      mutable metadata: Trace_piqi.metadata_t option;
      mutable instruction: Trace_piqi.instruction_t option;
    }
  end = Body_t
and Chunk_t:
  sig
    type t = {
      mutable body: Trace_piqi.body_t list;
    }
  end = Chunk_t
and Trace_t:
  sig
    type t = {
      mutable header: Trace_piqi.header_t;
      mutable body: Trace_piqi.body_t list;
    }
  end = Trace_t


let rec parse_string x = Piqirun.string_of_block x

and parse_uint32 x = Piqirun.int32_of_varint x
and packed_parse_uint32 x = Piqirun.int32_of_packed_varint x

and parse_uint64 x = Piqirun.int64_of_varint x
and packed_parse_uint64 x = Piqirun.int64_of_packed_varint x

and parse_binary x = Piqirun.string_of_block x

and parse_header_t x =
  let x = Piqirun.parse_record x in
  let _architecture, x = Piqirun.parse_required_field 1 parse_header_t_architecture_t x in
  let _address_size, x = Piqirun.parse_required_field 2 Common.parse_proto_size_t x in
  let _kind, x = Piqirun.parse_required_field 3 Common.parse_ir_kind_t x ~default:"\b\001" in
  Piqirun.check_unparsed_fields x;
  {
    Header_t.architecture = _architecture;
    Header_t.address_size = _address_size;
    Header_t.kind = _kind;
  }

and parse_header_t_architecture_t x =
  match Piqirun.int32_of_signed_varint x with
    | 0l -> `x86
    | 1l -> `x86_64
    | x -> Piqirun.error_enum_const x
and packed_parse_header_t_architecture_t x =
  match Piqirun.int32_of_packed_signed_varint x with
    | 0l -> `x86
    | 1l -> `x86_64
    | x -> Piqirun.error_enum_const x

and parse_metadata_t x =
  let x = Piqirun.parse_record x in
  let _typeid, x = Piqirun.parse_required_field 1 parse_metadata_t_typeid_metadata_t x in
  let _module_metadata, x = Piqirun.parse_optional_field 2 parse_string x in
  let _exception_metadata, x = Piqirun.parse_optional_field 3 parse_metadata_t_exception_t x in
  let _wave_metadata, x = Piqirun.parse_optional_field 4 parse_uint32 x in
  Piqirun.check_unparsed_fields x;
  {
    Metadata_t.typeid = _typeid;
    Metadata_t.module_metadata = _module_metadata;
    Metadata_t.exception_metadata = _exception_metadata;
    Metadata_t.wave_metadata = _wave_metadata;
  }

and parse_metadata_t_exception_t x =
  let x = Piqirun.parse_record x in
  let _type_exception, x = Piqirun.parse_required_field 1 parse_uint32 x in
  let _handler, x = Piqirun.parse_required_field 2 parse_uint64 x in
  Piqirun.check_unparsed_fields x;
  {
    Metadata_t_exception_t.type_exception = _type_exception;
    Metadata_t_exception_t.handler = _handler;
  }

and parse_metadata_t_typeid_metadata_t x =
  match Piqirun.int32_of_signed_varint x with
    | 0l -> `invalid_metadata
    | 1l -> `exception_type
    | 2l -> `module_type
    | 3l -> `wave_type
    | x -> Piqirun.error_enum_const x
and packed_parse_metadata_t_typeid_metadata_t x =
  match Piqirun.int32_of_packed_signed_varint x with
    | 0l -> `invalid_metadata
    | 1l -> `exception_type
    | 2l -> `module_type
    | 3l -> `wave_type
    | x -> Piqirun.error_enum_const x

and parse_ins_con_info_t x =
  let x = Piqirun.parse_record x in
  let _typeid, x = Piqirun.parse_required_field 1 parse_ins_con_info_t_typeid_con_info_t x in
  let _read_register, x = Piqirun.parse_optional_field 2 Common.parse_register_t x in
  let _write_register, x = Piqirun.parse_optional_field 3 Common.parse_register_t x in
  let _load_memory, x = Piqirun.parse_optional_field 4 Common.parse_memory_t x in
  let _store_memory, x = Piqirun.parse_optional_field 5 Common.parse_memory_t x in
  let _call, x = Piqirun.parse_optional_field 6 Libcall.parse_libcall_t x in
  let _system_call, x = Piqirun.parse_optional_field 8 Syscall.parse_syscall_t x in
  let _next_address, x = Piqirun.parse_optional_field 9 parse_uint64 x in
  let _reserved_comment, x = Piqirun.parse_optional_field 10 parse_string x in
  let _wave, x = Piqirun.parse_optional_field 11 parse_uint32 x in
  Piqirun.check_unparsed_fields x;
  {
    Ins_con_info_t.typeid = _typeid;
    Ins_con_info_t.read_register = _read_register;
    Ins_con_info_t.write_register = _write_register;
    Ins_con_info_t.load_memory = _load_memory;
    Ins_con_info_t.store_memory = _store_memory;
    Ins_con_info_t.call = _call;
    Ins_con_info_t.system_call = _system_call;
    Ins_con_info_t.next_address = _next_address;
    Ins_con_info_t.reserved_comment = _reserved_comment;
    Ins_con_info_t.wave = _wave;
  }

and parse_ins_con_info_t_typeid_con_info_t x =
  match Piqirun.int32_of_signed_varint x with
    | 0l -> `invalid
    | 1l -> `regread
    | 2l -> `regwrite
    | 3l -> `memload
    | 4l -> `memstore
    | 5l -> `call
    | 6l -> `syscall
    | 7l -> `not_retrieved
    | 8l -> `next_address
    | 9l -> `comment
    | 10l -> `wave
    | x -> Piqirun.error_enum_const x
and packed_parse_ins_con_info_t_typeid_con_info_t x =
  match Piqirun.int32_of_packed_signed_varint x with
    | 0l -> `invalid
    | 1l -> `regread
    | 2l -> `regwrite
    | 3l -> `memload
    | 4l -> `memstore
    | 5l -> `call
    | 6l -> `syscall
    | 7l -> `not_retrieved
    | 8l -> `next_address
    | 9l -> `comment
    | 10l -> `wave
    | x -> Piqirun.error_enum_const x

and parse_instruction_t x =
  let x = Piqirun.parse_record x in
  let _thread_id, x = Piqirun.parse_required_field 1 parse_uint32 x in
  let _address, x = Piqirun.parse_required_field 2 parse_uint64 x in
  let _opcode, x = Piqirun.parse_required_field 3 parse_binary x in
  let _concrete_infos, x = Piqirun.parse_repeated_field 4 parse_ins_con_info_t x in
  let _dba_instrs, x = Piqirun.parse_optional_field 5 Dba.parse_dba_list x in
  Piqirun.check_unparsed_fields x;
  {
    Instruction_t.thread_id = _thread_id;
    Instruction_t.address = _address;
    Instruction_t.opcode = _opcode;
    Instruction_t.concrete_infos = _concrete_infos;
    Instruction_t.dba_instrs = _dba_instrs;
  }

and parse_body_t x =
  let x = Piqirun.parse_record x in
  let _typeid, x = Piqirun.parse_required_field 1 parse_body_t_typeid_body_t x in
  let _metadata, x = Piqirun.parse_optional_field 2 parse_metadata_t x in
  let _instruction, x = Piqirun.parse_optional_field 3 parse_instruction_t x in
  Piqirun.check_unparsed_fields x;
  {
    Body_t.typeid = _typeid;
    Body_t.metadata = _metadata;
    Body_t.instruction = _instruction;
  }

and parse_body_t_typeid_body_t x =
  match Piqirun.int32_of_signed_varint x with
    | 0l -> `metadata
    | 1l -> `instruction
    | x -> Piqirun.error_enum_const x
and packed_parse_body_t_typeid_body_t x =
  match Piqirun.int32_of_packed_signed_varint x with
    | 0l -> `metadata
    | 1l -> `instruction
    | x -> Piqirun.error_enum_const x

and parse_chunk_t x =
  let x = Piqirun.parse_record x in
  let _body, x = Piqirun.parse_repeated_field 1 parse_body_t x in
  Piqirun.check_unparsed_fields x;
  {
    Chunk_t.body = _body;
  }

and parse_trace_t x =
  let x = Piqirun.parse_record x in
  let _header, x = Piqirun.parse_required_field 1 parse_header_t x in
  let _body, x = Piqirun.parse_repeated_field 2 parse_body_t x in
  Piqirun.check_unparsed_fields x;
  {
    Trace_t.header = _header;
    Trace_t.body = _body;
  }


let rec gen__string code x = Piqirun.string_to_block code x

and gen__uint32 code x = Piqirun.int32_to_varint code x
and packed_gen__uint32 x = Piqirun.int32_to_packed_varint x

and gen__uint64 code x = Piqirun.int64_to_varint code x
and packed_gen__uint64 x = Piqirun.int64_to_packed_varint x

and gen__binary code x = Piqirun.string_to_block code x

and gen__header_t code x =
  let _architecture = Piqirun.gen_required_field 1 gen__header_t_architecture_t x.Header_t.architecture in
  let _address_size = Piqirun.gen_required_field 2 Common.gen__proto_size_t x.Header_t.address_size in
  let _kind = Piqirun.gen_required_field 3 Common.gen__ir_kind_t x.Header_t.kind in
  Piqirun.gen_record code (_architecture :: _address_size :: _kind :: [])

and gen__header_t_architecture_t code x =
  Piqirun.int32_to_signed_varint code (match x with
    | `x86 -> 0l
    | `x86_64 -> 1l
  )
and packed_gen__header_t_architecture_t x =
  Piqirun.int32_to_packed_signed_varint (match x with
    | `x86 -> 0l
    | `x86_64 -> 1l
  )

and gen__metadata_t code x =
  let _typeid = Piqirun.gen_required_field 1 gen__metadata_t_typeid_metadata_t x.Metadata_t.typeid in
  let _module_metadata = Piqirun.gen_optional_field 2 gen__string x.Metadata_t.module_metadata in
  let _exception_metadata = Piqirun.gen_optional_field 3 gen__metadata_t_exception_t x.Metadata_t.exception_metadata in
  let _wave_metadata = Piqirun.gen_optional_field 4 gen__uint32 x.Metadata_t.wave_metadata in
  Piqirun.gen_record code (_typeid :: _module_metadata :: _exception_metadata :: _wave_metadata :: [])

and gen__metadata_t_exception_t code x =
  let _type_exception = Piqirun.gen_required_field 1 gen__uint32 x.Metadata_t_exception_t.type_exception in
  let _handler = Piqirun.gen_required_field 2 gen__uint64 x.Metadata_t_exception_t.handler in
  Piqirun.gen_record code (_type_exception :: _handler :: [])

and gen__metadata_t_typeid_metadata_t code x =
  Piqirun.int32_to_signed_varint code (match x with
    | `invalid_metadata -> 0l
    | `exception_type -> 1l
    | `module_type -> 2l
    | `wave_type -> 3l
  )
and packed_gen__metadata_t_typeid_metadata_t x =
  Piqirun.int32_to_packed_signed_varint (match x with
    | `invalid_metadata -> 0l
    | `exception_type -> 1l
    | `module_type -> 2l
    | `wave_type -> 3l
  )

and gen__ins_con_info_t code x =
  let _typeid = Piqirun.gen_required_field 1 gen__ins_con_info_t_typeid_con_info_t x.Ins_con_info_t.typeid in
  let _read_register = Piqirun.gen_optional_field 2 Common.gen__register_t x.Ins_con_info_t.read_register in
  let _write_register = Piqirun.gen_optional_field 3 Common.gen__register_t x.Ins_con_info_t.write_register in
  let _load_memory = Piqirun.gen_optional_field 4 Common.gen__memory_t x.Ins_con_info_t.load_memory in
  let _store_memory = Piqirun.gen_optional_field 5 Common.gen__memory_t x.Ins_con_info_t.store_memory in
  let _call = Piqirun.gen_optional_field 6 Libcall.gen__libcall_t x.Ins_con_info_t.call in
  let _system_call = Piqirun.gen_optional_field 8 Syscall.gen__syscall_t x.Ins_con_info_t.system_call in
  let _next_address = Piqirun.gen_optional_field 9 gen__uint64 x.Ins_con_info_t.next_address in
  let _reserved_comment = Piqirun.gen_optional_field 10 gen__string x.Ins_con_info_t.reserved_comment in
  let _wave = Piqirun.gen_optional_field 11 gen__uint32 x.Ins_con_info_t.wave in
  Piqirun.gen_record code (_typeid :: _read_register :: _write_register :: _load_memory :: _store_memory :: _call :: _system_call :: _next_address :: _reserved_comment :: _wave :: [])

and gen__ins_con_info_t_typeid_con_info_t code x =
  Piqirun.int32_to_signed_varint code (match x with
    | `invalid -> 0l
    | `regread -> 1l
    | `regwrite -> 2l
    | `memload -> 3l
    | `memstore -> 4l
    | `call -> 5l
    | `syscall -> 6l
    | `not_retrieved -> 7l
    | `next_address -> 8l
    | `comment -> 9l
    | `wave -> 10l
  )
and packed_gen__ins_con_info_t_typeid_con_info_t x =
  Piqirun.int32_to_packed_signed_varint (match x with
    | `invalid -> 0l
    | `regread -> 1l
    | `regwrite -> 2l
    | `memload -> 3l
    | `memstore -> 4l
    | `call -> 5l
    | `syscall -> 6l
    | `not_retrieved -> 7l
    | `next_address -> 8l
    | `comment -> 9l
    | `wave -> 10l
  )

and gen__instruction_t code x =
  let _thread_id = Piqirun.gen_required_field 1 gen__uint32 x.Instruction_t.thread_id in
  let _address = Piqirun.gen_required_field 2 gen__uint64 x.Instruction_t.address in
  let _opcode = Piqirun.gen_required_field 3 gen__binary x.Instruction_t.opcode in
  let _concrete_infos = Piqirun.gen_repeated_field 4 gen__ins_con_info_t x.Instruction_t.concrete_infos in
  let _dba_instrs = Piqirun.gen_optional_field 5 Dba.gen__dba_list x.Instruction_t.dba_instrs in
  Piqirun.gen_record code (_thread_id :: _address :: _opcode :: _concrete_infos :: _dba_instrs :: [])

and gen__body_t code x =
  let _typeid = Piqirun.gen_required_field 1 gen__body_t_typeid_body_t x.Body_t.typeid in
  let _metadata = Piqirun.gen_optional_field 2 gen__metadata_t x.Body_t.metadata in
  let _instruction = Piqirun.gen_optional_field 3 gen__instruction_t x.Body_t.instruction in
  Piqirun.gen_record code (_typeid :: _metadata :: _instruction :: [])

and gen__body_t_typeid_body_t code x =
  Piqirun.int32_to_signed_varint code (match x with
    | `metadata -> 0l
    | `instruction -> 1l
  )
and packed_gen__body_t_typeid_body_t x =
  Piqirun.int32_to_packed_signed_varint (match x with
    | `metadata -> 0l
    | `instruction -> 1l
  )

and gen__chunk_t code x =
  let _body = Piqirun.gen_repeated_field 1 gen__body_t x.Chunk_t.body in
  Piqirun.gen_record code (_body :: [])

and gen__trace_t code x =
  let _header = Piqirun.gen_required_field 1 gen__header_t x.Trace_t.header in
  let _body = Piqirun.gen_repeated_field 2 gen__body_t x.Trace_t.body in
  Piqirun.gen_record code (_header :: _body :: [])


let gen_string x = gen__string (-1) x
let gen_uint32 x = gen__uint32 (-1) x
let gen_uint64 x = gen__uint64 (-1) x
let gen_binary x = gen__binary (-1) x
let gen_header_t x = gen__header_t (-1) x
let gen_header_t_architecture_t x = gen__header_t_architecture_t (-1) x
let gen_metadata_t x = gen__metadata_t (-1) x
let gen_metadata_t_exception_t x = gen__metadata_t_exception_t (-1) x
let gen_metadata_t_typeid_metadata_t x = gen__metadata_t_typeid_metadata_t (-1) x
let gen_ins_con_info_t x = gen__ins_con_info_t (-1) x
let gen_ins_con_info_t_typeid_con_info_t x = gen__ins_con_info_t_typeid_con_info_t (-1) x
let gen_instruction_t x = gen__instruction_t (-1) x
let gen_body_t x = gen__body_t (-1) x
let gen_body_t_typeid_body_t x = gen__body_t_typeid_body_t (-1) x
let gen_chunk_t x = gen__chunk_t (-1) x
let gen_trace_t x = gen__trace_t (-1) x


let rec default_string () = ""
and default_uint32 () = 0l
and default_uint64 () = 0L
and default_binary () = ""
and default_header_t () =
  {
    Header_t.architecture = default_header_t_architecture_t ();
    Header_t.address_size = Common.default_proto_size_t ();
    Header_t.kind = Common.parse_ir_kind_t (Piqirun.parse_default "\b\001");
  }
and default_header_t_architecture_t () = `x86
and default_metadata_t () =
  {
    Metadata_t.typeid = default_metadata_t_typeid_metadata_t ();
    Metadata_t.module_metadata = None;
    Metadata_t.exception_metadata = None;
    Metadata_t.wave_metadata = None;
  }
and default_metadata_t_exception_t () =
  {
    Metadata_t_exception_t.type_exception = default_uint32 ();
    Metadata_t_exception_t.handler = default_uint64 ();
  }
and default_metadata_t_typeid_metadata_t () = `invalid_metadata
and default_ins_con_info_t () =
  {
    Ins_con_info_t.typeid = default_ins_con_info_t_typeid_con_info_t ();
    Ins_con_info_t.read_register = None;
    Ins_con_info_t.write_register = None;
    Ins_con_info_t.load_memory = None;
    Ins_con_info_t.store_memory = None;
    Ins_con_info_t.call = None;
    Ins_con_info_t.system_call = None;
    Ins_con_info_t.next_address = None;
    Ins_con_info_t.reserved_comment = None;
    Ins_con_info_t.wave = None;
  }
and default_ins_con_info_t_typeid_con_info_t () = `invalid
and default_instruction_t () =
  {
    Instruction_t.thread_id = default_uint32 ();
    Instruction_t.address = default_uint64 ();
    Instruction_t.opcode = default_binary ();
    Instruction_t.concrete_infos = [];
    Instruction_t.dba_instrs = None;
  }
and default_body_t () =
  {
    Body_t.typeid = default_body_t_typeid_body_t ();
    Body_t.metadata = None;
    Body_t.instruction = None;
  }
and default_body_t_typeid_body_t () = `metadata
and default_chunk_t () =
  {
    Chunk_t.body = [];
  }
and default_trace_t () =
  {
    Trace_t.header = default_header_t ();
    Trace_t.body = [];
  }


let piqi = "\226\202\2304\005trace\226\231\249\238\001\015piqi/trace.piqi\170\150\212\160\004\b\226\202\2304\003dba\170\150\212\160\004\011\226\202\2304\006common\170\150\212\160\004\012\226\202\2304\007syscall\170\150\212\160\004\012\226\202\2304\007libcall\162\244\146\155\011\012trace_format\218\244\134\182\012\178\002\138\233\142\251\014\171\002\210\203\242$\\\232\146\150q\002\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\012architecture\208\215\133\174\005\000\210\171\158\194\006\023header-t-architecture-t\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$X\232\146\150q\004\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\012address-size\208\215\133\174\005\000\210\171\158\194\006\019common/proto-size-t\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$Z\232\146\150q\006\152\247\223\136\002\000\152\182\154\152\004\160\223\186\243\001\232\243\204\157\004\000\218\164\238\191\004\004kind\208\215\133\174\005\000\210\171\158\194\006\016common/ir-kind-t\192\139\160\247\t\000\138\140\251\240\r\007\218\148\211\024\002\b\001\136\158\147\199\014\000\218\164\238\191\004\bheader-t\218\244\134\182\012\129\001\138\176\205\197\001{\218\164\238\191\004\023header-t-architecture-t\170\183\218\222\005&\232\146\150q\000\152\247\223\136\002\000\234\188\204\215\002\012header_t_X86\218\164\238\191\004\003X86\170\183\218\222\005,\232\146\150q\002\152\247\223\136\002\000\234\188\204\215\002\015header_t_X86_64\218\164\238\191\004\006X86-64\218\244\134\182\012\129\003\138\233\142\251\014\250\002\210\203\242$[\232\146\150q\002\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\006typeid\208\215\133\174\005\000\210\171\158\194\006\028metadata-t-typeid-metadata-t\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$N\232\146\150q\004\152\247\223\136\002\000\152\182\154\152\004\160\223\186\243\001\232\243\204\157\004\000\218\164\238\191\004\015module-metadata\208\215\133\174\005\000\210\171\158\194\006\006string\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$a\232\146\150q\006\152\247\223\136\002\000\152\182\154\152\004\160\223\186\243\001\232\243\204\157\004\000\218\164\238\191\004\018exception-metadata\208\215\133\174\005\000\210\171\158\194\006\022metadata-t-exception-t\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$L\232\146\150q\b\152\247\223\136\002\000\152\182\154\152\004\160\223\186\243\001\232\243\204\157\004\000\218\164\238\191\004\rwave-metadata\208\215\133\174\005\000\210\171\158\194\006\006uint32\192\139\160\247\t\000\136\158\147\199\014\000\218\164\238\191\004\nmetadata-t\218\244\134\182\012\192\001\138\233\142\251\014\185\001\210\203\242$M\232\146\150q\002\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\014type-exception\208\215\133\174\005\000\210\171\158\194\006\006uint32\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$F\232\146\150q\004\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\007handler\208\215\133\174\005\000\210\171\158\194\006\006uint64\192\139\160\247\t\000\136\158\147\199\014\000\218\164\238\191\004\022metadata-t-exception-t\218\244\134\182\012\173\002\138\176\205\197\001\166\002\218\164\238\191\004\028metadata-t-typeid-metadata-t\170\183\218\222\005B\232\146\150q\000\152\247\223\136\002\000\234\188\204\215\002\027metadata_t_INVALID_METADATA\218\164\238\191\004\016INVALID-METADATA\170\183\218\222\005>\232\146\150q\002\152\247\223\136\002\000\234\188\204\215\002\025metadata_t_EXCEPTION_TYPE\218\164\238\191\004\014EXCEPTION-TYPE\170\183\218\222\0058\232\146\150q\004\152\247\223\136\002\000\234\188\204\215\002\022metadata_t_MODULE_TYPE\218\164\238\191\004\011MODULE-TYPE\170\183\218\222\0054\232\146\150q\006\152\247\223\136\002\000\234\188\204\215\002\020metadata_t_WAVE_TYPE\218\164\238\191\004\tWAVE-TYPE\218\244\134\182\012\130\007\138\233\142\251\014\251\006\210\203\242$_\232\146\150q\002\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\006typeid\208\215\133\174\005\000\210\171\158\194\006 ins-con-info-t-typeid-con-info-t\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$W\232\146\150q\004\152\247\223\136\002\000\152\182\154\152\004\160\223\186\243\001\232\243\204\157\004\000\218\164\238\191\004\rread-register\208\215\133\174\005\000\210\171\158\194\006\017common/register-t\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$X\232\146\150q\006\152\247\223\136\002\000\152\182\154\152\004\160\223\186\243\001\232\243\204\157\004\000\218\164\238\191\004\014write-register\208\215\133\174\005\000\210\171\158\194\006\017common/register-t\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$S\232\146\150q\b\152\247\223\136\002\000\152\182\154\152\004\160\223\186\243\001\232\243\204\157\004\000\218\164\238\191\004\011load-memory\208\215\133\174\005\000\210\171\158\194\006\015common/memory-t\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$T\232\146\150q\n\152\247\223\136\002\000\152\182\154\152\004\160\223\186\243\001\232\243\204\157\004\000\218\164\238\191\004\012store-memory\208\215\133\174\005\000\210\171\158\194\006\015common/memory-t\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$N\232\146\150q\012\152\247\223\136\002\000\152\182\154\152\004\160\223\186\243\001\232\243\204\157\004\000\218\164\238\191\004\004call\208\215\133\174\005\000\210\171\158\194\006\017libcall/libcall-t\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$U\232\146\150q\016\152\247\223\136\002\000\152\182\154\152\004\160\223\186\243\001\232\243\204\157\004\000\218\164\238\191\004\011system-call\208\215\133\174\005\000\210\171\158\194\006\017syscall/syscall-t\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$K\232\146\150q\018\152\247\223\136\002\000\152\182\154\152\004\160\223\186\243\001\232\243\204\157\004\000\218\164\238\191\004\012next-address\208\215\133\174\005\000\210\171\158\194\006\006uint64\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$O\232\146\150q\020\152\247\223\136\002\000\152\182\154\152\004\160\223\186\243\001\232\243\204\157\004\000\218\164\238\191\004\016reserved-comment\208\215\133\174\005\000\210\171\158\194\006\006string\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$C\232\146\150q\022\152\247\223\136\002\000\152\182\154\152\004\160\223\186\243\001\232\243\204\157\004\000\218\164\238\191\004\004wave\208\215\133\174\005\000\210\171\158\194\006\006uint32\192\139\160\247\t\000\136\158\147\199\014\000\218\164\238\191\004\014ins-con-info-t\218\244\134\182\012\185\005\138\176\205\197\001\178\005\218\164\238\191\004 ins-con-info-t-typeid-con-info-t\170\183\218\222\0054\232\146\150q\000\152\247\223\136\002\000\234\188\204\215\002\022ins_con_info_t_INVALID\218\164\238\191\004\007INVALID\170\183\218\222\0054\232\146\150q\002\152\247\223\136\002\000\234\188\204\215\002\022ins_con_info_t_REGREAD\218\164\238\191\004\007REGREAD\170\183\218\222\0056\232\146\150q\004\152\247\223\136\002\000\234\188\204\215\002\023ins_con_info_t_REGWRITE\218\164\238\191\004\bREGWRITE\170\183\218\222\0054\232\146\150q\006\152\247\223\136\002\000\234\188\204\215\002\022ins_con_info_t_MEMLOAD\218\164\238\191\004\007MEMLOAD\170\183\218\222\0056\232\146\150q\b\152\247\223\136\002\000\234\188\204\215\002\023ins_con_info_t_MEMSTORE\218\164\238\191\004\bMEMSTORE\170\183\218\222\005.\232\146\150q\n\152\247\223\136\002\000\234\188\204\215\002\019ins_con_info_t_CALL\218\164\238\191\004\004CALL\170\183\218\222\0054\232\146\150q\012\152\247\223\136\002\000\234\188\204\215\002\022ins_con_info_t_SYSCALL\218\164\238\191\004\007SYSCALL\170\183\218\222\005@\232\146\150q\014\152\247\223\136\002\000\234\188\204\215\002\028ins_con_info_t_NOT_RETRIEVED\218\164\238\191\004\rNOT-RETRIEVED\170\183\218\222\005>\232\146\150q\016\152\247\223\136\002\000\234\188\204\215\002\027ins_con_info_t_NEXT_ADDRESS\218\164\238\191\004\012NEXT-ADDRESS\170\183\218\222\0054\232\146\150q\018\152\247\223\136\002\000\234\188\204\215\002\022ins_con_info_t_COMMENT\218\164\238\191\004\007COMMENT\170\183\218\222\005.\232\146\150q\020\152\247\223\136\002\000\234\188\204\215\002\019ins_con_info_t_WAVE\218\164\238\191\004\004WAVE\218\244\134\182\012\170\003\138\233\142\251\014\163\003\210\203\242$H\232\146\150q\002\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\tthread-id\208\215\133\174\005\000\210\171\158\194\006\006uint32\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$F\232\146\150q\004\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\007address\208\215\133\174\005\000\210\171\158\194\006\006uint64\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$E\232\146\150q\006\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\006opcode\208\215\133\174\005\000\210\171\158\194\006\006binary\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$U\232\146\150q\b\152\247\223\136\002\000\152\182\154\152\004\250\248\214\130\001\232\243\204\157\004\000\218\164\238\191\004\014concrete-infos\208\215\133\174\005\000\210\171\158\194\006\014ins-con-info-t\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$O\232\146\150q\n\152\247\223\136\002\000\152\182\154\152\004\160\223\186\243\001\232\243\204\157\004\000\218\164\238\191\004\ndba-instrs\208\215\133\174\005\000\210\171\158\194\006\012dba/dba-list\192\139\160\247\t\000\136\158\147\199\014\000\218\164\238\191\004\rinstruction-t\218\244\134\182\012\145\002\138\233\142\251\014\138\002\210\203\242$S\232\146\150q\002\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\006typeid\208\215\133\174\005\000\210\171\158\194\006\020body-t-typeid-body-t\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$K\232\146\150q\004\152\247\223\136\002\000\152\182\154\152\004\160\223\186\243\001\232\243\204\157\004\000\218\164\238\191\004\bmetadata\208\215\133\174\005\000\210\171\158\194\006\nmetadata-t\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$Q\232\146\150q\006\152\247\223\136\002\000\152\182\154\152\004\160\223\186\243\001\232\243\204\157\004\000\218\164\238\191\004\011instruction\208\215\133\174\005\000\210\171\158\194\006\rinstruction-t\192\139\160\247\t\000\136\158\147\199\014\000\218\164\238\191\004\006body-t\218\244\134\182\012\143\001\138\176\205\197\001\136\001\218\164\238\191\004\020body-t-typeid-body-t\170\183\218\222\005.\232\146\150q\000\152\247\223\136\002\000\234\188\204\215\002\015body_t_METADATA\218\164\238\191\004\bMETADATA\170\183\218\222\0054\232\146\150q\002\152\247\223\136\002\000\234\188\204\215\002\018body_t_INSTRUCTION\218\164\238\191\004\011INSTRUCTION\218\244\134\182\012[\138\233\142\251\014U\210\203\242$C\232\146\150q\002\152\247\223\136\002\000\152\182\154\152\004\250\248\214\130\001\232\243\204\157\004\000\218\164\238\191\004\004body\208\215\133\174\005\000\210\171\158\194\006\006body-t\192\139\160\247\t\000\136\158\147\199\014\000\218\164\238\191\004\007chunk-t\218\244\134\182\012\168\001\138\233\142\251\014\161\001\210\203\242$G\232\146\150q\002\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\006header\208\215\133\174\005\000\210\171\158\194\006\bheader-t\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$C\232\146\150q\004\152\247\223\136\002\000\152\182\154\152\004\250\248\214\130\001\232\243\204\157\004\000\218\164\238\191\004\004body\208\215\133\174\005\000\210\171\158\194\006\006body-t\192\139\160\247\t\000\136\158\147\199\014\000\218\164\238\191\004\007trace-t"
include Trace_piqi
