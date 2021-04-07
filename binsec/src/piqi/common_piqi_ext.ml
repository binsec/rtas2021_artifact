let piqi = Common_piqi.piqi


let _ = Piqirun_ext.init_piqi piqi


let _register_value_t_piqi_type = Piqirun_ext.find_piqi_type "common/register-value-t"
let _register_t_piqi_type = Piqirun_ext.find_piqi_type "common/register-t"
let _memory_t_piqi_type = Piqirun_ext.find_piqi_type "common/memory-t"
let _indirect_register_t_piqi_type = Piqirun_ext.find_piqi_type "common/indirect-register-t"
let _memory_pol_piqi_type = Piqirun_ext.find_piqi_type "common/memory-pol"
let _smt_result_piqi_type = Piqirun_ext.find_piqi_type "common/smt-result"
let _tracing_action_piqi_type = Piqirun_ext.find_piqi_type "common/tracing-action"
let _action_piqi_type = Piqirun_ext.find_piqi_type "common/action"
let _call_convention_t_piqi_type = Piqirun_ext.find_piqi_type "common/call-convention-t"
let _ir_kind_t_piqi_type = Piqirun_ext.find_piqi_type "common/ir-kind-t"
let _solver_t_piqi_type = Piqirun_ext.find_piqi_type "common/solver-t"
let _analysis_direction_t_piqi_type = Piqirun_ext.find_piqi_type "common/analysis-direction-t"
let _proto_size_t_piqi_type = Piqirun_ext.find_piqi_type "common/proto-size-t"


let parse_register_value_t ?opts x (format :Piqirun_ext.input_format) =
  let x_pb = Piqirun_ext.convert _register_value_t_piqi_type format `pb x ?opts in
  let buf = Piqirun.init_from_string x_pb in
  Common_piqi.parse_register_value_t buf

let parse_register_t ?opts x (format :Piqirun_ext.input_format) =
  let x_pb = Piqirun_ext.convert _register_t_piqi_type format `pb x ?opts in
  let buf = Piqirun.init_from_string x_pb in
  Common_piqi.parse_register_t buf

let parse_memory_t ?opts x (format :Piqirun_ext.input_format) =
  let x_pb = Piqirun_ext.convert _memory_t_piqi_type format `pb x ?opts in
  let buf = Piqirun.init_from_string x_pb in
  Common_piqi.parse_memory_t buf

let parse_indirect_register_t ?opts x (format :Piqirun_ext.input_format) =
  let x_pb = Piqirun_ext.convert _indirect_register_t_piqi_type format `pb x ?opts in
  let buf = Piqirun.init_from_string x_pb in
  Common_piqi.parse_indirect_register_t buf

let parse_memory_pol ?opts x (format :Piqirun_ext.input_format) =
  let x_pb = Piqirun_ext.convert _memory_pol_piqi_type format `pb x ?opts in
  let buf = Piqirun.init_from_string x_pb in
  Common_piqi.parse_memory_pol buf

let parse_smt_result ?opts x (format :Piqirun_ext.input_format) =
  let x_pb = Piqirun_ext.convert _smt_result_piqi_type format `pb x ?opts in
  let buf = Piqirun.init_from_string x_pb in
  Common_piqi.parse_smt_result buf

let parse_tracing_action ?opts x (format :Piqirun_ext.input_format) =
  let x_pb = Piqirun_ext.convert _tracing_action_piqi_type format `pb x ?opts in
  let buf = Piqirun.init_from_string x_pb in
  Common_piqi.parse_tracing_action buf

let parse_action ?opts x (format :Piqirun_ext.input_format) =
  let x_pb = Piqirun_ext.convert _action_piqi_type format `pb x ?opts in
  let buf = Piqirun.init_from_string x_pb in
  Common_piqi.parse_action buf

