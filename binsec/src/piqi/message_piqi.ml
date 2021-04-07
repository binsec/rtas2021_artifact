module  Common = Common_piqi

module  Dba = Dba_piqi

module  Config = Config_piqi

module rec Message_piqi:
  sig
    type uint64 = int64
    type uint32 = int32
    type message_decode_instr_bin_kind =
      [
        | `hexa
        | `bin
      ]
    type message_start_symb_exec_trace_kind =
      [
        | `file
        | `stream
      ]
    type message_output_output_kind =
      [
        | `success
        | `result
        | `debug
        | `warning
        | `error
        | `failure
      ]
    type message_start_exec = Message_start_exec.t
    type message_bp_reached = Message_bp_reached.t
    type message_patch = Message_patch.t
    type message_infos = Message_infos.t
    type message_decode_instr = Message_decode_instr.t
    type message_decode_instr_instr_entry = Message_decode_instr_instr_entry.t
    type message_decode_instr_reply = Message_decode_instr_reply.t
    type message_decode_instr_reply_instr_entry = Message_decode_instr_reply_instr_entry.t
    type message_start_symb_exec = Message_start_symb_exec.t
    type message_output = Message_output.t
  end = Message_piqi
and Message_start_exec:
  sig
    type t = {
      mutable start: Message_piqi.uint64;
      mutable stop: Message_piqi.uint64;
      mutable size_chunks: Message_piqi.uint32;
      mutable breakpoints: Message_piqi.uint64 list;
      mutable inputs: Config.input_t list;
      mutable skips: Message_piqi.uint64 list;
    }
  end = Message_start_exec
and Message_bp_reached:
  sig
    type t = {
      mutable addr: Message_piqi.uint64;
    }
  end = Message_bp_reached
and Message_patch:
  sig
    type t = {
      mutable patch: Config.input_t list;
    }
  end = Message_patch
and Message_infos:
  sig
    type t = {
      mutable nb_workers: Message_piqi.uint32;
      mutable analyses: string list;
      mutable solvers: string list;
    }
  end = Message_infos
and Message_decode_instr:
  sig
    type t = {
      mutable instrs: Message_piqi.message_decode_instr_instr_entry list;
      mutable kind: Message_piqi.message_decode_instr_bin_kind;
      mutable irkind: Common.ir_kind_t;
    }
  end = Message_decode_instr
and Message_decode_instr_instr_entry:
  sig
    type t = {
      mutable instr: string;
      mutable base_addr: Message_piqi.uint64;
    }
  end = Message_decode_instr_instr_entry
and Message_decode_instr_reply:
  sig
    type t = {
      mutable instrs: Message_piqi.message_decode_instr_reply_instr_entry list;
    }
  end = Message_decode_instr_reply
and Message_decode_instr_reply_instr_entry:
  sig
    type t = {
      mutable opcode: string;
      mutable irkind: Common.ir_kind_t;
      mutable dba_instrs: Dba.dba_list option;
    }
  end = Message_decode_instr_reply_instr_entry
and Message_start_symb_exec:
  sig
    type t = {
      mutable trace_type: Message_piqi.message_start_symb_exec_trace_kind;
      mutable name: string;
      mutable solver: Common.solver_t;
      mutable config: Config.configuration option;
      mutable irkind: Common.ir_kind_t;
      mutable formula_optim_cstfold: bool option;
      mutable formula_optim_rebase: bool option;
      mutable formula_optim_row: bool option;
      mutable verbose: Message_piqi.uint32;
      mutable addr_predicate: Message_piqi.uint64 option;
      mutable predicate: string option;
      mutable trace_filename: string option;
    }
  end = Message_start_symb_exec
and Message_output:
  sig
    type t = {
      mutable type_: Message_piqi.message_output_output_kind;
      mutable message: string;
      mutable ram_total: Message_piqi.uint32 option;
      mutable ram_free: Message_piqi.uint32 option;
      mutable ram_available: Message_piqi.uint32 option;
    }
  end = Message_output


let rec parse_uint64 x = Piqirun.int64_of_varint x
and packed_parse_uint64 x = Piqirun.int64_of_packed_varint x

and parse_uint32 x = Piqirun.int32_of_varint x
and packed_parse_uint32 x = Piqirun.int32_of_packed_varint x

and parse_string x = Piqirun.string_of_block x

and parse_bool x = Piqirun.bool_of_varint x
and packed_parse_bool x = Piqirun.bool_of_packed_varint x

