module  Common = Common_piqi

module rec Instruction_piqi:
  sig
    type instr_ident =
      [
        | `invalid_inst
        | `cpuid
      ]
    type instr_pol = Instr_pol.t
    type cpuid_pol = Cpuid_pol.t
  end = Instruction_piqi
and Instr_pol:
  sig
    type t = {
      mutable ident: Instruction_piqi.instr_ident;
      mutable opcode: string;
      mutable cpuid: Instruction_piqi.cpuid_pol option;
    }
  end = Instr_pol
and Cpuid_pol:
  sig
    type t = {
      mutable eax: Common.action;
      mutable ebx: Common.action;
      mutable ecx: Common.action;
      mutable edx: Common.action;
    }
  end = Cpuid_pol


let rec parse_string x = Piqirun.string_of_block x

and parse_instr_pol x =
  let x = Piqirun.parse_record x in
  let _ident, x = Piqirun.parse_required_field 1 parse_instr_ident x in
  let _opcode, x = Piqirun.parse_required_field 2 parse_string x in
  let _cpuid, x = Piqirun.parse_optional_field 3 parse_cpuid_pol x in
  Piqirun.check_unparsed_fields x;
  {
    Instr_pol.ident = _ident;
    Instr_pol.opcode = _opcode;
    Instr_pol.cpuid = _cpuid;
  }

and parse_cpuid_pol x =
  let x = Piqirun.parse_record x in
  let _eax, x = Piqirun.parse_required_field 1 Common.parse_action x ~default:"\b\000" in
  let _ebx, x = Piqirun.parse_required_field 2 Common.parse_action x ~default:"\b\000" in
  let _ecx, x = Piqirun.parse_required_field 3 Common.parse_action x ~default:"\b\000" in
  let _edx, x = Piqirun.parse_required_field 4 Common.parse_action x ~default:"\b\000" in
  Piqirun.check_unparsed_fields x;
  {
    Cpuid_pol.eax = _eax;
    Cpuid_pol.ebx = _ebx;
    Cpuid_pol.ecx = _ecx;
    Cpuid_pol.edx = _edx;
  }

and parse_instr_ident x =
  match Piqirun.int32_of_signed_varint x with
    | 0l -> `invalid_inst
    | 1l -> `cpuid
    | x -> Piqirun.error_enum_const x
and packed_parse_instr_ident x =
  match Piqirun.int32_of_packed_signed_varint x with
    | 0l -> `invalid_inst
    | 1l -> `cpuid
    | x -> Piqirun.error_enum_const x


let rec gen__string code x = Piqirun.string_to_block code x

and gen__instr_pol code x =
  let _ident = Piqirun.gen_required_field 1 gen__instr_ident x.Instr_pol.ident in
  let _opcode = Piqirun.gen_required_field 2 gen__string x.Instr_pol.opcode in
  let _cpuid = Piqirun.gen_optional_field 3 gen__cpuid_pol x.Instr_pol.cpuid in
  Piqirun.gen_record code (_ident :: _opcode :: _cpuid :: [])

and gen__cpuid_pol code x =
  let _eax = Piqirun.gen_required_field 1 Common.gen__action x.Cpuid_pol.eax in
  let _ebx = Piqirun.gen_required_field 2 Common.gen__action x.Cpuid_pol.ebx in
  let _ecx = Piqirun.gen_required_field 3 Common.gen__action x.Cpuid_pol.ecx in
  let _edx = Piqirun.gen_required_field 4 Common.gen__action x.Cpuid_pol.edx in
  Piqirun.gen_record code (_eax :: _ebx :: _ecx :: _edx :: [])

and gen__instr_ident code x =
  Piqirun.int32_to_signed_varint code (match x with
    | `invalid_inst -> 0l
    | `cpuid -> 1l
  )
and packed_gen__instr_ident x =
  Piqirun.int32_to_packed_signed_varint (match x with
    | `invalid_inst -> 0l
    | `cpuid -> 1l
  )


