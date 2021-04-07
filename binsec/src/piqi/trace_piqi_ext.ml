module Dba_ext = Dba_piqi_ext
module Common_ext = Common_piqi_ext
module Syscall_ext = Syscall_piqi_ext
module Libcall_ext = Libcall_piqi_ext
let piqi = Trace_piqi.piqi


let _ = Piqirun_ext.init_piqi piqi


let _header_t_piqi_type = Piqirun_ext.find_piqi_type "trace/header-t"
let _header_t_architecture_t_piqi_type = Piqirun_ext.find_piqi_type "trace/header-t-architecture-t"
let _metadata_t_piqi_type = Piqirun_ext.find_piqi_type "trace/metadata-t"
let _metadata_t_exception_t_piqi_type = Piqirun_ext.find_piqi_type "trace/metadata-t-exception-t"
let _metadata_t_typeid_metadata_t_piqi_type = Piqirun_ext.find_piqi_type "trace/metadata-t-typeid-metadata-t"
let _ins_con_info_t_piqi_type = Piqirun_ext.find_piqi_type "trace/ins-con-info-t"
let _ins_con_info_t_typeid_con_info_t_piqi_type = Piqirun_ext.find_piqi_type "trace/ins-con-info-t-typeid-con-info-t"
let _instruction_t_piqi_type = Piqirun_ext.find_piqi_type "trace/instruction-t"
let _body_t_piqi_type = Piqirun_ext.find_piqi_type "trace/body-t"
let _body_t_typeid_body_t_piqi_type = Piqirun_ext.find_piqi_type "trace/body-t-typeid-body-t"
let _chunk_t_piqi_type = Piqirun_ext.find_piqi_type "trace/chunk-t"
let _trace_t_piqi_type = Piqirun_ext.find_piqi_type "trace/trace-t"


let parse_header_t ?opts x (format :Piqirun_ext.input_format) =
  let x_pb = Piqirun_ext.convert _header_t_piqi_type format `pb x ?opts in
  let buf = Piqirun.init_from_string x_pb in
  Trace_piqi.parse_header_t buf

let parse_header_t_architecture_t ?opts x (format :Piqirun_ext.input_format) =
  let x_pb = Piqirun_ext.convert _header_t_architecture_t_piqi_type format `pb x ?opts in
  let buf = Piqirun.init_from_string x_pb in
  Trace_piqi.parse_header_t_architecture_t buf

let parse_metadata_t ?opts x (format :Piqirun_ext.input_format) =
  let x_pb = Piqirun_ext.convert _metadata_t_piqi_type format `pb x ?opts in
  let buf = Piqirun.init_from_string x_pb in
  Trace_piqi.parse_metadata_t buf

let parse_metadata_t_exception_t ?opts x (format :Piqirun_ext.input_format) =
  let x_pb = Piqirun_ext.convert _metadata_t_exception_t_piqi_type format `pb x ?opts in
  let buf = Piqirun.init_from_string x_pb in
  Trace_piqi.parse_metadata_t_exception_t buf

let parse_metadata_t_typeid_metadata_t ?opts x (format :Piqirun_ext.input_format) =
  let x_pb = Piqirun_ext.convert _metadata_t_typeid_metadata_t_piqi_type format `pb x ?opts in
  let buf = Piqirun.init_from_string x_pb in
  Trace_piqi.parse_metadata_t_typeid_metadata_t buf

let parse_ins_con_info_t ?opts x (format :Piqirun_ext.input_format) =
  let x_pb = Piqirun_ext.convert _ins_con_info_t_piqi_type format `pb x ?opts in
  let buf = Piqirun.init_from_string x_pb in
  Trace_piqi.parse_ins_con_info_t buf

let parse_ins_con_info_t_typeid_con_info_t ?opts x (format :Piqirun_ext.input_format) =
  let x_pb = Piqirun_ext.convert _ins_con_info_t_typeid_con_info_t_piqi_type format `pb x ?opts in
  let buf = Piqirun.init_from_string x_pb in
  Trace_piqi.parse_ins_con_info_t_typeid_con_info_t buf

let parse_instruction_t ?opts x (format :Piqirun_ext.input_format) =
  let x_pb = Piqirun_ext.convert _instruction_t_piqi_type format `pb x ?opts in
  let buf = Piqirun.init_from_string x_pb in
  Trace_piqi.parse_instruction_t buf