and parse_message_start_exec x =
  let x = Piqirun.parse_record x in
  let _start, x = Piqirun.parse_required_field 1 parse_uint64 x in
  let _stop, x = Piqirun.parse_required_field 2 parse_uint64 x in
  let _size_chunks, x = Piqirun.parse_required_field 3 parse_uint32 x ~default:"\b\232\007" in
  let _breakpoints, x = Piqirun.parse_repeated_field 4 parse_uint64 x in
  let _inputs, x = Piqirun.parse_repeated_field 5 Config.parse_input_t x in
  let _skips, x = Piqirun.parse_repeated_field 6 parse_uint64 x in
  Piqirun.check_unparsed_fields x;
  {
    Message_start_exec.start = _start;
    Message_start_exec.stop = _stop;
    Message_start_exec.size_chunks = _size_chunks;
    Message_start_exec.breakpoints = _breakpoints;
    Message_start_exec.inputs = _inputs;
    Message_start_exec.skips = _skips;
  }

and parse_message_bp_reached x =
  let x = Piqirun.parse_record x in
  let _addr, x = Piqirun.parse_required_field 1 parse_uint64 x in
  Piqirun.check_unparsed_fields x;
  {
    Message_bp_reached.addr = _addr;
  }

and parse_message_patch x =
  let x = Piqirun.parse_record x in
  let _patch, x = Piqirun.parse_repeated_field 1 Config.parse_input_t x in
  Piqirun.check_unparsed_fields x;
  {
    Message_patch.patch = _patch;
  }

and parse_message_infos x =
  let x = Piqirun.parse_record x in
  let _nb_workers, x = Piqirun.parse_required_field 1 parse_uint32 x in
  let _analyses, x = Piqirun.parse_repeated_field 2 parse_string x in
  let _solvers, x = Piqirun.parse_repeated_field 3 parse_string x in
  Piqirun.check_unparsed_fields x;
  {
    Message_infos.nb_workers = _nb_workers;
    Message_infos.analyses = _analyses;
    Message_infos.solvers = _solvers;
  }

and parse_message_decode_instr x =
  let x = Piqirun.parse_record x in
  let _instrs, x = Piqirun.parse_repeated_field 1 parse_message_decode_instr_instr_entry x in
  let _kind, x = Piqirun.parse_required_field 2 parse_message_decode_instr_bin_kind x ~default:"\b\002" in
  let _irkind, x = Piqirun.parse_required_field 3 Common.parse_ir_kind_t x ~default:"\b\001" in
  Piqirun.check_unparsed_fields x;
  {
    Message_decode_instr.instrs = _instrs;
    Message_decode_instr.kind = _kind;
    Message_decode_instr.irkind = _irkind;
  }

and parse_message_decode_instr_instr_entry x =
  let x = Piqirun.parse_record x in
  let _instr, x = Piqirun.parse_required_field 1 parse_string x in
  let _base_addr, x = Piqirun.parse_required_field 2 parse_uint64 x ~default:"\b\000" in
  Piqirun.check_unparsed_fields x;
  {
    Message_decode_instr_instr_entry.instr = _instr;
    Message_decode_instr_instr_entry.base_addr = _base_addr;
  }

and parse_message_decode_instr_bin_kind x =
  match Piqirun.int32_of_signed_varint x with
    | 1l -> `hexa
    | 2l -> `bin
    | x -> Piqirun.error_enum_const x
and packed_parse_message_decode_instr_bin_kind x =
  match Piqirun.int32_of_packed_signed_varint x with
    | 1l -> `hexa
    | 2l -> `bin
    | x -> Piqirun.error_enum_const x

and parse_message_decode_instr_reply x =
  let x = Piqirun.parse_record x in
  let _instrs, x = Piqirun.parse_repeated_field 1 parse_message_decode_instr_reply_instr_entry x in
  Piqirun.check_unparsed_fields x;
  {
    Message_decode_instr_reply.instrs = _instrs;
  }

and parse_message_decode_instr_reply_instr_entry x =
  let x = Piqirun.parse_record x in
  let _opcode, x = Piqirun.parse_required_field 1 parse_string x in
  let _irkind, x = Piqirun.parse_required_field 2 Common.parse_ir_kind_t x in
  let _dba_instrs, x = Piqirun.parse_optional_field 3 Dba.parse_dba_list x in
  Piqirun.check_unparsed_fields x;
  {
    Message_decode_instr_reply_instr_entry.opcode = _opcode;
    Message_decode_instr_reply_instr_entry.irkind = _irkind;
    Message_decode_instr_reply_instr_entry.dba_instrs = _dba_instrs;
  }

