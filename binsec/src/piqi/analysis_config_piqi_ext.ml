module Common_ext = Common_piqi_ext
let piqi = Analysis_config_piqi.piqi


let _ = Piqirun_ext.init_piqi piqi


let _specific_parameters_t_piqi_type = Piqirun_ext.find_piqi_type "analysis_config/specific-parameters-t"
let _specific_parameters_t_analyse_type_piqi_type = Piqirun_ext.find_piqi_type "analysis_config/specific-parameters-t-analyse-type"
let _standard_analysis_piqi_type = Piqirun_ext.find_piqi_type "analysis_config/standard-analysis"
let _generic_analysis_piqi_type = Piqirun_ext.find_piqi_type "analysis_config/generic-analysis"
let _generic_analysis_query_type_piqi_type = Piqirun_ext.find_piqi_type "analysis_config/generic-analysis-query-type"
let _generic_analysis_results_piqi_type = Piqirun_ext.find_piqi_type "analysis_config/generic-analysis-results"
let _callret_analysis_results_piqi_type = Piqirun_ext.find_piqi_type "analysis_config/callret-analysis-results"
let _callret_analysis_results_call_data_piqi_type = Piqirun_ext.find_piqi_type "analysis_config/callret-analysis-results-call-data"
let _callret_analysis_results_ret_data_piqi_type = Piqirun_ext.find_piqi_type "analysis_config/callret-analysis-results-ret-data"
let _callret_analysis_results_callret_labels_piqi_type = Piqirun_ext.find_piqi_type "analysis_config/callret-analysis-results-callret-labels"
let _callret_analysis_results_callret_status_piqi_type = Piqirun_ext.find_piqi_type "analysis_config/callret-analysis-results-callret-status"
let _po_analysis_results_piqi_type = Piqirun_ext.find_piqi_type "analysis_config/po-analysis-results"
let _po_analysis_results_po_data_piqi_type = Piqirun_ext.find_piqi_type "analysis_config/po-analysis-results-po-data"
let _po_analysis_results_po_status_piqi_type = Piqirun_ext.find_piqi_type "analysis_config/po-analysis-results-po-status"


let parse_specific_parameters_t ?opts x (format :Piqirun_ext.input_format) =
  let x_pb = Piqirun_ext.convert _specific_parameters_t_piqi_type format `pb x ?opts in
  let buf = Piqirun.init_from_string x_pb in
  Analysis_config_piqi.parse_specific_parameters_t buf

let parse_specific_parameters_t_analyse_type ?opts x (format :Piqirun_ext.input_format) =
  let x_pb = Piqirun_ext.convert _specific_parameters_t_analyse_type_piqi_type format `pb x ?opts in
  let buf = Piqirun.init_from_string x_pb in
  Analysis_config_piqi.parse_specific_parameters_t_analyse_type buf

let parse_standard_analysis ?opts x (format :Piqirun_ext.input_format) =
  let x_pb = Piqirun_ext.convert _standard_analysis_piqi_type format `pb x ?opts in
  let buf = Piqirun.init_from_string x_pb in
  Analysis_config_piqi.parse_standard_analysis buf

let parse_generic_analysis ?opts x (format :Piqirun_ext.input_format) =
  let x_pb = Piqirun_ext.convert _generic_analysis_piqi_type format `pb x ?opts in
  let buf = Piqirun.init_from_string x_pb in
  Analysis_config_piqi.parse_generic_analysis buf

let parse_generic_analysis_query_type ?opts x (format :Piqirun_ext.input_format) =
  let x_pb = Piqirun_ext.convert _generic_analysis_query_type_piqi_type format `pb x ?opts in
  let buf = Piqirun.init_from_string x_pb in
  Analysis_config_piqi.parse_generic_analysis_query_type buf

let parse_generic_analysis_results ?opts x (format :Piqirun_ext.input_format) =
  let x_pb = Piqirun_ext.convert _generic_analysis_results_piqi_type format `pb x ?opts in
  let buf = Piqirun.init_from_string x_pb in
  Analysis_config_piqi.parse_generic_analysis_results buf