let gen_string x = gen__string (-1) x
let gen_instr_pol x = gen__instr_pol (-1) x
let gen_cpuid_pol x = gen__cpuid_pol (-1) x
let gen_instr_ident x = gen__instr_ident (-1) x


let rec default_string () = ""
and default_instr_pol () =
  {
    Instr_pol.ident = default_instr_ident ();
    Instr_pol.opcode = default_string ();
    Instr_pol.cpuid = None;
  }
and default_cpuid_pol () =
  {
    Cpuid_pol.eax = Common.parse_action (Piqirun.parse_default "\b\000");
    Cpuid_pol.ebx = Common.parse_action (Piqirun.parse_default "\b\000");
    Cpuid_pol.ecx = Common.parse_action (Piqirun.parse_default "\b\000");
    Cpuid_pol.edx = Common.parse_action (Piqirun.parse_default "\b\000");
  }
and default_instr_ident () = `invalid_inst


let piqi = "\226\202\2304\011instruction\226\231\249\238\001\021piqi/instruction.piqi\170\150\212\160\004\011\226\202\2304\006common\162\244\146\155\011\015instruction_pol\218\244\134\182\012\250\001\138\233\142\251\014\243\001\210\203\242$I\232\146\150q\002\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\005ident\208\215\133\174\005\000\210\171\158\194\006\011instr-ident\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$E\232\146\150q\004\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\006opcode\208\215\133\174\005\000\210\171\158\194\006\006string\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$G\232\146\150q\006\152\247\223\136\002\000\152\182\154\152\004\160\223\186\243\001\232\243\204\157\004\000\218\164\238\191\004\005cpuid\208\215\133\174\005\000\210\171\158\194\006\tcpuid-pol\192\139\160\247\t\000\136\158\147\199\014\000\218\164\238\191\004\tinstr-pol\218\244\134\182\012\130\003\138\233\142\251\014\251\002\210\203\242$V\232\146\150q\002\152\247\223\136\002\000\152\182\154\152\004\160\223\186\243\001\232\243\204\157\004\000\218\164\238\191\004\003eax\208\215\133\174\005\000\210\171\158\194\006\rcommon/action\192\139\160\247\t\000\138\140\251\240\r\007\218\148\211\024\002\b\000\136\158\147\199\014\000\210\203\242$V\232\146\150q\004\152\247\223\136\002\000\152\182\154\152\004\160\223\186\243\001\232\243\204\157\004\000\218\164\238\191\004\003ebx\208\215\133\174\005\000\210\171\158\194\006\rcommon/action\192\139\160\247\t\000\138\140\251\240\r\007\218\148\211\024\002\b\000\136\158\147\199\014\000\210\203\242$V\232\146\150q\006\152\247\223\136\002\000\152\182\154\152\004\160\223\186\243\001\232\243\204\157\004\000\218\164\238\191\004\003ecx\208\215\133\174\005\000\210\171\158\194\006\rcommon/action\192\139\160\247\t\000\138\140\251\240\r\007\218\148\211\024\002\b\000\136\158\147\199\014\000\210\203\242$V\232\146\150q\b\152\247\223\136\002\000\152\182\154\152\004\160\223\186\243\001\232\243\204\157\004\000\218\164\238\191\004\003edx\208\215\133\174\005\000\210\171\158\194\006\rcommon/action\192\139\160\247\t\000\138\140\251\240\r\007\218\148\211\024\002\b\000\136\158\147\199\014\000\218\164\238\191\004\tcpuid-pol\218\244\134\182\012V\138\176\205\197\001P\218\164\238\191\004\011instr-ident\170\183\218\222\005\029\232\146\150q\000\152\247\223\136\002\000\218\164\238\191\004\012INVALID-INST\170\183\218\222\005\022\232\146\150q\002\152\247\223\136\002\000\218\164\238\191\004\005CPUID"
include Instruction_piqi
