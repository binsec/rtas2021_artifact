module Common_ext = Common_piqi_ext
module Libcall_ext = Libcall_piqi_ext
module Syscall_ext = Syscall_piqi_ext
module Instruction_ext = Instruction_piqi_ext
module Analysis_config_ext = Analysis_config_piqi_ext
let piqi = Config_piqi.piqi


let _ = Piqirun_ext.init_piqi piqi


let _call_name_t_piqi_type = Piqirun_ext.find_piqi_type "config/call-name-t"
let _configuration_piqi_type = Piqirun_ext.find_piqi_type "config/configuration"
let _input_t_piqi_type = Piqirun_ext.find_piqi_type "config/input-t"
let _input_t_input_kind_piqi_type = Piqirun_ext.find_piqi_type "config/input-t-input-kind"
let _input_t_when_t_piqi_type = Piqirun_ext.find_piqi_type "config/input-t-when-t"


let parse_call_name_t ?opts x (format :Piqirun_ext.input_format) =
  let x_pb = Piqirun_ext.convert _call_name_t_piqi_type format `pb x ?opts in
  let buf = Piqirun.init_from_string x_pb in
  Config_piqi.parse_call_name_t buf

let parse_configuration ?opts x (format :Piqirun_ext.input_format) =
  let x_pb = Piqirun_ext.convert _configuration_piqi_type format `pb x ?opts in
  let buf = Piqirun.init_from_string x_pb in
  Config_piqi.parse_configuration buf

let parse_input_t ?opts x (format :Piqirun_ext.input_format) =
  let x_pb = Piqirun_ext.convert _input_t_piqi_type format `pb x ?opts in
  let buf = Piqirun.init_from_string x_pb in
  Config_piqi.parse_input_t buf

let parse_input_t_input_kind ?opts x (format :Piqirun_ext.input_format) =
  let x_pb = Piqirun_ext.convert _input_t_input_kind_piqi_type format `pb x ?opts in
  let buf = Piqirun.init_from_string x_pb in
  Config_piqi.parse_input_t_input_kind buf

let parse_input_t_when_t ?opts x (format :Piqirun_ext.input_format) =
  let x_pb = Piqirun_ext.convert _input_t_when_t_piqi_type format `pb x ?opts in
  let buf = Piqirun.init_from_string x_pb in
  Config_piqi.parse_input_t_when_t buf


let gen_call_name_t ?opts x (format :Piqirun_ext.output_format) =
  let buf = Config_piqi.gen_call_name_t x in
  let x_pb = Piqirun.to_string buf in
  Piqirun_ext.convert _call_name_t_piqi_type `pb format x_pb ?opts

let gen_configuration ?opts x (format :Piqirun_ext.output_format) =
  let buf = Config_piqi.gen_configuration x in
  let x_pb = Piqirun.to_string buf in
  Piqirun_ext.convert _configuration_piqi_type `pb format x_pb ?opts

let gen_input_t ?opts x (format :Piqirun_ext.output_format) =
  let buf = Config_piqi.gen_input_t x in
  let x_pb = Piqirun.to_string buf in
  Piqirun_ext.convert _input_t_piqi_type `pb format x_pb ?opts

let gen_input_t_input_kind ?opts x (format :Piqirun_ext.output_format) =
  let buf = Config_piqi.gen_input_t_input_kind x in
  let x_pb = Piqirun.to_string buf in
  Piqirun_ext.convert _input_t_input_kind_piqi_type `pb format x_pb ?opts

let gen_input_t_when_t ?opts x (format :Piqirun_ext.output_format) =
  let buf = Config_piqi.gen_input_t_when_t x in
  let x_pb = Piqirun.to_string buf in
  Piqirun_ext.convert _input_t_when_t_piqi_type `pb format x_pb ?opts


let print_call_name_t ?opts x =
  Stdlib.print_endline (gen_call_name_t x `piq ?opts)
let prerr_call_name_t ?opts x =
  Stdlib.prerr_endline (gen_call_name_t x `piq ?opts)

let print_configuration ?opts x =
  Stdlib.print_endline (gen_configuration x `piq ?opts)
let prerr_configuration ?opts x =
  Stdlib.prerr_endline (gen_configuration x `piq ?opts)

let print_input_t ?opts x =
  Stdlib.print_endline (gen_input_t x `piq ?opts)
let prerr_input_t ?opts x =
  Stdlib.prerr_endline (gen_input_t x `piq ?opts)

let print_input_t_input_kind ?opts x =
  Stdlib.print_endline (gen_input_t_input_kind x `piq ?opts)
let prerr_input_t_input_kind ?opts x =
  Stdlib.prerr_endline (gen_input_t_input_kind x `piq ?opts)

let print_input_t_when_t ?opts x =
  Stdlib.print_endline (gen_input_t_when_t x `piq ?opts)
let prerr_input_t_when_t ?opts x =
  Stdlib.prerr_endline (gen_input_t_when_t x `piq ?opts)


