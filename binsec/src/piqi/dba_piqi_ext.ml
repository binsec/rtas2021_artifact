let piqi = Dba_piqi.piqi


let _ = Piqirun_ext.init_piqi piqi


let _bitvector_piqi_type = Piqirun_ext.find_piqi_type "dba/bitvector"
let _dbacodeaddress_piqi_type = Piqirun_ext.find_piqi_type "dba/dbacodeaddress"
let _codeaddress_piqi_type = Piqirun_ext.find_piqi_type "dba/codeaddress"
let _codeaddress_typeid_codeaddress_piqi_type = Piqirun_ext.find_piqi_type "dba/codeaddress-typeid-codeaddress"
let _dbatag_piqi_type = Piqirun_ext.find_piqi_type "dba/dbatag"
let _dbatag_typeid_dbatag_piqi_type = Piqirun_ext.find_piqi_type "dba/dbatag-typeid-dbatag"
let _dbastopstate_piqi_type = Piqirun_ext.find_piqi_type "dba/dbastopstate"
let _dbastopstate_typeid_dbastate_piqi_type = Piqirun_ext.find_piqi_type "dba/dbastopstate-typeid-dbastate"
let _dbaexpr_piqi_type = Piqirun_ext.find_piqi_type "dba/dbaexpr"
let _dbaexpr_typeid_dbaexpr_piqi_type = Piqirun_ext.find_piqi_type "dba/dbaexpr-typeid-dbaexpr"
let _dbaexpr_dbaunary_piqi_type = Piqirun_ext.find_piqi_type "dba/dbaexpr-dbaunary"
let _dbaexpr_dbabinary_piqi_type = Piqirun_ext.find_piqi_type "dba/dbaexpr-dbabinary"
let _dbacond_piqi_type = Piqirun_ext.find_piqi_type "dba/dbacond"
let _dbacond_typeid_dbacond_piqi_type = Piqirun_ext.find_piqi_type "dba/dbacond-typeid-dbacond"
let _dba_lhs_piqi_type = Piqirun_ext.find_piqi_type "dba/dbaLhs"
let _dba_lhs_typeid_dbalhs_piqi_type = Piqirun_ext.find_piqi_type "dba/dbaLhs-typeid-dbalhs"
let _dbainstr_piqi_type = Piqirun_ext.find_piqi_type "dba/dbainstr"
let _dbainstr_typeid_instrkind_piqi_type = Piqirun_ext.find_piqi_type "dba/dbainstr-typeid-instrkind"
let _dba_list_piqi_type = Piqirun_ext.find_piqi_type "dba/dba-list"
let _endianness_piqi_type = Piqirun_ext.find_piqi_type "dba/endianness"


let parse_bitvector ?opts x (format :Piqirun_ext.input_format) =
  let x_pb = Piqirun_ext.convert _bitvector_piqi_type format `pb x ?opts in
  let buf = Piqirun.init_from_string x_pb in
  Dba_piqi.parse_bitvector buf

let parse_dbacodeaddress ?opts x (format :Piqirun_ext.input_format) =
  let x_pb = Piqirun_ext.convert _dbacodeaddress_piqi_type format `pb x ?opts in
  let buf = Piqirun.init_from_string x_pb in
  Dba_piqi.parse_dbacodeaddress buf

let parse_codeaddress ?opts x (format :Piqirun_ext.input_format) =
  let x_pb = Piqirun_ext.convert _codeaddress_piqi_type format `pb x ?opts in
  let buf = Piqirun.init_from_string x_pb in
  Dba_piqi.parse_codeaddress buf

let parse_codeaddress_typeid_codeaddress ?opts x (format :Piqirun_ext.input_format) =
  let x_pb = Piqirun_ext.convert _codeaddress_typeid_codeaddress_piqi_type format `pb x ?opts in
  let buf = Piqirun.init_from_string x_pb in
  Dba_piqi.parse_codeaddress_typeid_codeaddress buf

