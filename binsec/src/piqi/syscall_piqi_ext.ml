module Common_ext = Common_piqi_ext
let piqi = Syscall_piqi.piqi


let _ = Piqirun_ext.init_piqi piqi


let _syscall_pol_piqi_type = Piqirun_ext.find_piqi_type "syscall/syscall-pol"
let _syscall_t_piqi_type = Piqirun_ext.find_piqi_type "syscall/syscall-t"
let _nr_read_pol_piqi_type = Piqirun_ext.find_piqi_type "syscall/nr-read-pol"
let _nr_open_t_piqi_type = Piqirun_ext.find_piqi_type "syscall/nr-open-t"
let _nr_read_t_piqi_type = Piqirun_ext.find_piqi_type "syscall/nr-read-t"
let _nr_write_t_piqi_type = Piqirun_ext.find_piqi_type "syscall/nr-write-t"
let _sys_generic_t_piqi_type = Piqirun_ext.find_piqi_type "syscall/sys-generic-t"
let _syscall_ident_piqi_type = Piqirun_ext.find_piqi_type "syscall/syscall-ident"


let parse_syscall_pol ?opts x (format :Piqirun_ext.input_format) =
  let x_pb = Piqirun_ext.convert _syscall_pol_piqi_type format `pb x ?opts in
  let buf = Piqirun.init_from_string x_pb in
  Syscall_piqi.parse_syscall_pol buf

let parse_syscall_t ?opts x (format :Piqirun_ext.input_format) =
  let x_pb = Piqirun_ext.convert _syscall_t_piqi_type format `pb x ?opts in
  let buf = Piqirun.init_from_string x_pb in
  Syscall_piqi.parse_syscall_t buf

let parse_nr_read_pol ?opts x (format :Piqirun_ext.input_format) =
  let x_pb = Piqirun_ext.convert _nr_read_pol_piqi_type format `pb x ?opts in
  let buf = Piqirun.init_from_string x_pb in
  Syscall_piqi.parse_nr_read_pol buf

let parse_nr_open_t ?opts x (format :Piqirun_ext.input_format) =
  let x_pb = Piqirun_ext.convert _nr_open_t_piqi_type format `pb x ?opts in
  let buf = Piqirun.init_from_string x_pb in
  Syscall_piqi.parse_nr_open_t buf

let parse_nr_read_t ?opts x (format :Piqirun_ext.input_format) =
  let x_pb = Piqirun_ext.convert _nr_read_t_piqi_type format `pb x ?opts in
  let buf = Piqirun.init_from_string x_pb in
  Syscall_piqi.parse_nr_read_t buf

let parse_nr_write_t ?opts x (format :Piqirun_ext.input_format) =
  let x_pb = Piqirun_ext.convert _nr_write_t_piqi_type format `pb x ?opts in
  let buf = Piqirun.init_from_string x_pb in
  Syscall_piqi.parse_nr_write_t buf

let parse_sys_generic_t ?opts x (format :Piqirun_ext.input_format) =
  let x_pb = Piqirun_ext.convert _sys_generic_t_piqi_type format `pb x ?opts in
  let buf = Piqirun.init_from_string x_pb in
  Syscall_piqi.parse_sys_generic_t buf

let parse_syscall_ident ?opts x (format :Piqirun_ext.input_format) =
  let x_pb = Piqirun_ext.convert _syscall_ident_piqi_type format `pb x ?opts in
  let buf = Piqirun.init_from_string x_pb in
  Syscall_piqi.parse_syscall_ident buf


let gen_syscall_pol ?opts x (format :Piqirun_ext.output_format) =
  let buf = Syscall_piqi.gen_syscall_pol x in
  let x_pb = Piqirun.to_string buf in
  Piqirun_ext.convert _syscall_pol_piqi_type `pb format x_pb ?opts

let gen_syscall_t ?opts x (format :Piqirun_ext.output_format) =
  let buf = Syscall_piqi.gen_syscall_t x in
  let x_pb = Piqirun.to_string buf in
  Piqirun_ext.convert _syscall_t_piqi_type `pb format x_pb ?opts