and parse_message_start_symb_exec x =
  let x = Piqirun.parse_record x in
  let _trace_type, x = Piqirun.parse_required_field 1 parse_message_start_symb_exec_trace_kind x in
  let _name, x = Piqirun.parse_required_field 2 parse_string x in
  let _solver, x = Piqirun.parse_required_field 3 Common.parse_solver_t x ~default:"\b\001" in
  let _config, x = Piqirun.parse_optional_field 4 Config.parse_configuration x in
  let _irkind, x = Piqirun.parse_required_field 5 Common.parse_ir_kind_t x ~default:"\b\001" in
  let _formula_optim_cstfold, x = Piqirun.parse_optional_field 6 parse_bool x in
  let _formula_optim_rebase, x = Piqirun.parse_optional_field 7 parse_bool x in
  let _formula_optim_row, x = Piqirun.parse_optional_field 8 parse_bool x in
  let _verbose, x = Piqirun.parse_required_field 9 parse_uint32 x in
  let _addr_predicate, x = Piqirun.parse_optional_field 10 parse_uint64 x in
  let _predicate, x = Piqirun.parse_optional_field 11 parse_string x in
  let _trace_filename, x = Piqirun.parse_optional_field 12 parse_string x in
  Piqirun.check_unparsed_fields x;
  {
    Message_start_symb_exec.trace_type = _trace_type;
    Message_start_symb_exec.name = _name;
    Message_start_symb_exec.solver = _solver;
    Message_start_symb_exec.config = _config;
    Message_start_symb_exec.irkind = _irkind;
    Message_start_symb_exec.formula_optim_cstfold = _formula_optim_cstfold;
    Message_start_symb_exec.formula_optim_rebase = _formula_optim_rebase;
    Message_start_symb_exec.formula_optim_row = _formula_optim_row;
    Message_start_symb_exec.verbose = _verbose;
    Message_start_symb_exec.addr_predicate = _addr_predicate;
    Message_start_symb_exec.predicate = _predicate;
    Message_start_symb_exec.trace_filename = _trace_filename;
  }

and parse_message_start_symb_exec_trace_kind x =
  match Piqirun.int32_of_signed_varint x with
    | 1l -> `file
    | 2l -> `stream
    | x -> Piqirun.error_enum_const x
and packed_parse_message_start_symb_exec_trace_kind x =
  match Piqirun.int32_of_packed_signed_varint x with
    | 1l -> `file
    | 2l -> `stream
    | x -> Piqirun.error_enum_const x

and parse_message_output x =
  let x = Piqirun.parse_record x in
  let _type_, x = Piqirun.parse_required_field 1 parse_message_output_output_kind x in
  let _message, x = Piqirun.parse_required_field 2 parse_string x in
  let _ram_total, x = Piqirun.parse_optional_field 3 parse_uint32 x in
  let _ram_free, x = Piqirun.parse_optional_field 4 parse_uint32 x in
  let _ram_available, x = Piqirun.parse_optional_field 5 parse_uint32 x in
  Piqirun.check_unparsed_fields x;
  {
    Message_output.type_ = _type_;
    Message_output.message = _message;
    Message_output.ram_total = _ram_total;
    Message_output.ram_free = _ram_free;
    Message_output.ram_available = _ram_available;
  }

and parse_message_output_output_kind x =
  match Piqirun.int32_of_signed_varint x with
    | 1l -> `success
    | 2l -> `result
    | 3l -> `debug
    | 4l -> `warning
    | 5l -> `error
    | 6l -> `failure
    | x -> Piqirun.error_enum_const x
and packed_parse_message_output_output_kind x =
  match Piqirun.int32_of_packed_signed_varint x with
    | 1l -> `success
    | 2l -> `result
    | 3l -> `debug
    | 4l -> `warning
    | 5l -> `error
    | 6l -> `failure
    | x -> Piqirun.error_enum_const x


let rec gen__uint64 code x = Piqirun.int64_to_varint code x
and packed_gen__uint64 x = Piqirun.int64_to_packed_varint x

and gen__uint32 code x = Piqirun.int32_to_varint code x
and packed_gen__uint32 x = Piqirun.int32_to_packed_varint x

and gen__string code x = Piqirun.string_to_block code x

and gen__bool code x = Piqirun.bool_to_varint code x
and packed_gen__bool x = Piqirun.bool_to_packed_varint x

and gen__message_start_exec code x =
  let _start = Piqirun.gen_required_field 1 gen__uint64 x.Message_start_exec.start in
  let _stop = Piqirun.gen_required_field 2 gen__uint64 x.Message_start_exec.stop in
  let _size_chunks = Piqirun.gen_required_field 3 gen__uint32 x.Message_start_exec.size_chunks in
  let _breakpoints = Piqirun.gen_repeated_field 4 gen__uint64 x.Message_start_exec.breakpoints in
  let _inputs = Piqirun.gen_repeated_field 5 Config.gen__input_t x.Message_start_exec.inputs in
  let _skips = Piqirun.gen_repeated_field 6 gen__uint64 x.Message_start_exec.skips in
  Piqirun.gen_record code (_start :: _stop :: _size_chunks :: _breakpoints :: _inputs :: _skips :: [])