let parse_dbatag ?opts x (format :Piqirun_ext.input_format) =
  let x_pb = Piqirun_ext.convert _dbatag_piqi_type format `pb x ?opts in
  let buf = Piqirun.init_from_string x_pb in
  Dba_piqi.parse_dbatag buf

let parse_dbatag_typeid_dbatag ?opts x (format :Piqirun_ext.input_format) =
  let x_pb = Piqirun_ext.convert _dbatag_typeid_dbatag_piqi_type format `pb x ?opts in
  let buf = Piqirun.init_from_string x_pb in
  Dba_piqi.parse_dbatag_typeid_dbatag buf

let parse_dbastopstate ?opts x (format :Piqirun_ext.input_format) =
  let x_pb = Piqirun_ext.convert _dbastopstate_piqi_type format `pb x ?opts in
  let buf = Piqirun.init_from_string x_pb in
  Dba_piqi.parse_dbastopstate buf

let parse_dbastopstate_typeid_dbastate ?opts x (format :Piqirun_ext.input_format) =
  let x_pb = Piqirun_ext.convert _dbastopstate_typeid_dbastate_piqi_type format `pb x ?opts in
  let buf = Piqirun.init_from_string x_pb in
  Dba_piqi.parse_dbastopstate_typeid_dbastate buf

let parse_dbaexpr ?opts x (format :Piqirun_ext.input_format) =
  let x_pb = Piqirun_ext.convert _dbaexpr_piqi_type format `pb x ?opts in
  let buf = Piqirun.init_from_string x_pb in
  Dba_piqi.parse_dbaexpr buf

let parse_dbaexpr_typeid_dbaexpr ?opts x (format :Piqirun_ext.input_format) =
  let x_pb = Piqirun_ext.convert _dbaexpr_typeid_dbaexpr_piqi_type format `pb x ?opts in
  let buf = Piqirun.init_from_string x_pb in
  Dba_piqi.parse_dbaexpr_typeid_dbaexpr buf

let parse_dbaexpr_dbaunary ?opts x (format :Piqirun_ext.input_format) =
  let x_pb = Piqirun_ext.convert _dbaexpr_dbaunary_piqi_type format `pb x ?opts in
  let buf = Piqirun.init_from_string x_pb in
  Dba_piqi.parse_dbaexpr_dbaunary buf

let parse_dbaexpr_dbabinary ?opts x (format :Piqirun_ext.input_format) =
  let x_pb = Piqirun_ext.convert _dbaexpr_dbabinary_piqi_type format `pb x ?opts in
  let buf = Piqirun.init_from_string x_pb in
  Dba_piqi.parse_dbaexpr_dbabinary buf

let parse_dbacond ?opts x (format :Piqirun_ext.input_format) =
  let x_pb = Piqirun_ext.convert _dbacond_piqi_type format `pb x ?opts in
  let buf = Piqirun.init_from_string x_pb in
  Dba_piqi.parse_dbacond buf

let parse_dbacond_typeid_dbacond ?opts x (format :Piqirun_ext.input_format) =
  let x_pb = Piqirun_ext.convert _dbacond_typeid_dbacond_piqi_type format `pb x ?opts in
  let buf = Piqirun.init_from_string x_pb in
  Dba_piqi.parse_dbacond_typeid_dbacond buf

let parse_dba_lhs ?opts x (format :Piqirun_ext.input_format) =
  let x_pb = Piqirun_ext.convert _dba_lhs_piqi_type format `pb x ?opts in
  let buf = Piqirun.init_from_string x_pb in
  Dba_piqi.parse_dba_lhs buf

let parse_dba_lhs_typeid_dbalhs ?opts x (format :Piqirun_ext.input_format) =
  let x_pb = Piqirun_ext.convert _dba_lhs_typeid_dbalhs_piqi_type format `pb x ?opts in
  let buf = Piqirun.init_from_string x_pb in
  Dba_piqi.parse_dba_lhs_typeid_dbalhs buf

let parse_dbainstr ?opts x (format :Piqirun_ext.input_format) =
  let x_pb = Piqirun_ext.convert _dbainstr_piqi_type format `pb x ?opts in
  let buf = Piqirun.init_from_string x_pb in
  Dba_piqi.parse_dbainstr buf

let parse_dbainstr_typeid_instrkind ?opts x (format :Piqirun_ext.input_format) =
  let x_pb = Piqirun_ext.convert _dbainstr_typeid_instrkind_piqi_type format `pb x ?opts in
  let buf = Piqirun.init_from_string x_pb in
  Dba_piqi.parse_dbainstr_typeid_instrkind buf