let parse_call_convention_t ?opts x (format :Piqirun_ext.input_format) =
  let x_pb = Piqirun_ext.convert _call_convention_t_piqi_type format `pb x ?opts in
  let buf = Piqirun.init_from_string x_pb in
  Common_piqi.parse_call_convention_t buf

let parse_ir_kind_t ?opts x (format :Piqirun_ext.input_format) =
  let x_pb = Piqirun_ext.convert _ir_kind_t_piqi_type format `pb x ?opts in
  let buf = Piqirun.init_from_string x_pb in
  Common_piqi.parse_ir_kind_t buf

let parse_solver_t ?opts x (format :Piqirun_ext.input_format) =
  let x_pb = Piqirun_ext.convert _solver_t_piqi_type format `pb x ?opts in
  let buf = Piqirun.init_from_string x_pb in
  Common_piqi.parse_solver_t buf

let parse_analysis_direction_t ?opts x (format :Piqirun_ext.input_format) =
  let x_pb = Piqirun_ext.convert _analysis_direction_t_piqi_type format `pb x ?opts in
  let buf = Piqirun.init_from_string x_pb in
  Common_piqi.parse_analysis_direction_t buf

let parse_proto_size_t ?opts x (format :Piqirun_ext.input_format) =
  let x_pb = Piqirun_ext.convert _proto_size_t_piqi_type format `pb x ?opts in
  let buf = Piqirun.init_from_string x_pb in
  Common_piqi.parse_proto_size_t buf


let gen_register_value_t ?opts x (format :Piqirun_ext.output_format) =
  let buf = Common_piqi.gen_register_value_t x in
  let x_pb = Piqirun.to_string buf in
  Piqirun_ext.convert _register_value_t_piqi_type `pb format x_pb ?opts

let gen_register_t ?opts x (format :Piqirun_ext.output_format) =
  let buf = Common_piqi.gen_register_t x in
  let x_pb = Piqirun.to_string buf in
  Piqirun_ext.convert _register_t_piqi_type `pb format x_pb ?opts

let gen_memory_t ?opts x (format :Piqirun_ext.output_format) =
  let buf = Common_piqi.gen_memory_t x in
  let x_pb = Piqirun.to_string buf in
  Piqirun_ext.convert _memory_t_piqi_type `pb format x_pb ?opts

let gen_indirect_register_t ?opts x (format :Piqirun_ext.output_format) =
  let buf = Common_piqi.gen_indirect_register_t x in
  let x_pb = Piqirun.to_string buf in
  Piqirun_ext.convert _indirect_register_t_piqi_type `pb format x_pb ?opts

let gen_memory_pol ?opts x (format :Piqirun_ext.output_format) =
  let buf = Common_piqi.gen_memory_pol x in
  let x_pb = Piqirun.to_string buf in
  Piqirun_ext.convert _memory_pol_piqi_type `pb format x_pb ?opts

let gen_smt_result ?opts x (format :Piqirun_ext.output_format) =
  let buf = Common_piqi.gen_smt_result x in
  let x_pb = Piqirun.to_string buf in
  Piqirun_ext.convert _smt_result_piqi_type `pb format x_pb ?opts

let gen_tracing_action ?opts x (format :Piqirun_ext.output_format) =
  let buf = Common_piqi.gen_tracing_action x in
  let x_pb = Piqirun.to_string buf in
  Piqirun_ext.convert _tracing_action_piqi_type `pb format x_pb ?opts

let gen_action ?opts x (format :Piqirun_ext.output_format) =
  let buf = Common_piqi.gen_action x in
  let x_pb = Piqirun.to_string buf in
  Piqirun_ext.convert _action_piqi_type `pb format x_pb ?opts

let gen_call_convention_t ?opts x (format :Piqirun_ext.output_format) =
  let buf = Common_piqi.gen_call_convention_t x in
  let x_pb = Piqirun.to_string buf in
  Piqirun_ext.convert _call_convention_t_piqi_type `pb format x_pb ?opts