let gen_nr_read_pol ?opts x (format :Piqirun_ext.output_format) =
  let buf = Syscall_piqi.gen_nr_read_pol x in
  let x_pb = Piqirun.to_string buf in
  Piqirun_ext.convert _nr_read_pol_piqi_type `pb format x_pb ?opts

let gen_nr_open_t ?opts x (format :Piqirun_ext.output_format) =
  let buf = Syscall_piqi.gen_nr_open_t x in
  let x_pb = Piqirun.to_string buf in
  Piqirun_ext.convert _nr_open_t_piqi_type `pb format x_pb ?opts

let gen_nr_read_t ?opts x (format :Piqirun_ext.output_format) =
  let buf = Syscall_piqi.gen_nr_read_t x in
  let x_pb = Piqirun.to_string buf in
  Piqirun_ext.convert _nr_read_t_piqi_type `pb format x_pb ?opts

let gen_nr_write_t ?opts x (format :Piqirun_ext.output_format) =
  let buf = Syscall_piqi.gen_nr_write_t x in
  let x_pb = Piqirun.to_string buf in
  Piqirun_ext.convert _nr_write_t_piqi_type `pb format x_pb ?opts

let gen_sys_generic_t ?opts x (format :Piqirun_ext.output_format) =
  let buf = Syscall_piqi.gen_sys_generic_t x in
  let x_pb = Piqirun.to_string buf in
  Piqirun_ext.convert _sys_generic_t_piqi_type `pb format x_pb ?opts

let gen_syscall_ident ?opts x (format :Piqirun_ext.output_format) =
  let buf = Syscall_piqi.gen_syscall_ident x in
  let x_pb = Piqirun.to_string buf in
  Piqirun_ext.convert _syscall_ident_piqi_type `pb format x_pb ?opts


let print_syscall_pol ?opts x =
  Stdlib.print_endline (gen_syscall_pol x `piq ?opts)
let prerr_syscall_pol ?opts x =
  Stdlib.prerr_endline (gen_syscall_pol x `piq ?opts)

let print_syscall_t ?opts x =
  Stdlib.print_endline (gen_syscall_t x `piq ?opts)
let prerr_syscall_t ?opts x =
  Stdlib.prerr_endline (gen_syscall_t x `piq ?opts)

let print_nr_read_pol ?opts x =
  Stdlib.print_endline (gen_nr_read_pol x `piq ?opts)
let prerr_nr_read_pol ?opts x =
  Stdlib.prerr_endline (gen_nr_read_pol x `piq ?opts)

let print_nr_open_t ?opts x =
  Stdlib.print_endline (gen_nr_open_t x `piq ?opts)
let prerr_nr_open_t ?opts x =
  Stdlib.prerr_endline (gen_nr_open_t x `piq ?opts)

let print_nr_read_t ?opts x =
  Stdlib.print_endline (gen_nr_read_t x `piq ?opts)
let prerr_nr_read_t ?opts x =
  Stdlib.prerr_endline (gen_nr_read_t x `piq ?opts)

let print_nr_write_t ?opts x =
  Stdlib.print_endline (gen_nr_write_t x `piq ?opts)
let prerr_nr_write_t ?opts x =
  Stdlib.prerr_endline (gen_nr_write_t x `piq ?opts)

let print_sys_generic_t ?opts x =
  Stdlib.print_endline (gen_sys_generic_t x `piq ?opts)
let prerr_sys_generic_t ?opts x =
  Stdlib.prerr_endline (gen_sys_generic_t x `piq ?opts)

let print_syscall_ident ?opts x =
  Stdlib.print_endline (gen_syscall_ident x `piq ?opts)
let prerr_syscall_ident ?opts x =
  Stdlib.prerr_endline (gen_syscall_ident x `piq ?opts)