let parse_dba_list ?opts x (format :Piqirun_ext.input_format) =
  let x_pb = Piqirun_ext.convert _dba_list_piqi_type format `pb x ?opts in
  let buf = Piqirun.init_from_string x_pb in
  Dba_piqi.parse_dba_list buf

let parse_endianness ?opts x (format :Piqirun_ext.input_format) =
  let x_pb = Piqirun_ext.convert _endianness_piqi_type format `pb x ?opts in
  let buf = Piqirun.init_from_string x_pb in
  Dba_piqi.parse_endianness buf


let gen_bitvector ?opts x (format :Piqirun_ext.output_format) =
  let buf = Dba_piqi.gen_bitvector x in
  let x_pb = Piqirun.to_string buf in
  Piqirun_ext.convert _bitvector_piqi_type `pb format x_pb ?opts

let gen_dbacodeaddress ?opts x (format :Piqirun_ext.output_format) =
  let buf = Dba_piqi.gen_dbacodeaddress x in
  let x_pb = Piqirun.to_string buf in
  Piqirun_ext.convert _dbacodeaddress_piqi_type `pb format x_pb ?opts

let gen_codeaddress ?opts x (format :Piqirun_ext.output_format) =
  let buf = Dba_piqi.gen_codeaddress x in
  let x_pb = Piqirun.to_string buf in
  Piqirun_ext.convert _codeaddress_piqi_type `pb format x_pb ?opts

let gen_codeaddress_typeid_codeaddress ?opts x (format :Piqirun_ext.output_format) =
  let buf = Dba_piqi.gen_codeaddress_typeid_codeaddress x in
  let x_pb = Piqirun.to_string buf in
  Piqirun_ext.convert _codeaddress_typeid_codeaddress_piqi_type `pb format x_pb ?opts

let gen_dbatag ?opts x (format :Piqirun_ext.output_format) =
  let buf = Dba_piqi.gen_dbatag x in
  let x_pb = Piqirun.to_string buf in
  Piqirun_ext.convert _dbatag_piqi_type `pb format x_pb ?opts

let gen_dbatag_typeid_dbatag ?opts x (format :Piqirun_ext.output_format) =
  let buf = Dba_piqi.gen_dbatag_typeid_dbatag x in
  let x_pb = Piqirun.to_string buf in
  Piqirun_ext.convert _dbatag_typeid_dbatag_piqi_type `pb format x_pb ?opts

let gen_dbastopstate ?opts x (format :Piqirun_ext.output_format) =
  let buf = Dba_piqi.gen_dbastopstate x in
  let x_pb = Piqirun.to_string buf in
  Piqirun_ext.convert _dbastopstate_piqi_type `pb format x_pb ?opts

let gen_dbastopstate_typeid_dbastate ?opts x (format :Piqirun_ext.output_format) =
  let buf = Dba_piqi.gen_dbastopstate_typeid_dbastate x in
  let x_pb = Piqirun.to_string buf in
  Piqirun_ext.convert _dbastopstate_typeid_dbastate_piqi_type `pb format x_pb ?opts

let gen_dbaexpr ?opts x (format :Piqirun_ext.output_format) =
  let buf = Dba_piqi.gen_dbaexpr x in
  let x_pb = Piqirun.to_string buf in
  Piqirun_ext.convert _dbaexpr_piqi_type `pb format x_pb ?opts

let gen_dbaexpr_typeid_dbaexpr ?opts x (format :Piqirun_ext.output_format) =
  let buf = Dba_piqi.gen_dbaexpr_typeid_dbaexpr x in
  let x_pb = Piqirun.to_string buf in
  Piqirun_ext.convert _dbaexpr_typeid_dbaexpr_piqi_type `pb format x_pb ?opts

