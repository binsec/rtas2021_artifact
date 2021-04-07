module Common_ext = Common_piqi_ext
module Dba_ext = Dba_piqi_ext
module Config_ext = Config_piqi_ext
let piqi = Message_piqi.piqi


let _ = Piqirun_ext.init_piqi piqi


let _message_start_exec_piqi_type = Piqirun_ext.find_piqi_type "message/message-start-exec"
let _message_bp_reached_piqi_type = Piqirun_ext.find_piqi_type "message/message-bp-reached"
let _message_patch_piqi_type = Piqirun_ext.find_piqi_type "message/message-patch"
let _message_infos_piqi_type = Piqirun_ext.find_piqi_type "message/message-infos"
let _message_decode_instr_piqi_type = Piqirun_ext.find_piqi_type "message/message-decode-instr"
let _message_decode_instr_instr_entry_piqi_type = Piqirun_ext.find_piqi_type "message/message-decode-instr-instr-entry"
let _message_decode_instr_bin_kind_piqi_type = Piqirun_ext.find_piqi_type "message/message-decode-instr-bin-kind"
let _message_decode_instr_reply_piqi_type = Piqirun_ext.find_piqi_type "message/message-decode-instr-reply"
let _message_decode_instr_reply_instr_entry_piqi_type = Piqirun_ext.find_piqi_type "message/message-decode-instr-reply-instr-entry"
let _message_start_symb_exec_piqi_type = Piqirun_ext.find_piqi_type "message/message-start-symb-exec"
let _message_start_symb_exec_trace_kind_piqi_type = Piqirun_ext.find_piqi_type "message/message-start-symb-exec-trace-kind"
let _message_output_piqi_type = Piqirun_ext.find_piqi_type "message/message-output"
let _message_output_output_kind_piqi_type = Piqirun_ext.find_piqi_type "message/message-output-output-kind"


let parse_message_start_exec ?opts x (format :Piqirun_ext.input_format) =
  let x_pb = Piqirun_ext.convert _message_start_exec_piqi_type format `pb x ?opts in
  let buf = Piqirun.init_from_string x_pb in
  Message_piqi.parse_message_start_exec buf

let parse_message_bp_reached ?opts x (format :Piqirun_ext.input_format) =
  let x_pb = Piqirun_ext.convert _message_bp_reached_piqi_type format `pb x ?opts in
  let buf = Piqirun.init_from_string x_pb in
  Message_piqi.parse_message_bp_reached buf

let parse_message_patch ?opts x (format :Piqirun_ext.input_format) =
  let x_pb = Piqirun_ext.convert _message_patch_piqi_type format `pb x ?opts in
  let buf = Piqirun.init_from_string x_pb in
  Message_piqi.parse_message_patch buf

let parse_message_infos ?opts x (format :Piqirun_ext.input_format) =
  let x_pb = Piqirun_ext.convert _message_infos_piqi_type format `pb x ?opts in
  let buf = Piqirun.init_from_string x_pb in
  Message_piqi.parse_message_infos buf

let parse_message_decode_instr ?opts x (format :Piqirun_ext.input_format) =
  let x_pb = Piqirun_ext.convert _message_decode_instr_piqi_type format `pb x ?opts in
  let buf = Piqirun.init_from_string x_pb in
  Message_piqi.parse_message_decode_instr buf

let parse_message_decode_instr_instr_entry ?opts x (format :Piqirun_ext.input_format) =
  let x_pb = Piqirun_ext.convert _message_decode_instr_instr_entry_piqi_type format `pb x ?opts in
  let buf = Piqirun.init_from_string x_pb in
  Message_piqi.parse_message_decode_instr_instr_entry buf

let parse_message_decode_instr_bin_kind ?opts x (format :Piqirun_ext.input_format) =
  let x_pb = Piqirun_ext.convert _message_decode_instr_bin_kind_piqi_type format `pb x ?opts in
  let buf = Piqirun.init_from_string x_pb in
  Message_piqi.parse_message_decode_instr_bin_kind buf

let parse_message_decode_instr_reply ?opts x (format :Piqirun_ext.input_format) =
  let x_pb = Piqirun_ext.convert _message_decode_instr_reply_piqi_type format `pb x ?opts in
  let buf = Piqirun.init_from_string x_pb in
  Message_piqi.parse_message_decode_instr_reply buf

