module  Common = Common_piqi

module  Libcall = Libcall_piqi

module  Syscall = Syscall_piqi

module  Instruction = Instruction_piqi

module  Analysis_config = Analysis_config_piqi

module rec Config_piqi:
  sig
    type uint64 = int64
    type uint32 = int32
    type input_t_input_kind =
      [
        | `reg
        | `mem
        | `indirect
      ]
    type input_t_when_t =
      [
        | `before
        | `after
      ]
    type call_name_t = Call_name_t.t
    type configuration = Configuration.t
    type input_t = Input_t.t
  end = Config_piqi
and Call_name_t:
  sig
    type t = {
      mutable address: Config_piqi.uint64;
      mutable name: string;
    }
  end = Call_name_t
and Configuration:
  sig
    type t = {
      mutable start: Config_piqi.uint64 option;
      mutable stop: Config_piqi.uint64 option;
      mutable call_skips: Config_piqi.uint64 list;
      mutable fun_skips: Config_piqi.uint64 list;
      mutable libcalls: Libcall.libcall_pol list;
      mutable syscalls: Syscall.syscall_pol list;
      mutable instrs: Instruction.instr_pol list;
      mutable policy: string list;
      mutable inputs: Config_piqi.input_t list;
      mutable call_map: Config_piqi.call_name_t list;
      mutable breakpoints: Config_piqi.uint64 list;
      mutable initial_state: Common.memory_t list;
      mutable direction: Common.analysis_direction_t;
      mutable ksteps: Config_piqi.uint32;
      mutable analysis_name: string;
      mutable solver: Common.solver_t;
      mutable incremental: bool;
      mutable timeout: Config_piqi.uint32;
      mutable optim_cstprop: bool;
      mutable optim_rebase: bool;
      mutable optim_row: bool;
      mutable optim_rowplus: bool;
      mutable optim_eqprop: bool;
      mutable callcvt: Common.call_convention_t;
      mutable default_action: Common.action;
      mutable verbosity: Config_piqi.uint32;
      mutable additional_parameters: Analysis_config.specific_parameters_t option;
    }
  end = Configuration
and Input_t:
  sig
    type t = {
      mutable typeid: Config_piqi.input_t_input_kind;
      mutable address: Config_piqi.uint64;
      mutable when_: Config_piqi.input_t_when_t;
      mutable action: Common.action;
      mutable iteration: Config_piqi.uint32;
      mutable reg: Common.register_t option;
      mutable mem: Common.memory_t option;
      mutable indirect: Common.indirect_register_t option;
    }
  end = Input_t


let rec parse_uint64 x = Piqirun.int64_of_varint x
and packed_parse_uint64 x = Piqirun.int64_of_packed_varint x

and parse_string x = Piqirun.string_of_block x

and parse_uint32 x = Piqirun.int32_of_varint x
and packed_parse_uint32 x = Piqirun.int32_of_packed_varint x

and parse_bool x = Piqirun.bool_of_varint x
and packed_parse_bool x = Piqirun.bool_of_packed_varint x

and parse_call_name_t x =
  let x = Piqirun.parse_record x in
  let _address, x = Piqirun.parse_required_field 1 parse_uint64 x in
  let _name, x = Piqirun.parse_required_field 2 parse_string x in
  Piqirun.check_unparsed_fields x;
  {
    Call_name_t.address = _address;
    Call_name_t.name = _name;
  }

and parse_configuration x =
  let x = Piqirun.parse_record x in
  let _start, x = Piqirun.parse_optional_field 1 parse_uint64 x in
  let _stop, x = Piqirun.parse_optional_field 2 parse_uint64 x in
  let _call_skips, x = Piqirun.parse_repeated_field 3 parse_uint64 x in
  let _fun_skips, x = Piqirun.parse_repeated_field 4 parse_uint64 x in
  let _libcalls, x = Piqirun.parse_repeated_field 5 Libcall.parse_libcall_pol x in
  let _syscalls, x = Piqirun.parse_repeated_field 6 Syscall.parse_syscall_pol x in
  let _instrs, x = Piqirun.parse_repeated_field 7 Instruction.parse_instr_pol x in
  let _policy, x = Piqirun.parse_repeated_field 8 parse_string x in
  let _inputs, x = Piqirun.parse_repeated_field 9 parse_input_t x in
  let _call_map, x = Piqirun.parse_repeated_field 10 parse_call_name_t x in
  let _breakpoints, x = Piqirun.parse_repeated_field 11 parse_uint64 x in
  let _initial_state, x = Piqirun.parse_repeated_field 12 Common.parse_memory_t x in
  let _direction, x = Piqirun.parse_required_field 13 Common.parse_analysis_direction_t x ~default:"\b\001" in
  let _ksteps, x = Piqirun.parse_required_field 14 parse_uint32 x ~default:"\b\000" in
  let _analysis_name, x = Piqirun.parse_required_field 15 parse_string x ~default:"\n\000" in
  let _solver, x = Piqirun.parse_required_field 16 Common.parse_solver_t x ~default:"\b\001" in
  let _incremental, x = Piqirun.parse_required_field 17 parse_bool x ~default:"\b\000" in
  let _timeout, x = Piqirun.parse_required_field 18 parse_uint32 x ~default:"\b\000" in
  let _optim_cstprop, x = Piqirun.parse_required_field 19 parse_bool x ~default:"\b\000" in
  let _optim_rebase, x = Piqirun.parse_required_field 20 parse_bool x ~default:"\b\000" in
  let _optim_row, x = Piqirun.parse_required_field 21 parse_bool x ~default:"\b\000" in
  let _callcvt, x = Piqirun.parse_required_field 22 Common.parse_call_convention_t x ~default:"\b\001" in
  let _default_action, x = Piqirun.parse_required_field 23 Common.parse_action x ~default:"\b\003" in
  let _verbosity, x = Piqirun.parse_required_field 24 parse_uint32 x ~default:"\b\000" in
  let _additional_parameters, x = Piqirun.parse_optional_field 25 Analysis_config.parse_specific_parameters_t x in
  let _optim_rowplus, x = Piqirun.parse_required_field 26 parse_bool x ~default:"\b\000" in
  let _optim_eqprop, x = Piqirun.parse_required_field 27 parse_bool x ~default:"\b\000" in
  Piqirun.check_unparsed_fields x;
  {
    Configuration.start = _start;
    Configuration.stop = _stop;
    Configuration.call_skips = _call_skips;
    Configuration.fun_skips = _fun_skips;
    Configuration.libcalls = _libcalls;
    Configuration.syscalls = _syscalls;
    Configuration.instrs = _instrs;
    Configuration.policy = _policy;
    Configuration.inputs = _inputs;
    Configuration.call_map = _call_map;
    Configuration.breakpoints = _breakpoints;
    Configuration.initial_state = _initial_state;
    Configuration.direction = _direction;
    Configuration.ksteps = _ksteps;
    Configuration.analysis_name = _analysis_name;
    Configuration.solver = _solver;
    Configuration.incremental = _incremental;
    Configuration.timeout = _timeout;
    Configuration.optim_cstprop = _optim_cstprop;
    Configuration.optim_rebase = _optim_rebase;
    Configuration.optim_row = _optim_row;
    Configuration.callcvt = _callcvt;
    Configuration.default_action = _default_action;
    Configuration.verbosity = _verbosity;
    Configuration.additional_parameters = _additional_parameters;
    Configuration.optim_rowplus = _optim_rowplus;
    Configuration.optim_eqprop = _optim_eqprop;
  }

and parse_input_t x =
  let x = Piqirun.parse_record x in
  let _typeid, x = Piqirun.parse_required_field 1 parse_input_t_input_kind x in
  let _address, x = Piqirun.parse_required_field 2 parse_uint64 x in
  let _when_, x = Piqirun.parse_required_field 3 parse_input_t_when_t x in
  let _action, x = Piqirun.parse_required_field 4 Common.parse_action x in
  let _reg, x = Piqirun.parse_optional_field 7 Common.parse_register_t x in
  let _mem, x = Piqirun.parse_optional_field 8 Common.parse_memory_t x in
  let _indirect, x = Piqirun.parse_optional_field 9 Common.parse_indirect_register_t x in
  let _iteration, x = Piqirun.parse_required_field 100 parse_uint32 x ~default:"\b\000" in
  Piqirun.check_unparsed_fields x;
  {
    Input_t.typeid = _typeid;
    Input_t.address = _address;
    Input_t.when_ = _when_;
    Input_t.action = _action;
    Input_t.reg = _reg;
    Input_t.mem = _mem;
    Input_t.indirect = _indirect;
    Input_t.iteration = _iteration;
  }

and parse_input_t_input_kind x =
  match Piqirun.int32_of_signed_varint x with
    | 0l -> `reg
    | 1l -> `mem
    | 2l -> `indirect
    | x -> Piqirun.error_enum_const x
and packed_parse_input_t_input_kind x =
  match Piqirun.int32_of_packed_signed_varint x with
    | 0l -> `reg
    | 1l -> `mem
    | 2l -> `indirect
    | x -> Piqirun.error_enum_const x

and parse_input_t_when_t x =
  match Piqirun.int32_of_signed_varint x with
    | 0l -> `before
    | 1l -> `after
    | x -> Piqirun.error_enum_const x
and packed_parse_input_t_when_t x =
  match Piqirun.int32_of_packed_signed_varint x with
    | 0l -> `before
    | 1l -> `after
    | x -> Piqirun.error_enum_const x


let rec gen__uint64 code x = Piqirun.int64_to_varint code x
and packed_gen__uint64 x = Piqirun.int64_to_packed_varint x

and gen__string code x = Piqirun.string_to_block code x

and gen__uint32 code x = Piqirun.int32_to_varint code x
and packed_gen__uint32 x = Piqirun.int32_to_packed_varint x

and gen__bool code x = Piqirun.bool_to_varint code x
and packed_gen__bool x = Piqirun.bool_to_packed_varint x

and gen__call_name_t code x =
  let _address = Piqirun.gen_required_field 1 gen__uint64 x.Call_name_t.address in
  let _name = Piqirun.gen_required_field 2 gen__string x.Call_name_t.name in
  Piqirun.gen_record code (_address :: _name :: [])

and gen__configuration code x =
  let _start = Piqirun.gen_optional_field 1 gen__uint64 x.Configuration.start in
  let _stop = Piqirun.gen_optional_field 2 gen__uint64 x.Configuration.stop in
  let _call_skips = Piqirun.gen_repeated_field 3 gen__uint64 x.Configuration.call_skips in
  let _fun_skips = Piqirun.gen_repeated_field 4 gen__uint64 x.Configuration.fun_skips in
  let _libcalls = Piqirun.gen_repeated_field 5 Libcall.gen__libcall_pol x.Configuration.libcalls in
  let _syscalls = Piqirun.gen_repeated_field 6 Syscall.gen__syscall_pol x.Configuration.syscalls in
  let _instrs = Piqirun.gen_repeated_field 7 Instruction.gen__instr_pol x.Configuration.instrs in
  let _policy = Piqirun.gen_repeated_field 8 gen__string x.Configuration.policy in
  let _inputs = Piqirun.gen_repeated_field 9 gen__input_t x.Configuration.inputs in
  let _call_map = Piqirun.gen_repeated_field 10 gen__call_name_t x.Configuration.call_map in
  let _breakpoints = Piqirun.gen_repeated_field 11 gen__uint64 x.Configuration.breakpoints in
  let _initial_state = Piqirun.gen_repeated_field 12 Common.gen__memory_t x.Configuration.initial_state in
  let _direction = Piqirun.gen_required_field 13 Common.gen__analysis_direction_t x.Configuration.direction in
  let _ksteps = Piqirun.gen_required_field 14 gen__uint32 x.Configuration.ksteps in
  let _analysis_name = Piqirun.gen_required_field 15 gen__string x.Configuration.analysis_name in
  let _solver = Piqirun.gen_required_field 16 Common.gen__solver_t x.Configuration.solver in
  let _incremental = Piqirun.gen_required_field 17 gen__bool x.Configuration.incremental in
  let _timeout = Piqirun.gen_required_field 18 gen__uint32 x.Configuration.timeout in
  let _optim_cstprop = Piqirun.gen_required_field 19 gen__bool x.Configuration.optim_cstprop in
  let _optim_rebase = Piqirun.gen_required_field 20 gen__bool x.Configuration.optim_rebase in
  let _optim_row = Piqirun.gen_required_field 21 gen__bool x.Configuration.optim_row in
  let _callcvt = Piqirun.gen_required_field 22 Common.gen__call_convention_t x.Configuration.callcvt in
  let _default_action = Piqirun.gen_required_field 23 Common.gen__action x.Configuration.default_action in
  let _verbosity = Piqirun.gen_required_field 24 gen__uint32 x.Configuration.verbosity in
  let _additional_parameters = Piqirun.gen_optional_field 25 Analysis_config.gen__specific_parameters_t x.Configuration.additional_parameters in
  let _optim_rowplus = Piqirun.gen_required_field 26 gen__bool x.Configuration.optim_rowplus in
  let _optim_eqprop = Piqirun.gen_required_field 27 gen__bool x.Configuration.optim_eqprop in
  Piqirun.gen_record code (_start :: _stop :: _call_skips :: _fun_skips :: _libcalls :: _syscalls :: _instrs :: _policy :: _inputs :: _call_map :: _breakpoints :: _initial_state :: _direction :: _ksteps :: _analysis_name :: _solver :: _incremental :: _timeout :: _optim_cstprop :: _optim_rebase :: _optim_row :: _callcvt :: _default_action :: _verbosity :: _additional_parameters :: _optim_rowplus :: _optim_eqprop :: [])

and gen__input_t code x =
  let _typeid = Piqirun.gen_required_field 1 gen__input_t_input_kind x.Input_t.typeid in
  let _address = Piqirun.gen_required_field 2 gen__uint64 x.Input_t.address in
  let _when_ = Piqirun.gen_required_field 3 gen__input_t_when_t x.Input_t.when_ in
  let _action = Piqirun.gen_required_field 4 Common.gen__action x.Input_t.action in
  let _reg = Piqirun.gen_optional_field 7 Common.gen__register_t x.Input_t.reg in
  let _mem = Piqirun.gen_optional_field 8 Common.gen__memory_t x.Input_t.mem in
  let _indirect = Piqirun.gen_optional_field 9 Common.gen__indirect_register_t x.Input_t.indirect in
  let _iteration = Piqirun.gen_required_field 100 gen__uint32 x.Input_t.iteration in
  Piqirun.gen_record code (_typeid :: _address :: _when_ :: _action :: _reg :: _mem :: _indirect :: _iteration :: [])

and gen__input_t_input_kind code x =
  Piqirun.int32_to_signed_varint code (match x with
    | `reg -> 0l
    | `mem -> 1l
    | `indirect -> 2l
  )