let gen_dbaexpr_dbaunary ?opts x (format :Piqirun_ext.output_format) =
  let buf = Dba_piqi.gen_dbaexpr_dbaunary x in
  let x_pb = Piqirun.to_string buf in
  Piqirun_ext.convert _dbaexpr_dbaunary_piqi_type `pb format x_pb ?opts

let gen_dbaexpr_dbabinary ?opts x (format :Piqirun_ext.output_format) =
  let buf = Dba_piqi.gen_dbaexpr_dbabinary x in
  let x_pb = Piqirun.to_string buf in
  Piqirun_ext.convert _dbaexpr_dbabinary_piqi_type `pb format x_pb ?opts

let gen_dbacond ?opts x (format :Piqirun_ext.output_format) =
  let buf = Dba_piqi.gen_dbacond x in
  let x_pb = Piqirun.to_string buf in
  Piqirun_ext.convert _dbacond_piqi_type `pb format x_pb ?opts

let gen_dbacond_typeid_dbacond ?opts x (format :Piqirun_ext.output_format) =
  let buf = Dba_piqi.gen_dbacond_typeid_dbacond x in
  let x_pb = Piqirun.to_string buf in
  Piqirun_ext.convert _dbacond_typeid_dbacond_piqi_type `pb format x_pb ?opts

let gen_dba_lhs ?opts x (format :Piqirun_ext.output_format) =
  let buf = Dba_piqi.gen_dba_lhs x in
  let x_pb = Piqirun.to_string buf in
  Piqirun_ext.convert _dba_lhs_piqi_type `pb format x_pb ?opts

let gen_dba_lhs_typeid_dbalhs ?opts x (format :Piqirun_ext.output_format) =
  let buf = Dba_piqi.gen_dba_lhs_typeid_dbalhs x in
  let x_pb = Piqirun.to_string buf in
  Piqirun_ext.convert _dba_lhs_typeid_dbalhs_piqi_type `pb format x_pb ?opts

let gen_dbainstr ?opts x (format :Piqirun_ext.output_format) =
  let buf = Dba_piqi.gen_dbainstr x in
  let x_pb = Piqirun.to_string buf in
  Piqirun_ext.convert _dbainstr_piqi_type `pb format x_pb ?opts

let gen_dbainstr_typeid_instrkind ?opts x (format :Piqirun_ext.output_format) =
  let buf = Dba_piqi.gen_dbainstr_typeid_instrkind x in
  let x_pb = Piqirun.to_string buf in
  Piqirun_ext.convert _dbainstr_typeid_instrkind_piqi_type `pb format x_pb ?opts

let gen_dba_list ?opts x (format :Piqirun_ext.output_format) =
  let buf = Dba_piqi.gen_dba_list x in
  let x_pb = Piqirun.to_string buf in
  Piqirun_ext.convert _dba_list_piqi_type `pb format x_pb ?opts

let gen_endianness ?opts x (format :Piqirun_ext.output_format) =
  let buf = Dba_piqi.gen_endianness x in
  let x_pb = Piqirun.to_string buf in
  Piqirun_ext.convert _endianness_piqi_type `pb format x_pb ?opts


let print_bitvector ?opts x =
  Stdlib.print_endline (gen_bitvector x `piq ?opts)
let prerr_bitvector ?opts x =
  Stdlib.prerr_endline (gen_bitvector x `piq ?opts)

let print_dbacodeaddress ?opts x =
  Stdlib.print_endline (gen_dbacodeaddress x `piq ?opts)
let prerr_dbacodeaddress ?opts x =
  Stdlib.prerr_endline (gen_dbacodeaddress x `piq ?opts)

let print_codeaddress ?opts x =
  Stdlib.print_endline (gen_codeaddress x `piq ?opts)
let prerr_codeaddress ?opts x =
  Stdlib.prerr_endline (gen_codeaddress x `piq ?opts)

let print_codeaddress_typeid_codeaddress ?opts x =
  Stdlib.print_endline (gen_codeaddress_typeid_codeaddress x `piq ?opts)
let prerr_codeaddress_typeid_codeaddress ?opts x =
  Stdlib.prerr_endline (gen_codeaddress_typeid_codeaddress x `piq ?opts)

let print_dbatag ?opts x =
  Stdlib.print_endline (gen_dbatag x `piq ?opts)
let prerr_dbatag ?opts x =
  Stdlib.prerr_endline (gen_dbatag x `piq ?opts)