let parse_message_decode_instr_reply_instr_entry ?opts x (format :Piqirun_ext.input_format) =
  let x_pb = Piqirun_ext.convert _message_decode_instr_reply_instr_entry_piqi_type format `pb x ?opts in
  let buf = Piqirun.init_from_string x_pb in
  Message_piqi.parse_message_decode_instr_reply_instr_entry buf

let parse_message_start_symb_exec ?opts x (format :Piqirun_ext.input_format) =
  let x_pb = Piqirun_ext.convert _message_start_symb_exec_piqi_type format `pb x ?opts in
  let buf = Piqirun.init_from_string x_pb in
  Message_piqi.parse_message_start_symb_exec buf

let parse_message_start_symb_exec_trace_kind ?opts x (format :Piqirun_ext.input_format) =
  let x_pb = Piqirun_ext.convert _message_start_symb_exec_trace_kind_piqi_type format `pb x ?opts in
  let buf = Piqirun.init_from_string x_pb in
  Message_piqi.parse_message_start_symb_exec_trace_kind buf

let parse_message_output ?opts x (format :Piqirun_ext.input_format) =
  let x_pb = Piqirun_ext.convert _message_output_piqi_type format `pb x ?opts in
  let buf = Piqirun.init_from_string x_pb in
  Message_piqi.parse_message_output buf

let parse_message_output_output_kind ?opts x (format :Piqirun_ext.input_format) =
  let x_pb = Piqirun_ext.convert _message_output_output_kind_piqi_type format `pb x ?opts in
  let buf = Piqirun.init_from_string x_pb in
  Message_piqi.parse_message_output_output_kind buf


let gen_message_start_exec ?opts x (format :Piqirun_ext.output_format) =
  let buf = Message_piqi.gen_message_start_exec x in
  let x_pb = Piqirun.to_string buf in
  Piqirun_ext.convert _message_start_exec_piqi_type `pb format x_pb ?opts

let gen_message_bp_reached ?opts x (format :Piqirun_ext.output_format) =
  let buf = Message_piqi.gen_message_bp_reached x in
  let x_pb = Piqirun.to_string buf in
  Piqirun_ext.convert _message_bp_reached_piqi_type `pb format x_pb ?opts

let gen_message_patch ?opts x (format :Piqirun_ext.output_format) =
  let buf = Message_piqi.gen_message_patch x in
  let x_pb = Piqirun.to_string buf in
  Piqirun_ext.convert _message_patch_piqi_type `pb format x_pb ?opts

let gen_message_infos ?opts x (format :Piqirun_ext.output_format) =
  let buf = Message_piqi.gen_message_infos x in
  let x_pb = Piqirun.to_string buf in
  Piqirun_ext.convert _message_infos_piqi_type `pb format x_pb ?opts

let gen_message_decode_instr ?opts x (format :Piqirun_ext.output_format) =
  let buf = Message_piqi.gen_message_decode_instr x in
  let x_pb = Piqirun.to_string buf in
  Piqirun_ext.convert _message_decode_instr_piqi_type `pb format x_pb ?opts

let gen_message_decode_instr_instr_entry ?opts x (format :Piqirun_ext.output_format) =
  let buf = Message_piqi.gen_message_decode_instr_instr_entry x in
  let x_pb = Piqirun.to_string buf in
  Piqirun_ext.convert _message_decode_instr_instr_entry_piqi_type `pb format x_pb ?opts

let gen_message_decode_instr_bin_kind ?opts x (format :Piqirun_ext.output_format) =
  let buf = Message_piqi.gen_message_decode_instr_bin_kind x in
  let x_pb = Piqirun.to_string buf in
  Piqirun_ext.convert _message_decode_instr_bin_kind_piqi_type `pb format x_pb ?opts

let gen_message_decode_instr_reply ?opts x (format :Piqirun_ext.output_format) =
  let buf = Message_piqi.gen_message_decode_instr_reply x in
  let x_pb = Piqirun.to_string buf in
  Piqirun_ext.convert _message_decode_instr_reply_piqi_type `pb format x_pb ?opts

let gen_message_decode_instr_reply_instr_entry ?opts x (format :Piqirun_ext.output_format) =
  let buf = Message_piqi.gen_message_decode_instr_reply_instr_entry x in
  let x_pb = Piqirun.to_string buf in
  Piqirun_ext.convert _message_decode_instr_reply_instr_entry_piqi_type `pb format x_pb ?opts