and gen__message_bp_reached code x =
  let _addr = Piqirun.gen_required_field 1 gen__uint64 x.Message_bp_reached.addr in
  Piqirun.gen_record code (_addr :: [])

and gen__message_patch code x =
  let _patch = Piqirun.gen_repeated_field 1 Config.gen__input_t x.Message_patch.patch in
  Piqirun.gen_record code (_patch :: [])

and gen__message_infos code x =
  let _nb_workers = Piqirun.gen_required_field 1 gen__uint32 x.Message_infos.nb_workers in
  let _analyses = Piqirun.gen_repeated_field 2 gen__string x.Message_infos.analyses in
  let _solvers = Piqirun.gen_repeated_field 3 gen__string x.Message_infos.solvers in
  Piqirun.gen_record code (_nb_workers :: _analyses :: _solvers :: [])

and gen__message_decode_instr code x =
  let _instrs = Piqirun.gen_repeated_field 1 gen__message_decode_instr_instr_entry x.Message_decode_instr.instrs in
  let _kind = Piqirun.gen_required_field 2 gen__message_decode_instr_bin_kind x.Message_decode_instr.kind in
  let _irkind = Piqirun.gen_required_field 3 Common.gen__ir_kind_t x.Message_decode_instr.irkind in
  Piqirun.gen_record code (_instrs :: _kind :: _irkind :: [])

and gen__message_decode_instr_instr_entry code x =
  let _instr = Piqirun.gen_required_field 1 gen__string x.Message_decode_instr_instr_entry.instr in
  let _base_addr = Piqirun.gen_required_field 2 gen__uint64 x.Message_decode_instr_instr_entry.base_addr in
  Piqirun.gen_record code (_instr :: _base_addr :: [])

and gen__message_decode_instr_bin_kind code x =
  Piqirun.int32_to_signed_varint code (match x with
    | `hexa -> 1l
    | `bin -> 2l
  )
and packed_gen__message_decode_instr_bin_kind x =
  Piqirun.int32_to_packed_signed_varint (match x with
    | `hexa -> 1l
    | `bin -> 2l
  )

and gen__message_decode_instr_reply code x =
  let _instrs = Piqirun.gen_repeated_field 1 gen__message_decode_instr_reply_instr_entry x.Message_decode_instr_reply.instrs in
  Piqirun.gen_record code (_instrs :: [])

and gen__message_decode_instr_reply_instr_entry code x =
  let _opcode = Piqirun.gen_required_field 1 gen__string x.Message_decode_instr_reply_instr_entry.opcode in
  let _irkind = Piqirun.gen_required_field 2 Common.gen__ir_kind_t x.Message_decode_instr_reply_instr_entry.irkind in
  let _dba_instrs = Piqirun.gen_optional_field 3 Dba.gen__dba_list x.Message_decode_instr_reply_instr_entry.dba_instrs in
  Piqirun.gen_record code (_opcode :: _irkind :: _dba_instrs :: [])

and gen__message_start_symb_exec code x =
  let _trace_type = Piqirun.gen_required_field 1 gen__message_start_symb_exec_trace_kind x.Message_start_symb_exec.trace_type in
  let _name = Piqirun.gen_required_field 2 gen__string x.Message_start_symb_exec.name in
  let _solver = Piqirun.gen_required_field 3 Common.gen__solver_t x.Message_start_symb_exec.solver in
  let _config = Piqirun.gen_optional_field 4 Config.gen__configuration x.Message_start_symb_exec.config in
  let _irkind = Piqirun.gen_required_field 5 Common.gen__ir_kind_t x.Message_start_symb_exec.irkind in
  let _formula_optim_cstfold = Piqirun.gen_optional_field 6 gen__bool x.Message_start_symb_exec.formula_optim_cstfold in
  let _formula_optim_rebase = Piqirun.gen_optional_field 7 gen__bool x.Message_start_symb_exec.formula_optim_rebase in
  let _formula_optim_row = Piqirun.gen_optional_field 8 gen__bool x.Message_start_symb_exec.formula_optim_row in
  let _verbose = Piqirun.gen_required_field 9 gen__uint32 x.Message_start_symb_exec.verbose in
  let _addr_predicate = Piqirun.gen_optional_field 10 gen__uint64 x.Message_start_symb_exec.addr_predicate in
  let _predicate = Piqirun.gen_optional_field 11 gen__string x.Message_start_symb_exec.predicate in
  let _trace_filename = Piqirun.gen_optional_field 12 gen__string x.Message_start_symb_exec.trace_filename in
  Piqirun.gen_record code (_trace_type :: _name :: _solver :: _config :: _irkind :: _formula_optim_cstfold :: _formula_optim_rebase :: _formula_optim_row :: _verbose :: _addr_predicate :: _predicate :: _trace_filename :: [])