let print_dbatag_typeid_dbatag ?opts x =
  Stdlib.print_endline (gen_dbatag_typeid_dbatag x `piq ?opts)
let prerr_dbatag_typeid_dbatag ?opts x =
  Stdlib.prerr_endline (gen_dbatag_typeid_dbatag x `piq ?opts)

let print_dbastopstate ?opts x =
  Stdlib.print_endline (gen_dbastopstate x `piq ?opts)
let prerr_dbastopstate ?opts x =
  Stdlib.prerr_endline (gen_dbastopstate x `piq ?opts)

let print_dbastopstate_typeid_dbastate ?opts x =
  Stdlib.print_endline (gen_dbastopstate_typeid_dbastate x `piq ?opts)
let prerr_dbastopstate_typeid_dbastate ?opts x =
  Stdlib.prerr_endline (gen_dbastopstate_typeid_dbastate x `piq ?opts)

let print_dbaexpr ?opts x =
  Stdlib.print_endline (gen_dbaexpr x `piq ?opts)
let prerr_dbaexpr ?opts x =
  Stdlib.prerr_endline (gen_dbaexpr x `piq ?opts)

let print_dbaexpr_typeid_dbaexpr ?opts x =
  Stdlib.print_endline (gen_dbaexpr_typeid_dbaexpr x `piq ?opts)
let prerr_dbaexpr_typeid_dbaexpr ?opts x =
  Stdlib.prerr_endline (gen_dbaexpr_typeid_dbaexpr x `piq ?opts)

let print_dbaexpr_dbaunary ?opts x =
  Stdlib.print_endline (gen_dbaexpr_dbaunary x `piq ?opts)
let prerr_dbaexpr_dbaunary ?opts x =
  Stdlib.prerr_endline (gen_dbaexpr_dbaunary x `piq ?opts)

let print_dbaexpr_dbabinary ?opts x =
  Stdlib.print_endline (gen_dbaexpr_dbabinary x `piq ?opts)
let prerr_dbaexpr_dbabinary ?opts x =
  Stdlib.prerr_endline (gen_dbaexpr_dbabinary x `piq ?opts)

let print_dbacond ?opts x =
  Stdlib.print_endline (gen_dbacond x `piq ?opts)
let prerr_dbacond ?opts x =
  Stdlib.prerr_endline (gen_dbacond x `piq ?opts)

let print_dbacond_typeid_dbacond ?opts x =
  Stdlib.print_endline (gen_dbacond_typeid_dbacond x `piq ?opts)
let prerr_dbacond_typeid_dbacond ?opts x =
  Stdlib.prerr_endline (gen_dbacond_typeid_dbacond x `piq ?opts)

let print_dba_lhs ?opts x =
  Stdlib.print_endline (gen_dba_lhs x `piq ?opts)
let prerr_dba_lhs ?opts x =
  Stdlib.prerr_endline (gen_dba_lhs x `piq ?opts)

let print_dba_lhs_typeid_dbalhs ?opts x =
  Stdlib.print_endline (gen_dba_lhs_typeid_dbalhs x `piq ?opts)
let prerr_dba_lhs_typeid_dbalhs ?opts x =
  Stdlib.prerr_endline (gen_dba_lhs_typeid_dbalhs x `piq ?opts)

let print_dbainstr ?opts x =
  Stdlib.print_endline (gen_dbainstr x `piq ?opts)
let prerr_dbainstr ?opts x =
  Stdlib.prerr_endline (gen_dbainstr x `piq ?opts)

let print_dbainstr_typeid_instrkind ?opts x =
  Stdlib.print_endline (gen_dbainstr_typeid_instrkind x `piq ?opts)
let prerr_dbainstr_typeid_instrkind ?opts x =
  Stdlib.prerr_endline (gen_dbainstr_typeid_instrkind x `piq ?opts)

let print_dba_list ?opts x =
  Stdlib.print_endline (gen_dba_list x `piq ?opts)
let prerr_dba_list ?opts x =
  Stdlib.prerr_endline (gen_dba_list x `piq ?opts)

let print_endianness ?opts x =
  Stdlib.print_endline (gen_endianness x `piq ?opts)
let prerr_endianness ?opts x =
  Stdlib.prerr_endline (gen_endianness x `piq ?opts)