let parse_body_t ?opts x (format :Piqirun_ext.input_format) =
  let x_pb = Piqirun_ext.convert _body_t_piqi_type format `pb x ?opts in
  let buf = Piqirun.init_from_string x_pb in
  Trace_piqi.parse_body_t buf

let parse_body_t_typeid_body_t ?opts x (format :Piqirun_ext.input_format) =
  let x_pb = Piqirun_ext.convert _body_t_typeid_body_t_piqi_type format `pb x ?opts in
  let buf = Piqirun.init_from_string x_pb in
  Trace_piqi.parse_body_t_typeid_body_t buf

let parse_chunk_t ?opts x (format :Piqirun_ext.input_format) =
  let x_pb = Piqirun_ext.convert _chunk_t_piqi_type format `pb x ?opts in
  let buf = Piqirun.init_from_string x_pb in
  Trace_piqi.parse_chunk_t buf

let parse_trace_t ?opts x (format :Piqirun_ext.input_format) =
  let x_pb = Piqirun_ext.convert _trace_t_piqi_type format `pb x ?opts in
  let buf = Piqirun.init_from_string x_pb in
  Trace_piqi.parse_trace_t buf


let gen_header_t ?opts x (format :Piqirun_ext.output_format) =
  let buf = Trace_piqi.gen_header_t x in
  let x_pb = Piqirun.to_string buf in
  Piqirun_ext.convert _header_t_piqi_type `pb format x_pb ?opts

let gen_header_t_architecture_t ?opts x (format :Piqirun_ext.output_format) =
  let buf = Trace_piqi.gen_header_t_architecture_t x in
  let x_pb = Piqirun.to_string buf in
  Piqirun_ext.convert _header_t_architecture_t_piqi_type `pb format x_pb ?opts

let gen_metadata_t ?opts x (format :Piqirun_ext.output_format) =
  let buf = Trace_piqi.gen_metadata_t x in
  let x_pb = Piqirun.to_string buf in
  Piqirun_ext.convert _metadata_t_piqi_type `pb format x_pb ?opts

let gen_metadata_t_exception_t ?opts x (format :Piqirun_ext.output_format) =
  let buf = Trace_piqi.gen_metadata_t_exception_t x in
  let x_pb = Piqirun.to_string buf in
  Piqirun_ext.convert _metadata_t_exception_t_piqi_type `pb format x_pb ?opts

let gen_metadata_t_typeid_metadata_t ?opts x (format :Piqirun_ext.output_format) =
  let buf = Trace_piqi.gen_metadata_t_typeid_metadata_t x in
  let x_pb = Piqirun.to_string buf in
  Piqirun_ext.convert _metadata_t_typeid_metadata_t_piqi_type `pb format x_pb ?opts

let gen_ins_con_info_t ?opts x (format :Piqirun_ext.output_format) =
  let buf = Trace_piqi.gen_ins_con_info_t x in
  let x_pb = Piqirun.to_string buf in
  Piqirun_ext.convert _ins_con_info_t_piqi_type `pb format x_pb ?opts

let gen_ins_con_info_t_typeid_con_info_t ?opts x (format :Piqirun_ext.output_format) =
  let buf = Trace_piqi.gen_ins_con_info_t_typeid_con_info_t x in
  let x_pb = Piqirun.to_string buf in
  Piqirun_ext.convert _ins_con_info_t_typeid_con_info_t_piqi_type `pb format x_pb ?opts

let gen_instruction_t ?opts x (format :Piqirun_ext.output_format) =
  let buf = Trace_piqi.gen_instruction_t x in
  let x_pb = Piqirun.to_string buf in
  Piqirun_ext.convert _instruction_t_piqi_type `pb format x_pb ?opts