and gen__message_start_symb_exec_trace_kind code x =
  Piqirun.int32_to_signed_varint code (match x with
    | `file -> 1l
    | `stream -> 2l
  )
and packed_gen__message_start_symb_exec_trace_kind x =
  Piqirun.int32_to_packed_signed_varint (match x with
    | `file -> 1l
    | `stream -> 2l
  )

and gen__message_output code x =
  let _type_ = Piqirun.gen_required_field 1 gen__message_output_output_kind x.Message_output.type_ in
  let _message = Piqirun.gen_required_field 2 gen__string x.Message_output.message in
  let _ram_total = Piqirun.gen_optional_field 3 gen__uint32 x.Message_output.ram_total in
  let _ram_free = Piqirun.gen_optional_field 4 gen__uint32 x.Message_output.ram_free in
  let _ram_available = Piqirun.gen_optional_field 5 gen__uint32 x.Message_output.ram_available in
  Piqirun.gen_record code (_type_ :: _message :: _ram_total :: _ram_free :: _ram_available :: [])

and gen__message_output_output_kind code x =
  Piqirun.int32_to_signed_varint code (match x with
    | `success -> 1l
    | `result -> 2l
    | `debug -> 3l
    | `warning -> 4l
    | `error -> 5l
    | `failure -> 6l
  )
and packed_gen__message_output_output_kind x =
  Piqirun.int32_to_packed_signed_varint (match x with
    | `success -> 1l
    | `result -> 2l
    | `debug -> 3l
    | `warning -> 4l
    | `error -> 5l
    | `failure -> 6l
  )


let gen_uint64 x = gen__uint64 (-1) x
let gen_uint32 x = gen__uint32 (-1) x
let gen_string x = gen__string (-1) x
let gen_bool x = gen__bool (-1) x
let gen_message_start_exec x = gen__message_start_exec (-1) x
let gen_message_bp_reached x = gen__message_bp_reached (-1) x
let gen_message_patch x = gen__message_patch (-1) x
let gen_message_infos x = gen__message_infos (-1) x
let gen_message_decode_instr x = gen__message_decode_instr (-1) x
let gen_message_decode_instr_instr_entry x = gen__message_decode_instr_instr_entry (-1) x
let gen_message_decode_instr_bin_kind x = gen__message_decode_instr_bin_kind (-1) x
let gen_message_decode_instr_reply x = gen__message_decode_instr_reply (-1) x
let gen_message_decode_instr_reply_instr_entry x = gen__message_decode_instr_reply_instr_entry (-1) x
let gen_message_start_symb_exec x = gen__message_start_symb_exec (-1) x
let gen_message_start_symb_exec_trace_kind x = gen__message_start_symb_exec_trace_kind (-1) x
let gen_message_output x = gen__message_output (-1) x
let gen_message_output_output_kind x = gen__message_output_output_kind (-1) x


let rec default_uint64 () = 0L
and default_uint32 () = 0l
and default_string () = ""
and default_bool () = false
and default_message_start_exec () =
  {
    Message_start_exec.start = default_uint64 ();
    Message_start_exec.stop = default_uint64 ();
    Message_start_exec.size_chunks = parse_uint32 (Piqirun.parse_default "\b\232\007");
    Message_start_exec.breakpoints = [];
    Message_start_exec.inputs = [];
    Message_start_exec.skips = [];
  }
and default_message_bp_reached () =
  {
    Message_bp_reached.addr = default_uint64 ();
  }
and default_message_patch () =
  {
    Message_patch.patch = [];
  }
and default_message_infos () =
  {
    Message_infos.nb_workers = default_uint32 ();
    Message_infos.analyses = [];
    Message_infos.solvers = [];
  }
and default_message_decode_instr () =
  {
    Message_decode_instr.instrs = [];
    Message_decode_instr.kind = parse_message_decode_instr_bin_kind (Piqirun.parse_default "\b\002");
    Message_decode_instr.irkind = Common.parse_ir_kind_t (Piqirun.parse_default "\b\001");
  }
and default_message_decode_instr_instr_entry () =
  {
    Message_decode_instr_instr_entry.instr = default_string ();
    Message_decode_instr_instr_entry.base_addr = parse_uint64 (Piqirun.parse_default "\b\000");
  }