let parse_callret_analysis_results ?opts x (format :Piqirun_ext.input_format) =
  let x_pb = Piqirun_ext.convert _callret_analysis_results_piqi_type format `pb x ?opts in
  let buf = Piqirun.init_from_string x_pb in
  Analysis_config_piqi.parse_callret_analysis_results buf

let parse_callret_analysis_results_call_data ?opts x (format :Piqirun_ext.input_format) =
  let x_pb = Piqirun_ext.convert _callret_analysis_results_call_data_piqi_type format `pb x ?opts in
  let buf = Piqirun.init_from_string x_pb in
  Analysis_config_piqi.parse_callret_analysis_results_call_data buf

let parse_callret_analysis_results_ret_data ?opts x (format :Piqirun_ext.input_format) =
  let x_pb = Piqirun_ext.convert _callret_analysis_results_ret_data_piqi_type format `pb x ?opts in
  let buf = Piqirun.init_from_string x_pb in
  Analysis_config_piqi.parse_callret_analysis_results_ret_data buf

let parse_callret_analysis_results_callret_labels ?opts x (format :Piqirun_ext.input_format) =
  let x_pb = Piqirun_ext.convert _callret_analysis_results_callret_labels_piqi_type format `pb x ?opts in
  let buf = Piqirun.init_from_string x_pb in
  Analysis_config_piqi.parse_callret_analysis_results_callret_labels buf

let parse_callret_analysis_results_callret_status ?opts x (format :Piqirun_ext.input_format) =
  let x_pb = Piqirun_ext.convert _callret_analysis_results_callret_status_piqi_type format `pb x ?opts in
  let buf = Piqirun.init_from_string x_pb in
  Analysis_config_piqi.parse_callret_analysis_results_callret_status buf

let parse_po_analysis_results ?opts x (format :Piqirun_ext.input_format) =
  let x_pb = Piqirun_ext.convert _po_analysis_results_piqi_type format `pb x ?opts in
  let buf = Piqirun.init_from_string x_pb in
  Analysis_config_piqi.parse_po_analysis_results buf

let parse_po_analysis_results_po_data ?opts x (format :Piqirun_ext.input_format) =
  let x_pb = Piqirun_ext.convert _po_analysis_results_po_data_piqi_type format `pb x ?opts in
  let buf = Piqirun.init_from_string x_pb in
  Analysis_config_piqi.parse_po_analysis_results_po_data buf

let parse_po_analysis_results_po_status ?opts x (format :Piqirun_ext.input_format) =
  let x_pb = Piqirun_ext.convert _po_analysis_results_po_status_piqi_type format `pb x ?opts in
  let buf = Piqirun.init_from_string x_pb in
  Analysis_config_piqi.parse_po_analysis_results_po_status buf


let gen_specific_parameters_t ?opts x (format :Piqirun_ext.output_format) =
  let buf = Analysis_config_piqi.gen_specific_parameters_t x in
  let x_pb = Piqirun.to_string buf in
  Piqirun_ext.convert _specific_parameters_t_piqi_type `pb format x_pb ?opts

let gen_specific_parameters_t_analyse_type ?opts x (format :Piqirun_ext.output_format) =
  let buf = Analysis_config_piqi.gen_specific_parameters_t_analyse_type x in
  let x_pb = Piqirun.to_string buf in
  Piqirun_ext.convert _specific_parameters_t_analyse_type_piqi_type `pb format x_pb ?opts