let gen_body_t ?opts x (format :Piqirun_ext.output_format) =
  let buf = Trace_piqi.gen_body_t x in
  let x_pb = Piqirun.to_string buf in
  Piqirun_ext.convert _body_t_piqi_type `pb format x_pb ?opts

let gen_body_t_typeid_body_t ?opts x (format :Piqirun_ext.output_format) =
  let buf = Trace_piqi.gen_body_t_typeid_body_t x in
  let x_pb = Piqirun.to_string buf in
  Piqirun_ext.convert _body_t_typeid_body_t_piqi_type `pb format x_pb ?opts

let gen_chunk_t ?opts x (format :Piqirun_ext.output_format) =
  let buf = Trace_piqi.gen_chunk_t x in
  let x_pb = Piqirun.to_string buf in
  Piqirun_ext.convert _chunk_t_piqi_type `pb format x_pb ?opts

let gen_trace_t ?opts x (format :Piqirun_ext.output_format) =
  let buf = Trace_piqi.gen_trace_t x in
  let x_pb = Piqirun.to_string buf in
  Piqirun_ext.convert _trace_t_piqi_type `pb format x_pb ?opts


let print_header_t ?opts x =
  Stdlib.print_endline (gen_header_t x `piq ?opts)
let prerr_header_t ?opts x =
  Stdlib.prerr_endline (gen_header_t x `piq ?opts)

let print_header_t_architecture_t ?opts x =
  Stdlib.print_endline (gen_header_t_architecture_t x `piq ?opts)
let prerr_header_t_architecture_t ?opts x =
  Stdlib.prerr_endline (gen_header_t_architecture_t x `piq ?opts)

let print_metadata_t ?opts x =
  Stdlib.print_endline (gen_metadata_t x `piq ?opts)
let prerr_metadata_t ?opts x =
  Stdlib.prerr_endline (gen_metadata_t x `piq ?opts)

let print_metadata_t_exception_t ?opts x =
  Stdlib.print_endline (gen_metadata_t_exception_t x `piq ?opts)
let prerr_metadata_t_exception_t ?opts x =
  Stdlib.prerr_endline (gen_metadata_t_exception_t x `piq ?opts)

let print_metadata_t_typeid_metadata_t ?opts x =
  Stdlib.print_endline (gen_metadata_t_typeid_metadata_t x `piq ?opts)
let prerr_metadata_t_typeid_metadata_t ?opts x =
  Stdlib.prerr_endline (gen_metadata_t_typeid_metadata_t x `piq ?opts)

let print_ins_con_info_t ?opts x =
  Stdlib.print_endline (gen_ins_con_info_t x `piq ?opts)
let prerr_ins_con_info_t ?opts x =
  Stdlib.prerr_endline (gen_ins_con_info_t x `piq ?opts)

let print_ins_con_info_t_typeid_con_info_t ?opts x =
  Stdlib.print_endline (gen_ins_con_info_t_typeid_con_info_t x `piq ?opts)
let prerr_ins_con_info_t_typeid_con_info_t ?opts x =
  Stdlib.prerr_endline (gen_ins_con_info_t_typeid_con_info_t x `piq ?opts)

let print_instruction_t ?opts x =
  Stdlib.print_endline (gen_instruction_t x `piq ?opts)
let prerr_instruction_t ?opts x =
  Stdlib.prerr_endline (gen_instruction_t x `piq ?opts)

let print_body_t ?opts x =
  Stdlib.print_endline (gen_body_t x `piq ?opts)
let prerr_body_t ?opts x =
  Stdlib.prerr_endline (gen_body_t x `piq ?opts)

let print_body_t_typeid_body_t ?opts x =
  Stdlib.print_endline (gen_body_t_typeid_body_t x `piq ?opts)
let prerr_body_t_typeid_body_t ?opts x =
  Stdlib.prerr_endline (gen_body_t_typeid_body_t x `piq ?opts)

let print_chunk_t ?opts x =
  Stdlib.print_endline (gen_chunk_t x `piq ?opts)
let prerr_chunk_t ?opts x =
  Stdlib.prerr_endline (gen_chunk_t x `piq ?opts)

let print_trace_t ?opts x =
  Stdlib.print_endline (gen_trace_t x `piq ?opts)
let prerr_trace_t ?opts x =
  Stdlib.prerr_endline (gen_trace_t x `piq ?opts)