and default_message_decode_instr_bin_kind () = `hexa
and default_message_decode_instr_reply () =
  {
    Message_decode_instr_reply.instrs = [];
  }
and default_message_decode_instr_reply_instr_entry () =
  {
    Message_decode_instr_reply_instr_entry.opcode = default_string ();
    Message_decode_instr_reply_instr_entry.irkind = Common.default_ir_kind_t ();
    Message_decode_instr_reply_instr_entry.dba_instrs = None;
  }
and default_message_start_symb_exec () =
  {
    Message_start_symb_exec.trace_type = default_message_start_symb_exec_trace_kind ();
    Message_start_symb_exec.name = default_string ();
    Message_start_symb_exec.solver = Common.parse_solver_t (Piqirun.parse_default "\b\001");
    Message_start_symb_exec.config = None;
    Message_start_symb_exec.irkind = Common.parse_ir_kind_t (Piqirun.parse_default "\b\001");
    Message_start_symb_exec.formula_optim_cstfold = None;
    Message_start_symb_exec.formula_optim_rebase = None;
    Message_start_symb_exec.formula_optim_row = None;
    Message_start_symb_exec.verbose = default_uint32 ();
    Message_start_symb_exec.addr_predicate = None;
    Message_start_symb_exec.predicate = None;
    Message_start_symb_exec.trace_filename = None;
  }
and default_message_start_symb_exec_trace_kind () = `file
and default_message_output () =
  {
    Message_output.type_ = default_message_output_output_kind ();
    Message_output.message = default_string ();
    Message_output.ram_total = None;
    Message_output.ram_free = None;
    Message_output.ram_available = None;
  }