let gen_message_start_symb_exec ?opts x (format :Piqirun_ext.output_format) =
  let buf = Message_piqi.gen_message_start_symb_exec x in
  let x_pb = Piqirun.to_string buf in
  Piqirun_ext.convert _message_start_symb_exec_piqi_type `pb format x_pb ?opts

let gen_message_start_symb_exec_trace_kind ?opts x (format :Piqirun_ext.output_format) =
  let buf = Message_piqi.gen_message_start_symb_exec_trace_kind x in
  let x_pb = Piqirun.to_string buf in
  Piqirun_ext.convert _message_start_symb_exec_trace_kind_piqi_type `pb format x_pb ?opts

let gen_message_output ?opts x (format :Piqirun_ext.output_format) =
  let buf = Message_piqi.gen_message_output x in
  let x_pb = Piqirun.to_string buf in
  Piqirun_ext.convert _message_output_piqi_type `pb format x_pb ?opts

let gen_message_output_output_kind ?opts x (format :Piqirun_ext.output_format) =
  let buf = Message_piqi.gen_message_output_output_kind x in
  let x_pb = Piqirun.to_string buf in
  Piqirun_ext.convert _message_output_output_kind_piqi_type `pb format x_pb ?opts


let print_message_start_exec ?opts x =
  Stdlib.print_endline (gen_message_start_exec x `piq ?opts)
let prerr_message_start_exec ?opts x =
  Stdlib.prerr_endline (gen_message_start_exec x `piq ?opts)

let print_message_bp_reached ?opts x =
  Stdlib.print_endline (gen_message_bp_reached x `piq ?opts)
let prerr_message_bp_reached ?opts x =
  Stdlib.prerr_endline (gen_message_bp_reached x `piq ?opts)

let print_message_patch ?opts x =
  Stdlib.print_endline (gen_message_patch x `piq ?opts)
let prerr_message_patch ?opts x =
  Stdlib.prerr_endline (gen_message_patch x `piq ?opts)

let print_message_infos ?opts x =
  Stdlib.print_endline (gen_message_infos x `piq ?opts)
let prerr_message_infos ?opts x =
  Stdlib.prerr_endline (gen_message_infos x `piq ?opts)

let print_message_decode_instr ?opts x =
  Stdlib.print_endline (gen_message_decode_instr x `piq ?opts)
let prerr_message_decode_instr ?opts x =
  Stdlib.prerr_endline (gen_message_decode_instr x `piq ?opts)

let print_message_decode_instr_instr_entry ?opts x =
  Stdlib.print_endline (gen_message_decode_instr_instr_entry x `piq ?opts)
let prerr_message_decode_instr_instr_entry ?opts x =
  Stdlib.prerr_endline (gen_message_decode_instr_instr_entry x `piq ?opts)

let print_message_decode_instr_bin_kind ?opts x =
  Stdlib.print_endline (gen_message_decode_instr_bin_kind x `piq ?opts)
let prerr_message_decode_instr_bin_kind ?opts x =
  Stdlib.prerr_endline (gen_message_decode_instr_bin_kind x `piq ?opts)

let print_message_decode_instr_reply ?opts x =
  Stdlib.print_endline (gen_message_decode_instr_reply x `piq ?opts)
let prerr_message_decode_instr_reply ?opts x =
  Stdlib.prerr_endline (gen_message_decode_instr_reply x `piq ?opts)

let print_message_decode_instr_reply_instr_entry ?opts x =
  Stdlib.print_endline (gen_message_decode_instr_reply_instr_entry x `piq ?opts)
let prerr_message_decode_instr_reply_instr_entry ?opts x =
  Stdlib.prerr_endline (gen_message_decode_instr_reply_instr_entry x `piq ?opts)

let print_message_start_symb_exec ?opts x =
  Stdlib.print_endline (gen_message_start_symb_exec x `piq ?opts)
let prerr_message_start_symb_exec ?opts x =
  Stdlib.prerr_endline (gen_message_start_symb_exec x `piq ?opts)

let print_message_start_symb_exec_trace_kind ?opts x =
  Stdlib.print_endline (gen_message_start_symb_exec_trace_kind x `piq ?opts)
let prerr_message_start_symb_exec_trace_kind ?opts x =
  Stdlib.prerr_endline (gen_message_start_symb_exec_trace_kind x `piq ?opts)

let print_message_output ?opts x =
  Stdlib.print_endline (gen_message_output x `piq ?opts)
let prerr_message_output ?opts x =
  Stdlib.prerr_endline (gen_message_output x `piq ?opts)

let print_message_output_output_kind ?opts x =
  Stdlib.print_endline (gen_message_output_output_kind x `piq ?opts)
let prerr_message_output_output_kind ?opts x =
  Stdlib.prerr_endline (gen_message_output_output_kind x `piq ?opts)


