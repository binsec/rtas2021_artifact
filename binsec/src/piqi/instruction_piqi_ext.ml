module Common_ext = Common_piqi_ext
let piqi = Instruction_piqi.piqi


let _ = Piqirun_ext.init_piqi piqi


let _instr_pol_piqi_type = Piqirun_ext.find_piqi_type "instruction/instr-pol"
let _cpuid_pol_piqi_type = Piqirun_ext.find_piqi_type "instruction/cpuid-pol"
let _instr_ident_piqi_type = Piqirun_ext.find_piqi_type "instruction/instr-ident"


let parse_instr_pol ?opts x (format :Piqirun_ext.input_format) =
  let x_pb = Piqirun_ext.convert _instr_pol_piqi_type format `pb x ?opts in
  let buf = Piqirun.init_from_string x_pb in
  Instruction_piqi.parse_instr_pol buf

let parse_cpuid_pol ?opts x (format :Piqirun_ext.input_format) =
  let x_pb = Piqirun_ext.convert _cpuid_pol_piqi_type format `pb x ?opts in
  let buf = Piqirun.init_from_string x_pb in
  Instruction_piqi.parse_cpuid_pol buf

let parse_instr_ident ?opts x (format :Piqirun_ext.input_format) =
  let x_pb = Piqirun_ext.convert _instr_ident_piqi_type format `pb x ?opts in
  let buf = Piqirun.init_from_string x_pb in
  Instruction_piqi.parse_instr_ident buf


let gen_instr_pol ?opts x (format :Piqirun_ext.output_format) =
  let buf = Instruction_piqi.gen_instr_pol x in
  let x_pb = Piqirun.to_string buf in
  Piqirun_ext.convert _instr_pol_piqi_type `pb format x_pb ?opts

let gen_cpuid_pol ?opts x (format :Piqirun_ext.output_format) =
  let buf = Instruction_piqi.gen_cpuid_pol x in
  let x_pb = Piqirun.to_string buf in
  Piqirun_ext.convert _cpuid_pol_piqi_type `pb format x_pb ?opts

let gen_instr_ident ?opts x (format :Piqirun_ext.output_format) =
  let buf = Instruction_piqi.gen_instr_ident x in
  let x_pb = Piqirun.to_string buf in
  Piqirun_ext.convert _instr_ident_piqi_type `pb format x_pb ?opts


let print_instr_pol ?opts x =
  Stdlib.print_endline (gen_instr_pol x `piq ?opts)
let prerr_instr_pol ?opts x =
  Stdlib.prerr_endline (gen_instr_pol x `piq ?opts)

let print_cpuid_pol ?opts x =
  Stdlib.print_endline (gen_cpuid_pol x `piq ?opts)
let prerr_cpuid_pol ?opts x =
  Stdlib.prerr_endline (gen_cpuid_pol x `piq ?opts)

let print_instr_ident ?opts x =
  Stdlib.print_endline (gen_instr_ident x `piq ?opts)
let prerr_instr_ident ?opts x =
  Stdlib.prerr_endline (gen_instr_ident x `piq ?opts)