and default_message_output_output_kind () = `success


let piqi = "\226\202\2304\007message\226\231\249\238\001\017piqi/message.piqi\170\150\212\160\004\011\226\202\2304\006common\170\150\212\160\004\b\226\202\2304\003dba\170\150\212\160\004\011\226\202\2304\006config\218\244\134\182\012\247\003\138\233\142\251\014\240\003\210\203\242$D\232\146\150q\002\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\005start\208\215\133\174\005\000\210\171\158\194\006\006uint64\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$C\232\146\150q\004\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\004stop\208\215\133\174\005\000\210\171\158\194\006\006uint64\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$X\232\146\150q\006\152\247\223\136\002\000\152\182\154\152\004\160\223\186\243\001\232\243\204\157\004\000\218\164\238\191\004\011size-chunks\208\215\133\174\005\000\210\171\158\194\006\006uint32\192\139\160\247\t\000\138\140\251\240\r\b\218\148\211\024\003\b\232\007\136\158\147\199\014\000\210\203\242$J\232\146\150q\b\152\247\223\136\002\000\152\182\154\152\004\250\248\214\130\001\232\243\204\157\004\000\218\164\238\191\004\011breakpoints\208\215\133\174\005\000\210\171\158\194\006\006uint64\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$M\232\146\150q\n\152\247\223\136\002\000\152\182\154\152\004\250\248\214\130\001\232\243\204\157\004\000\218\164\238\191\004\006inputs\208\215\133\174\005\000\210\171\158\194\006\014config/input-t\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$D\232\146\150q\012\152\247\223\136\002\000\152\182\154\152\004\250\248\214\130\001\232\243\204\157\004\000\218\164\238\191\004\005skips\208\215\133\174\005\000\210\171\158\194\006\006uint64\192\139\160\247\t\000\136\158\147\199\014\000\218\164\238\191\004\018message-start-exec\218\244\134\182\012f\138\233\142\251\014`\210\203\242$C\232\146\150q\002\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\004addr\208\215\133\174\005\000\210\171\158\194\006\006uint64\192\139\160\247\t\000\136\158\147\199\014\000\218\164\238\191\004\018message-bp-reached\218\244\134\182\012j\138\233\142\251\014d\210\203\242$L\232\146\150q\002\152\247\223\136\002\000\152\182\154\152\004\250\248\214\130\001\232\243\204\157\004\000\218\164\238\191\004\005patch\208\215\133\174\005\000\210\171\158\194\006\014config/input-t\192\139\160\247\t\000\136\158\147\199\014\000\218\164\238\191\004\rmessage-patch\218\244\134\182\012\255\001\138\233\142\251\014\248\001\210\203\242$I\232\146\150q\002\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\nnb-workers\208\215\133\174\005\000\210\171\158\194\006\006uint32\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$G\232\146\150q\004\152\247\223\136\002\000\152\182\154\152\004\250\248\214\130\001\232\243\204\157\004\000\218\164\238\191\004\banalyses\208\215\133\174\005\000\210\171\158\194\006\006string\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$F\232\146\150q\006\152\247\223\136\002\000\152\182\154\152\004\250\248\214\130\001\232\243\204\157\004\000\218\164\238\191\004\007solvers\208\215\133\174\005\000\210\171\158\194\006\006string\192\139\160\247\t\000\136\158\147\199\014\000\218\164\238\191\004\rmessage-infos\218\244\134\182\012\210\002\138\233\142\251\014\203\002\210\203\242$_\232\146\150q\002\152\247\223\136\002\000\152\182\154\152\004\250\248\214\130\001\232\243\204\157\004\000\218\164\238\191\004\006instrs\208\215\133\174\005\000\210\171\158\194\006 message-decode-instr-instr-entry\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$g\232\146\150q\004\152\247\223\136\002\000\152\182\154\152\004\160\223\186\243\001\232\243\204\157\004\000\218\164\238\191\004\004kind\208\215\133\174\005\000\210\171\158\194\006\029message-decode-instr-bin-kind\192\139\160\247\t\000\138\140\251\240\r\007\218\148\211\024\002\b\002\136\158\147\199\014\000\210\203\242$\\\232\146\150q\006\152\247\223\136\002\000\152\182\154\152\004\160\223\186\243\001\232\243\204\157\004\000\218\164\238\191\004\006irkind\208\215\133\174\005\000\210\171\158\194\006\016common/ir-kind-t\192\139\160\247\t\000\138\140\251\240\r\007\218\148\211\024\002\b\001\136\158\147\199\014\000\218\164\238\191\004\020message-decode-instr\218\244\134\182\012\208\001\138\233\142\251\014\201\001\210\203\242$D\232\146\150q\002\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\005instr\208\215\133\174\005\000\210\171\158\194\006\006string\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$U\232\146\150q\004\152\247\223\136\002\000\152\182\154\152\004\160\223\186\243\001\232\243\204\157\004\000\218\164\238\191\004\tbase-addr\208\215\133\174\005\000\210\171\158\194\006\006uint64\192\139\160\247\t\000\138\140\251\240\r\007\218\148\211\024\002\b\000\136\158\147\199\014\000\218\164\238\191\004 message-decode-instr-instr-entry\218\244\134\182\012\156\001\138\176\205\197\001\149\001\218\164\238\191\004\029message-decode-instr-bin-kind\170\183\218\222\0054\232\146\150q\002\152\247\223\136\002\000\234\188\204\215\002\025message_decode_instr_HEXA\218\164\238\191\004\004HEXA\170\183\218\222\0052\232\146\150q\004\152\247\223\136\002\000\234\188\204\215\002\024message_decode_instr_BIN\218\164\238\191\004\003BIN\218\244\134\182\012\145\001\138\233\142\251\014\138\001\210\203\242$e\232\146\150q\002\152\247\223\136\002\000\152\182\154\152\004\250\248\214\130\001\232\243\204\157\004\000\218\164\238\191\004\006instrs\208\215\133\174\005\000\210\171\158\194\006&message-decode-instr-reply-instr-entry\192\139\160\247\t\000\136\158\147\199\014\000\218\164\238\191\004\026message-decode-instr-reply\218\244\134\182\012\165\002\138\233\142\251\014\158\002\210\203\242$E\232\146\150q\002\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\006opcode\208\215\133\174\005\000\210\171\158\194\006\006string\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$O\232\146\150q\004\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\006irkind\208\215\133\174\005\000\210\171\158\194\006\016common/ir-kind-t\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$O\232\146\150q\006\152\247\223\136\002\000\152\182\154\152\004\160\223\186\243\001\232\243\204\157\004\000\218\164\238\191\004\ndba-instrs\208\215\133\174\005\000\210\171\158\194\006\012dba/dba-list\192\139\160\247\t\000\136\158\147\199\014\000\218\164\238\191\004&message-decode-instr-reply-instr-entry\218\244\134\182\012\210\b\138\233\142\251\014\203\b\210\203\242$e\232\146\150q\002\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\ntrace-type\208\215\133\174\005\000\210\171\158\194\006\"message-start-symb-exec-trace-kind\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$C\232\146\150q\004\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\004name\208\215\133\174\005\000\210\171\158\194\006\006string\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$[\232\146\150q\006\152\247\223\136\002\000\152\182\154\152\004\160\223\186\243\001\232\243\204\157\004\000\218\164\238\191\004\006solver\208\215\133\174\005\000\210\171\158\194\006\015common/solver-t\192\139\160\247\t\000\138\140\251\240\r\007\218\148\211\024\002\b\001\136\158\147\199\014\000\210\203\242$S\232\146\150q\b\152\247\223\136\002\000\152\182\154\152\004\160\223\186\243\001\232\243\204\157\004\000\218\164\238\191\004\006config\208\215\133\174\005\000\210\171\158\194\006\020config/configuration\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$\\\232\146\150q\n\152\247\223\136\002\000\152\182\154\152\004\160\223\186\243\001\232\243\204\157\004\000\218\164\238\191\004\006irkind\208\215\133\174\005\000\210\171\158\194\006\016common/ir-kind-t\192\139\160\247\t\000\138\140\251\240\r\007\218\148\211\024\002\b\001\136\158\147\199\014\000\210\203\242$_\232\146\150q\012\152\247\223\136\002\000\170\131\252\172\003\007\218\148\211\024\002\b\001\152\182\154\152\004\160\223\186\243\001\232\243\204\157\004\000\218\164\238\191\004\021formula-optim-cstfold\208\215\133\174\005\000\210\171\158\194\006\004bool\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$^\232\146\150q\014\152\247\223\136\002\000\170\131\252\172\003\007\218\148\211\024\002\b\001\152\182\154\152\004\160\223\186\243\001\232\243\204\157\004\000\218\164\238\191\004\020formula-optim-rebase\208\215\133\174\005\000\210\171\158\194\006\004bool\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$[\232\146\150q\016\152\247\223\136\002\000\170\131\252\172\003\007\218\148\211\024\002\b\001\152\182\154\152\004\160\223\186\243\001\232\243\204\157\004\000\218\164\238\191\004\017formula-optim-row\208\215\133\174\005\000\210\171\158\194\006\004bool\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$F\232\146\150q\018\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\007verbose\208\215\133\174\005\000\210\171\158\194\006\006uint32\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$M\232\146\150q\020\152\247\223\136\002\000\152\182\154\152\004\160\223\186\243\001\232\243\204\157\004\000\218\164\238\191\004\014addr-predicate\208\215\133\174\005\000\210\171\158\194\006\006uint64\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$H\232\146\150q\022\152\247\223\136\002\000\152\182\154\152\004\160\223\186\243\001\232\243\204\157\004\000\218\164\238\191\004\tpredicate\208\215\133\174\005\000\210\171\158\194\006\006string\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$M\232\146\150q\024\152\247\223\136\002\000\152\182\154\152\004\160\223\186\243\001\232\243\204\157\004\000\218\164\238\191\004\014trace-filename\208\215\133\174\005\000\210\171\158\194\006\006string\192\139\160\247\t\000\136\158\147\199\014\000\218\164\238\191\004\023message-start-symb-exec\218\244\134\182\012\173\001\138\176\205\197\001\166\001\218\164\238\191\004\"message-start-symb-exec-trace-kind\170\183\218\222\0057\232\146\150q\002\152\247\223\136\002\000\234\188\204\215\002\028message_start_symb_exec_FILE\218\164\238\191\004\004FILE\170\183\218\222\005;\232\146\150q\004\152\247\223\136\002\000\234\188\204\215\002\030message_start_symb_exec_STREAM\218\164\238\191\004\006STREAM\218\244\134\182\012\172\003\138\233\142\251\014\165\003\210\203\242$W\232\146\150q\002\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\004type\208\215\133\174\005\000\210\171\158\194\006\026message-output-output-kind\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$F\232\146\150q\004\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\007message\208\215\133\174\005\000\210\171\158\194\006\006string\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$H\232\146\150q\006\152\247\223\136\002\000\152\182\154\152\004\160\223\186\243\001\232\243\204\157\004\000\218\164\238\191\004\tram-total\208\215\133\174\005\000\210\171\158\194\006\006uint32\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$G\232\146\150q\b\152\247\223\136\002\000\152\182\154\152\004\160\223\186\243\001\232\243\204\157\004\000\218\164\238\191\004\bram-free\208\215\133\174\005\000\210\171\158\194\006\006uint32\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$L\232\146\150q\n\152\247\223\136\002\000\152\182\154\152\004\160\223\186\243\001\232\243\204\157\004\000\218\164\238\191\004\rram-available\208\215\133\174\005\000\210\171\158\194\006\006uint32\192\139\160\247\t\000\136\158\147\199\014\000\218\164\238\191\004\014message-output\218\244\134\182\012\249\002\138\176\205\197\001\242\002\218\164\238\191\004\026message-output-output-kind\170\183\218\222\0054\232\146\150q\002\152\247\223\136\002\000\234\188\204\215\002\022message_output_SUCCESS\218\164\238\191\004\007SUCCESS\170\183\218\222\0052\232\146\150q\004\152\247\223\136\002\000\234\188\204\215\002\021message_output_RESULT\218\164\238\191\004\006RESULT\170\183\218\222\0050\232\146\150q\006\152\247\223\136\002\000\234\188\204\215\002\020message_output_DEBUG\218\164\238\191\004\005DEBUG\170\183\218\222\0054\232\146\150q\b\152\247\223\136\002\000\234\188\204\215\002\022message_output_WARNING\218\164\238\191\004\007WARNING\170\183\218\222\0050\232\146\150q\n\152\247\223\136\002\000\234\188\204\215\002\020message_output_ERROR\218\164\238\191\004\005ERROR\170\183\218\222\0054\232\146\150q\012\152\247\223\136\002\000\234\188\204\215\002\022message_output_FAILURE\218\164\238\191\004\007FAILURE"
include Message_piqi