let gen_standard_analysis ?opts x (format :Piqirun_ext.output_format) =
  let buf = Analysis_config_piqi.gen_standard_analysis x in
  let x_pb = Piqirun.to_string buf in
  Piqirun_ext.convert _standard_analysis_piqi_type `pb format x_pb ?opts

let gen_generic_analysis ?opts x (format :Piqirun_ext.output_format) =
  let buf = Analysis_config_piqi.gen_generic_analysis x in
  let x_pb = Piqirun.to_string buf in
  Piqirun_ext.convert _generic_analysis_piqi_type `pb format x_pb ?opts

let gen_generic_analysis_query_type ?opts x (format :Piqirun_ext.output_format) =
  let buf = Analysis_config_piqi.gen_generic_analysis_query_type x in
  let x_pb = Piqirun.to_string buf in
  Piqirun_ext.convert _generic_analysis_query_type_piqi_type `pb format x_pb ?opts

let gen_generic_analysis_results ?opts x (format :Piqirun_ext.output_format) =
  let buf = Analysis_config_piqi.gen_generic_analysis_results x in
  let x_pb = Piqirun.to_string buf in
  Piqirun_ext.convert _generic_analysis_results_piqi_type `pb format x_pb ?opts

let gen_callret_analysis_results ?opts x (format :Piqirun_ext.output_format) =
  let buf = Analysis_config_piqi.gen_callret_analysis_results x in
  let x_pb = Piqirun.to_string buf in
  Piqirun_ext.convert _callret_analysis_results_piqi_type `pb format x_pb ?opts

let gen_callret_analysis_results_call_data ?opts x (format :Piqirun_ext.output_format) =
  let buf = Analysis_config_piqi.gen_callret_analysis_results_call_data x in
  let x_pb = Piqirun.to_string buf in
  Piqirun_ext.convert _callret_analysis_results_call_data_piqi_type `pb format x_pb ?opts

let gen_callret_analysis_results_ret_data ?opts x (format :Piqirun_ext.output_format) =
  let buf = Analysis_config_piqi.gen_callret_analysis_results_ret_data x in
  let x_pb = Piqirun.to_string buf in
  Piqirun_ext.convert _callret_analysis_results_ret_data_piqi_type `pb format x_pb ?opts

let gen_callret_analysis_results_callret_labels ?opts x (format :Piqirun_ext.output_format) =
  let buf = Analysis_config_piqi.gen_callret_analysis_results_callret_labels x in
  let x_pb = Piqirun.to_string buf in
  Piqirun_ext.convert _callret_analysis_results_callret_labels_piqi_type `pb format x_pb ?opts

let gen_callret_analysis_results_callret_status ?opts x (format :Piqirun_ext.output_format) =
  let buf = Analysis_config_piqi.gen_callret_analysis_results_callret_status x in
  let x_pb = Piqirun.to_string buf in
  Piqirun_ext.convert _callret_analysis_results_callret_status_piqi_type `pb format x_pb ?opts

let gen_po_analysis_results ?opts x (format :Piqirun_ext.output_format) =
  let buf = Analysis_config_piqi.gen_po_analysis_results x in
  let x_pb = Piqirun.to_string buf in
  Piqirun_ext.convert _po_analysis_results_piqi_type `pb format x_pb ?opts

let gen_po_analysis_results_po_data ?opts x (format :Piqirun_ext.output_format) =
  let buf = Analysis_config_piqi.gen_po_analysis_results_po_data x in
  let x_pb = Piqirun.to_string buf in
  Piqirun_ext.convert _po_analysis_results_po_data_piqi_type `pb format x_pb ?opts

let gen_po_analysis_results_po_status ?opts x (format :Piqirun_ext.output_format) =
  let buf = Analysis_config_piqi.gen_po_analysis_results_po_status x in
  let x_pb = Piqirun.to_string buf in
  Piqirun_ext.convert _po_analysis_results_po_status_piqi_type `pb format x_pb ?opts


let print_specific_parameters_t ?opts x =
  Stdlib.print_endline (gen_specific_parameters_t x `piq ?opts)
let prerr_specific_parameters_t ?opts x =
  Stdlib.prerr_endline (gen_specific_parameters_t x `piq ?opts)