and packed_gen__input_t_input_kind x =
  Piqirun.int32_to_packed_signed_varint (match x with
    | `reg -> 0l
    | `mem -> 1l
    | `indirect -> 2l
  )

and gen__input_t_when_t code x =
  Piqirun.int32_to_signed_varint code (match x with
    | `before -> 0l
    | `after -> 1l
  )
and packed_gen__input_t_when_t x =
  Piqirun.int32_to_packed_signed_varint (match x with
    | `before -> 0l
    | `after -> 1l
  )


let gen_uint64 x = gen__uint64 (-1) x
let gen_string x = gen__string (-1) x
let gen_uint32 x = gen__uint32 (-1) x
let gen_bool x = gen__bool (-1) x
let gen_call_name_t x = gen__call_name_t (-1) x
let gen_configuration x = gen__configuration (-1) x
let gen_input_t x = gen__input_t (-1) x
let gen_input_t_input_kind x = gen__input_t_input_kind (-1) x
let gen_input_t_when_t x = gen__input_t_when_t (-1) x


let rec default_uint64 () = 0L
and default_string () = ""
and default_uint32 () = 0l
and default_bool () = false
and default_call_name_t () =
  {
    Call_name_t.address = default_uint64 ();
    Call_name_t.name = default_string ();
  }
and default_configuration () =
  {
    Configuration.start = None;
    Configuration.stop = None;
    Configuration.call_skips = [];
    Configuration.fun_skips = [];
    Configuration.libcalls = [];
    Configuration.syscalls = [];
    Configuration.instrs = [];
    Configuration.policy = [];
    Configuration.inputs = [];
    Configuration.call_map = [];
    Configuration.breakpoints = [];
    Configuration.initial_state = [];
    Configuration.direction = Common.parse_analysis_direction_t (Piqirun.parse_default "\b\001");
    Configuration.ksteps = parse_uint32 (Piqirun.parse_default "\b\000");
    Configuration.analysis_name = parse_string (Piqirun.parse_default "\n\000");
    Configuration.solver = Common.parse_solver_t (Piqirun.parse_default "\b\001");
    Configuration.incremental = parse_bool (Piqirun.parse_default "\b\000");
    Configuration.timeout = parse_uint32 (Piqirun.parse_default "\b\000");
    Configuration.optim_cstprop = parse_bool (Piqirun.parse_default "\b\000");
    Configuration.optim_rebase = parse_bool (Piqirun.parse_default "\b\000");
    Configuration.optim_row = parse_bool (Piqirun.parse_default "\b\000");
    Configuration.callcvt = Common.parse_call_convention_t (Piqirun.parse_default "\b\001");
    Configuration.default_action = Common.parse_action (Piqirun.parse_default "\b\003");
    Configuration.verbosity = parse_uint32 (Piqirun.parse_default "\b\000");
    Configuration.additional_parameters = None;
    Configuration.optim_rowplus = parse_bool (Piqirun.parse_default "\b\000");
    Configuration.optim_eqprop = parse_bool (Piqirun.parse_default "\b\000");
  }
and default_input_t () =
  {
    Input_t.typeid = default_input_t_input_kind ();
    Input_t.address = default_uint64 ();
    Input_t.when_ = default_input_t_when_t ();
    Input_t.action = Common.default_action ();
    Input_t.reg = None;
    Input_t.mem = None;
    Input_t.indirect = None;
    Input_t.iteration = parse_uint32 (Piqirun.parse_default "\b\000");
  }
and default_input_t_input_kind () = `reg
and default_input_t_when_t () = `before


let piqi = "\226\202\2304\006config\226\231\249\238\001\016piqi/config.piqi\170\150\212\160\004\011\226\202\2304\006common\170\150\212\160\004\012\226\202\2304\007libcall\170\150\212\160\004\012\226\202\2304\007syscall\170\150\212\160\004\016\226\202\2304\011instruction\170\150\212\160\004)\226\202\2304\015analysis_config\218\164\238\191\004\015analysis-config\162\244\146\155\011\rconfiguration\218\244\134\182\012\171\001\138\233\142\251\014\164\001\210\203\242$F\232\146\150q\002\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\007address\208\215\133\174\005\000\210\171\158\194\006\006uint64\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$C\232\146\150q\004\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\004name\208\215\133\174\005\000\210\171\158\194\006\006string\192\139\160\247\t\000\136\158\147\199\014\000\218\164\238\191\004\011call-name-t\218\244\134\182\012\204\019\138\233\142\251\014\197\019\210\203\242$D\232\146\150q\002\152\247\223\136\002\000\152\182\154\152\004\160\223\186\243\001\232\243\204\157\004\000\218\164\238\191\004\005start\208\215\133\174\005\000\210\171\158\194\006\006uint64\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$C\232\146\150q\004\152\247\223\136\002\000\152\182\154\152\004\160\223\186\243\001\232\243\204\157\004\000\218\164\238\191\004\004stop\208\215\133\174\005\000\210\171\158\194\006\006uint64\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$I\232\146\150q\006\152\247\223\136\002\000\152\182\154\152\004\250\248\214\130\001\232\243\204\157\004\000\218\164\238\191\004\ncall-skips\208\215\133\174\005\000\210\171\158\194\006\006uint64\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$H\232\146\150q\b\152\247\223\136\002\000\152\182\154\152\004\250\248\214\130\001\232\243\204\157\004\000\218\164\238\191\004\tfun-skips\208\215\133\174\005\000\210\171\158\194\006\006uint64\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$T\232\146\150q\n\152\247\223\136\002\000\152\182\154\152\004\250\248\214\130\001\232\243\204\157\004\000\218\164\238\191\004\blibcalls\208\215\133\174\005\000\210\171\158\194\006\019libcall/libcall-pol\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$T\232\146\150q\012\152\247\223\136\002\000\152\182\154\152\004\250\248\214\130\001\232\243\204\157\004\000\218\164\238\191\004\bsyscalls\208\215\133\174\005\000\210\171\158\194\006\019syscall/syscall-pol\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$T\232\146\150q\014\152\247\223\136\002\000\152\182\154\152\004\250\248\214\130\001\232\243\204\157\004\000\218\164\238\191\004\006instrs\208\215\133\174\005\000\210\171\158\194\006\021instruction/instr-pol\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$E\232\146\150q\016\152\247\223\136\002\000\152\182\154\152\004\250\248\214\130\001\232\243\204\157\004\000\218\164\238\191\004\006policy\208\215\133\174\005\000\210\171\158\194\006\006string\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$F\232\146\150q\018\152\247\223\136\002\000\152\182\154\152\004\250\248\214\130\001\232\243\204\157\004\000\218\164\238\191\004\006inputs\208\215\133\174\005\000\210\171\158\194\006\007input-t\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$L\232\146\150q\020\152\247\223\136\002\000\152\182\154\152\004\250\248\214\130\001\232\243\204\157\004\000\218\164\238\191\004\bcall-map\208\215\133\174\005\000\210\171\158\194\006\011call-name-t\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$J\232\146\150q\022\152\247\223\136\002\000\152\182\154\152\004\250\248\214\130\001\232\243\204\157\004\000\218\164\238\191\004\011breakpoints\208\215\133\174\005\000\210\171\158\194\006\006uint64\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$U\232\146\150q\024\152\247\223\136\002\000\152\182\154\152\004\250\248\214\130\001\232\243\204\157\004\000\218\164\238\191\004\rinitial-state\208\215\133\174\005\000\210\171\158\194\006\015common/memory-t\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$j\232\146\150q\026\152\247\223\136\002\000\152\182\154\152\004\160\223\186\243\001\232\243\204\157\004\000\218\164\238\191\004\tdirection\208\215\133\174\005\000\210\171\158\194\006\027common/analysis-direction-t\192\139\160\247\t\000\138\140\251\240\r\007\218\148\211\024\002\b\001\136\158\147\199\014\000\210\203\242$R\232\146\150q\028\152\247\223\136\002\000\152\182\154\152\004\160\223\186\243\001\232\243\204\157\004\000\218\164\238\191\004\006ksteps\208\215\133\174\005\000\210\171\158\194\006\006uint32\192\139\160\247\t\000\138\140\251\240\r\007\218\148\211\024\002\b\000\136\158\147\199\014\000\210\203\242$Y\232\146\150q\030\152\247\223\136\002\000\152\182\154\152\004\160\223\186\243\001\232\243\204\157\004\000\218\164\238\191\004\ranalysis-name\208\215\133\174\005\000\210\171\158\194\006\006string\192\139\160\247\t\000\138\140\251\240\r\007\218\148\211\024\002\n\000\136\158\147\199\014\000\210\203\242$[\232\146\150q \152\247\223\136\002\000\152\182\154\152\004\160\223\186\243\001\232\243\204\157\004\000\218\164\238\191\004\006solver\208\215\133\174\005\000\210\171\158\194\006\015common/solver-t\192\139\160\247\t\000\138\140\251\240\r\007\218\148\211\024\002\b\001\136\158\147\199\014\000\210\203\242$b\232\146\150q\"\152\247\223\136\002\000\170\131\252\172\003\007\218\148\211\024\002\b\001\152\182\154\152\004\160\223\186\243\001\232\243\204\157\004\000\218\164\238\191\004\011incremental\208\215\133\174\005\000\210\171\158\194\006\004bool\192\139\160\247\t\000\138\140\251\240\r\007\218\148\211\024\002\b\000\136\158\147\199\014\000\210\203\242$S\232\146\150q$\152\247\223\136\002\000\152\182\154\152\004\160\223\186\243\001\232\243\204\157\004\000\218\164\238\191\004\007timeout\208\215\133\174\005\000\210\171\158\194\006\006uint32\192\139\160\247\t\000\138\140\251\240\r\007\218\148\211\024\002\b\000\136\158\147\199\014\000\210\203\242$d\232\146\150q&\152\247\223\136\002\000\170\131\252\172\003\007\218\148\211\024\002\b\001\152\182\154\152\004\160\223\186\243\001\232\243\204\157\004\000\218\164\238\191\004\roptim-cstprop\208\215\133\174\005\000\210\171\158\194\006\004bool\192\139\160\247\t\000\138\140\251\240\r\007\218\148\211\024\002\b\000\136\158\147\199\014\000\210\203\242$c\232\146\150q(\152\247\223\136\002\000\170\131\252\172\003\007\218\148\211\024\002\b\001\152\182\154\152\004\160\223\186\243\001\232\243\204\157\004\000\218\164\238\191\004\012optim-rebase\208\215\133\174\005\000\210\171\158\194\006\004bool\192\139\160\247\t\000\138\140\251\240\r\007\218\148\211\024\002\b\000\136\158\147\199\014\000\210\203\242$`\232\146\150q*\152\247\223\136\002\000\170\131\252\172\003\007\218\148\211\024\002\b\001\152\182\154\152\004\160\223\186\243\001\232\243\204\157\004\000\218\164\238\191\004\toptim-row\208\215\133\174\005\000\210\171\158\194\006\004bool\192\139\160\247\t\000\138\140\251\240\r\007\218\148\211\024\002\b\000\136\158\147\199\014\000\210\203\242$d\232\146\150q4\152\247\223\136\002\000\170\131\252\172\003\007\218\148\211\024\002\b\001\152\182\154\152\004\160\223\186\243\001\232\243\204\157\004\000\218\164\238\191\004\roptim-rowplus\208\215\133\174\005\000\210\171\158\194\006\004bool\192\139\160\247\t\000\138\140\251\240\r\007\218\148\211\024\002\b\000\136\158\147\199\014\000\210\203\242$c\232\146\150q6\152\247\223\136\002\000\170\131\252\172\003\007\218\148\211\024\002\b\001\152\182\154\152\004\160\223\186\243\001\232\243\204\157\004\000\218\164\238\191\004\012optim-eqprop\208\215\133\174\005\000\210\171\158\194\006\004bool\192\139\160\247\t\000\138\140\251\240\r\007\218\148\211\024\002\b\000\136\158\147\199\014\000\210\203\242$e\232\146\150q,\152\247\223\136\002\000\152\182\154\152\004\160\223\186\243\001\232\243\204\157\004\000\218\164\238\191\004\007callcvt\208\215\133\174\005\000\210\171\158\194\006\024common/call-convention-t\192\139\160\247\t\000\138\140\251\240\r\007\218\148\211\024\002\b\001\136\158\147\199\014\000\210\203\242$a\232\146\150q.\152\247\223\136\002\000\152\182\154\152\004\160\223\186\243\001\232\243\204\157\004\000\218\164\238\191\004\014default-action\208\215\133\174\005\000\210\171\158\194\006\rcommon/action\192\139\160\247\t\000\138\140\251\240\r\007\218\148\211\024\002\b\003\136\158\147\199\014\000\210\203\242$U\232\146\150q0\152\247\223\136\002\000\152\182\154\152\004\160\223\186\243\001\232\243\204\157\004\000\218\164\238\191\004\tverbosity\208\215\133\174\005\000\210\171\158\194\006\006uint32\192\139\160\247\t\000\138\140\251\240\r\007\218\148\211\024\002\b\000\136\158\147\199\014\000\210\203\242$s\232\146\150q2\152\247\223\136\002\000\152\182\154\152\004\160\223\186\243\001\232\243\204\157\004\000\218\164\238\191\004\021additional-parameters\208\215\133\174\005\000\210\171\158\194\006%analysis-config/specific-parameters-t\192\139\160\247\t\000\136\158\147\199\014\000\218\164\238\191\004\rconfiguration\218\244\134\182\012\179\005\138\233\142\251\014\172\005\210\203\242$Q\232\146\150q\002\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\006typeid\208\215\133\174\005\000\210\171\158\194\006\018input-t-input-kind\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$F\232\146\150q\004\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\007address\208\215\133\174\005\000\210\171\158\194\006\006uint64\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$K\232\146\150q\006\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\004when\208\215\133\174\005\000\210\171\158\194\006\014input-t-when-t\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$L\232\146\150q\b\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\006action\208\215\133\174\005\000\210\171\158\194\006\rcommon/action\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$V\232\146\150q\200\001\152\247\223\136\002\000\152\182\154\152\004\160\223\186\243\001\232\243\204\157\004\000\218\164\238\191\004\titeration\208\215\133\174\005\000\210\171\158\194\006\006uint32\192\139\160\247\t\000\138\140\251\240\r\007\218\148\211\024\002\b\000\136\158\147\199\014\000\210\203\242$M\232\146\150q\014\152\247\223\136\002\000\152\182\154\152\004\160\223\186\243\001\232\243\204\157\004\000\218\164\238\191\004\003reg\208\215\133\174\005\000\210\171\158\194\006\017common/register-t\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$K\232\146\150q\016\152\247\223\136\002\000\152\182\154\152\004\160\223\186\243\001\232\243\204\157\004\000\218\164\238\191\004\003mem\208\215\133\174\005\000\210\171\158\194\006\015common/memory-t\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$[\232\146\150q\018\152\247\223\136\002\000\152\182\154\152\004\160\223\186\243\001\232\243\204\157\004\000\218\164\238\191\004\bindirect\208\215\133\174\005\000\210\171\158\194\006\026common/indirect-register-t\192\139\160\247\t\000\136\158\147\199\014\000\218\164\238\191\004\007input-t\218\244\134\182\012\170\001\138\176\205\197\001\163\001\218\164\238\191\004\018input-t-input-kind\170\183\218\222\005%\232\146\150q\000\152\247\223\136\002\000\234\188\204\215\002\011input_t_REG\218\164\238\191\004\003REG\170\183\218\222\005%\232\146\150q\002\152\247\223\136\002\000\234\188\204\215\002\011input_t_MEM\218\164\238\191\004\003MEM\170\183\218\222\005/\232\146\150q\004\152\247\223\136\002\000\234\188\204\215\002\016input_t_INDIRECT\218\164\238\191\004\bINDIRECT\218\244\134\182\012z\138\176\205\197\001t\218\164\238\191\004\014input-t-when-t\170\183\218\222\005+\232\146\150q\000\152\247\223\136\002\000\234\188\204\215\002\014input_t_BEFORE\218\164\238\191\004\006BEFORE\170\183\218\222\005)\232\146\150q\002\152\247\223\136\002\000\234\188\204\215\002\rinput_t_AFTER\218\164\238\191\004\005AFTER"
include Config_piqi