let gen_ir_kind_t ?opts x (format :Piqirun_ext.output_format) =
  let buf = Common_piqi.gen_ir_kind_t x in
  let x_pb = Piqirun.to_string buf in
  Piqirun_ext.convert _ir_kind_t_piqi_type `pb format x_pb ?opts

let gen_solver_t ?opts x (format :Piqirun_ext.output_format) =
  let buf = Common_piqi.gen_solver_t x in
  let x_pb = Piqirun.to_string buf in
  Piqirun_ext.convert _solver_t_piqi_type `pb format x_pb ?opts

let gen_analysis_direction_t ?opts x (format :Piqirun_ext.output_format) =
  let buf = Common_piqi.gen_analysis_direction_t x in
  let x_pb = Piqirun.to_string buf in
  Piqirun_ext.convert _analysis_direction_t_piqi_type `pb format x_pb ?opts

let gen_proto_size_t ?opts x (format :Piqirun_ext.output_format) =
  let buf = Common_piqi.gen_proto_size_t x in
  let x_pb = Piqirun.to_string buf in
  Piqirun_ext.convert _proto_size_t_piqi_type `pb format x_pb ?opts


let print_register_value_t ?opts x =
  Stdlib.print_endline (gen_register_value_t x `piq ?opts)
let prerr_register_value_t ?opts x =
  Stdlib.prerr_endline (gen_register_value_t x `piq ?opts)

let print_register_t ?opts x =
  Stdlib.print_endline (gen_register_t x `piq ?opts)
let prerr_register_t ?opts x =
  Stdlib.prerr_endline (gen_register_t x `piq ?opts)

let print_memory_t ?opts x =
  Stdlib.print_endline (gen_memory_t x `piq ?opts)
let prerr_memory_t ?opts x =
  Stdlib.prerr_endline (gen_memory_t x `piq ?opts)

let print_indirect_register_t ?opts x =
  Stdlib.print_endline (gen_indirect_register_t x `piq ?opts)
let prerr_indirect_register_t ?opts x =
  Stdlib.prerr_endline (gen_indirect_register_t x `piq ?opts)

let print_memory_pol ?opts x =
  Stdlib.print_endline (gen_memory_pol x `piq ?opts)
let prerr_memory_pol ?opts x =
  Stdlib.prerr_endline (gen_memory_pol x `piq ?opts)

let print_smt_result ?opts x =
  Stdlib.print_endline (gen_smt_result x `piq ?opts)
let prerr_smt_result ?opts x =
  Stdlib.prerr_endline (gen_smt_result x `piq ?opts)

let print_tracing_action ?opts x =
  Stdlib.print_endline (gen_tracing_action x `piq ?opts)
let prerr_tracing_action ?opts x =
  Stdlib.prerr_endline (gen_tracing_action x `piq ?opts)

let print_action ?opts x =
  Stdlib.print_endline (gen_action x `piq ?opts)
let prerr_action ?opts x =
  Stdlib.prerr_endline (gen_action x `piq ?opts)

let print_call_convention_t ?opts x =
  Stdlib.print_endline (gen_call_convention_t x `piq ?opts)
let prerr_call_convention_t ?opts x =
  Stdlib.prerr_endline (gen_call_convention_t x `piq ?opts)

let print_ir_kind_t ?opts x =
  Stdlib.print_endline (gen_ir_kind_t x `piq ?opts)
let prerr_ir_kind_t ?opts x =
  Stdlib.prerr_endline (gen_ir_kind_t x `piq ?opts)

let print_solver_t ?opts x =
  Stdlib.print_endline (gen_solver_t x `piq ?opts)
let prerr_solver_t ?opts x =
  Stdlib.prerr_endline (gen_solver_t x `piq ?opts)

let print_analysis_direction_t ?opts x =
  Stdlib.print_endline (gen_analysis_direction_t x `piq ?opts)
let prerr_analysis_direction_t ?opts x =
  Stdlib.prerr_endline (gen_analysis_direction_t x `piq ?opts)

let print_proto_size_t ?opts x =
  Stdlib.print_endline (gen_proto_size_t x `piq ?opts)
let prerr_proto_size_t ?opts x =
  Stdlib.prerr_endline (gen_proto_size_t x `piq ?opts)