let print_specific_parameters_t_analyse_type ?opts x =
  Stdlib.print_endline (gen_specific_parameters_t_analyse_type x `piq ?opts)
let prerr_specific_parameters_t_analyse_type ?opts x =
  Stdlib.prerr_endline (gen_specific_parameters_t_analyse_type x `piq ?opts)

let print_standard_analysis ?opts x =
  Stdlib.print_endline (gen_standard_analysis x `piq ?opts)
let prerr_standard_analysis ?opts x =
  Stdlib.prerr_endline (gen_standard_analysis x `piq ?opts)

let print_generic_analysis ?opts x =
  Stdlib.print_endline (gen_generic_analysis x `piq ?opts)
let prerr_generic_analysis ?opts x =
  Stdlib.prerr_endline (gen_generic_analysis x `piq ?opts)

let print_generic_analysis_query_type ?opts x =
  Stdlib.print_endline (gen_generic_analysis_query_type x `piq ?opts)
let prerr_generic_analysis_query_type ?opts x =
  Stdlib.prerr_endline (gen_generic_analysis_query_type x `piq ?opts)

let print_generic_analysis_results ?opts x =
  Stdlib.print_endline (gen_generic_analysis_results x `piq ?opts)
let prerr_generic_analysis_results ?opts x =
  Stdlib.prerr_endline (gen_generic_analysis_results x `piq ?opts)

let print_callret_analysis_results ?opts x =
  Stdlib.print_endline (gen_callret_analysis_results x `piq ?opts)
let prerr_callret_analysis_results ?opts x =
  Stdlib.prerr_endline (gen_callret_analysis_results x `piq ?opts)

let print_callret_analysis_results_call_data ?opts x =
  Stdlib.print_endline (gen_callret_analysis_results_call_data x `piq ?opts)
let prerr_callret_analysis_results_call_data ?opts x =
  Stdlib.prerr_endline (gen_callret_analysis_results_call_data x `piq ?opts)

let print_callret_analysis_results_ret_data ?opts x =
  Stdlib.print_endline (gen_callret_analysis_results_ret_data x `piq ?opts)
let prerr_callret_analysis_results_ret_data ?opts x =
  Stdlib.prerr_endline (gen_callret_analysis_results_ret_data x `piq ?opts)

let print_callret_analysis_results_callret_labels ?opts x =
  Stdlib.print_endline (gen_callret_analysis_results_callret_labels x `piq ?opts)
let prerr_callret_analysis_results_callret_labels ?opts x =
  Stdlib.prerr_endline (gen_callret_analysis_results_callret_labels x `piq ?opts)

let print_callret_analysis_results_callret_status ?opts x =
  Stdlib.print_endline (gen_callret_analysis_results_callret_status x `piq ?opts)
let prerr_callret_analysis_results_callret_status ?opts x =
  Stdlib.prerr_endline (gen_callret_analysis_results_callret_status x `piq ?opts)

let print_po_analysis_results ?opts x =
  Stdlib.print_endline (gen_po_analysis_results x `piq ?opts)
let prerr_po_analysis_results ?opts x =
  Stdlib.prerr_endline (gen_po_analysis_results x `piq ?opts)

let print_po_analysis_results_po_data ?opts x =
  Stdlib.print_endline (gen_po_analysis_results_po_data x `piq ?opts)
let prerr_po_analysis_results_po_data ?opts x =
  Stdlib.prerr_endline (gen_po_analysis_results_po_data x `piq ?opts)

let print_po_analysis_results_po_status ?opts x =
  Stdlib.print_endline (gen_po_analysis_results_po_status x `piq ?opts)
let prerr_po_analysis_results_po_status ?opts x =
  Stdlib.prerr_endline (gen_po_analysis_results_po_status x `piq ?opts)


