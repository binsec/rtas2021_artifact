module  Common = Common_piqi

module rec Analysis_config_piqi:
  sig
    type uint64 = int64
    type uint32 = int32
    type float32 = float
    type specific_parameters_t_analyse_type =
      [
        | `none
        | `generic
        | `standard
      ]
    type generic_analysis_query_type =
      [
        | `satisfiability
        | `values
      ]
    type callret_analysis_results_callret_labels =
      [
        | `violable
        | `aligned
        | `disaligned
        | `can_return
        | `single
        | `multiple
        | `strong
        | `weak
        | `solver_wrong
        | `no_call
        | `has_returned
      ]
    type callret_analysis_results_callret_status =
      [
        | `ok
        | `viol
      ]
    type po_analysis_results_po_status =
      [
        | `unknown
        | `not_opaque
        | `opaque
        | `likely
      ]
    type specific_parameters_t = Specific_parameters_t.t
    type standard_analysis = Standard_analysis.t
    type generic_analysis = Generic_analysis.t
    type generic_analysis_results = Generic_analysis_results.t
    type callret_analysis_results = Callret_analysis_results.t
    type callret_analysis_results_call_data = Callret_analysis_results_call_data.t
    type callret_analysis_results_ret_data = Callret_analysis_results_ret_data.t
    type po_analysis_results = Po_analysis_results.t
    type po_analysis_results_po_data = Po_analysis_results_po_data.t
  end = Analysis_config_piqi
and Specific_parameters_t:
  sig
    type t = {
      mutable typeid: Analysis_config_piqi.specific_parameters_t_analyse_type;
      mutable generic_params: Analysis_config_piqi.generic_analysis option;
      mutable standard_params: Analysis_config_piqi.standard_analysis option;
    }
  end = Specific_parameters_t
and Standard_analysis:
  sig
    type t = {
      mutable target_addr: Analysis_config_piqi.uint64 option;
      mutable uniq: bool option;
      mutable get_formula: bool option;
    }
  end = Standard_analysis
and Generic_analysis:
  sig
    type t = {
      mutable kind: Analysis_config_piqi.generic_analysis_query_type;
      mutable target_addr: Analysis_config_piqi.uint64;
      mutable dba: string;
      mutable limit_values: Analysis_config_piqi.uint32 option;
      mutable get_formula: bool option;
      mutable from_addr: Analysis_config_piqi.uint64 option;
      mutable to_addr: Analysis_config_piqi.uint64 option;
      mutable restrict_values_from: Analysis_config_piqi.uint64 option;
      mutable restrict_values_to: Analysis_config_piqi.uint64 option;
    }
  end = Generic_analysis
and Generic_analysis_results:
  sig
    type t = {
      mutable result: Common.smt_result;
      mutable values: Analysis_config_piqi.uint64 list;
      mutable smt_formula: string option;
    }
  end = Generic_analysis_results
and Callret_analysis_results:
  sig
    type t = {
      mutable values: Analysis_config_piqi.callret_analysis_results_ret_data list;
    }
  end = Callret_analysis_results
and Callret_analysis_results_call_data:
  sig
    type t = {
      mutable addr: Analysis_config_piqi.uint64;
      mutable status: Analysis_config_piqi.callret_analysis_results_callret_status;
    }
  end = Callret_analysis_results_call_data
and Callret_analysis_results_ret_data:
  sig
    type t = {
      mutable ret_addr: Analysis_config_piqi.uint64;
      mutable status: Analysis_config_piqi.callret_analysis_results_callret_status;
      mutable labels: Analysis_config_piqi.callret_analysis_results_callret_labels list;
      mutable returnsites: Analysis_config_piqi.uint64 list;
      mutable solve_count: Analysis_config_piqi.uint32;
      mutable calls: Analysis_config_piqi.callret_analysis_results_call_data list;
    }
  end = Callret_analysis_results_ret_data
and Po_analysis_results:
  sig
    type t = {
      mutable values: Analysis_config_piqi.po_analysis_results_po_data list;
    }
  end = Po_analysis_results
and Po_analysis_results_po_data:
  sig
    type t = {
      mutable jmp_addr: Analysis_config_piqi.uint64;
      mutable status: Analysis_config_piqi.po_analysis_results_po_status;
      mutable ksteps: Analysis_config_piqi.uint32;
      mutable computation_time: Analysis_config_piqi.float32;
      mutable nb_paths: Analysis_config_piqi.uint32 option;
      mutable alive_branch: Analysis_config_piqi.uint64 option;
      mutable formula: string option;
    }
  end = Po_analysis_results_po_data


let rec parse_uint64 x = Piqirun.int64_of_varint x
and packed_parse_uint64 x = Piqirun.int64_of_packed_varint x

and parse_bool x = Piqirun.bool_of_varint x
and packed_parse_bool x = Piqirun.bool_of_packed_varint x

and parse_string x = Piqirun.string_of_block x

and parse_uint32 x = Piqirun.int32_of_varint x
and packed_parse_uint32 x = Piqirun.int32_of_packed_varint x

and parse_float32 x = Piqirun.float_of_fixed32 x
and packed_parse_float32 x = Piqirun.float_of_packed_fixed32 x

and parse_specific_parameters_t x =
  let x = Piqirun.parse_record x in
  let _typeid, x = Piqirun.parse_required_field 1 parse_specific_parameters_t_analyse_type x in
  let _generic_params, x = Piqirun.parse_optional_field 2 parse_generic_analysis x in
  let _standard_params, x = Piqirun.parse_optional_field 3 parse_standard_analysis x in
  Piqirun.check_unparsed_fields x;
  {
    Specific_parameters_t.typeid = _typeid;
    Specific_parameters_t.generic_params = _generic_params;
    Specific_parameters_t.standard_params = _standard_params;
  }

and parse_specific_parameters_t_analyse_type x =
  match Piqirun.int32_of_signed_varint x with
    | 0l -> `none
    | 1l -> `generic
    | 2l -> `standard
    | x -> Piqirun.error_enum_const x
and packed_parse_specific_parameters_t_analyse_type x =
  match Piqirun.int32_of_packed_signed_varint x with
    | 0l -> `none
    | 1l -> `generic
    | 2l -> `standard
    | x -> Piqirun.error_enum_const x

and parse_standard_analysis x =
  let x = Piqirun.parse_record x in
  let _target_addr, x = Piqirun.parse_optional_field 1 parse_uint64 x in
  let _uniq, x = Piqirun.parse_optional_field 2 parse_bool x in
  let _get_formula, x = Piqirun.parse_optional_field 3 parse_bool x in
  Piqirun.check_unparsed_fields x;
  {
    Standard_analysis.target_addr = _target_addr;
    Standard_analysis.uniq = _uniq;
    Standard_analysis.get_formula = _get_formula;
  }

and parse_generic_analysis x =
  let x = Piqirun.parse_record x in
  let _kind, x = Piqirun.parse_required_field 1 parse_generic_analysis_query_type x in
  let _target_addr, x = Piqirun.parse_required_field 2 parse_uint64 x in
  let _dba, x = Piqirun.parse_required_field 3 parse_string x in
  let _limit_values, x = Piqirun.parse_optional_field 4 parse_uint32 x in
  let _get_formula, x = Piqirun.parse_optional_field 5 parse_bool x in
  let _from_addr, x = Piqirun.parse_optional_field 6 parse_uint64 x in
  let _to_addr, x = Piqirun.parse_optional_field 7 parse_uint64 x in
  let _restrict_values_from, x = Piqirun.parse_optional_field 8 parse_uint64 x in
  let _restrict_values_to, x = Piqirun.parse_optional_field 9 parse_uint64 x in
  Piqirun.check_unparsed_fields x;
  {
    Generic_analysis.kind = _kind;
    Generic_analysis.target_addr = _target_addr;
    Generic_analysis.dba = _dba;
    Generic_analysis.limit_values = _limit_values;
    Generic_analysis.get_formula = _get_formula;
    Generic_analysis.from_addr = _from_addr;
    Generic_analysis.to_addr = _to_addr;
    Generic_analysis.restrict_values_from = _restrict_values_from;
    Generic_analysis.restrict_values_to = _restrict_values_to;
  }

and parse_generic_analysis_query_type x =
  match Piqirun.int32_of_signed_varint x with
    | 0l -> `satisfiability
    | 1l -> `values
    | x -> Piqirun.error_enum_const x
and packed_parse_generic_analysis_query_type x =
  match Piqirun.int32_of_packed_signed_varint x with
    | 0l -> `satisfiability
    | 1l -> `values
    | x -> Piqirun.error_enum_const x

and parse_generic_analysis_results x =
  let x = Piqirun.parse_record x in
  let _result, x = Piqirun.parse_required_field 1 Common.parse_smt_result x in
  let _values, x = Piqirun.parse_repeated_field 2 parse_uint64 x in
  let _smt_formula, x = Piqirun.parse_optional_field 3 parse_string x in
  Piqirun.check_unparsed_fields x;
  {
    Generic_analysis_results.result = _result;
    Generic_analysis_results.values = _values;
    Generic_analysis_results.smt_formula = _smt_formula;
  }

and parse_callret_analysis_results x =
  let x = Piqirun.parse_record x in
  let _values, x = Piqirun.parse_repeated_field 1 parse_callret_analysis_results_ret_data x in
  Piqirun.check_unparsed_fields x;
  {
    Callret_analysis_results.values = _values;
  }

and parse_callret_analysis_results_call_data x =
  let x = Piqirun.parse_record x in
  let _addr, x = Piqirun.parse_required_field 1 parse_uint64 x in
  let _status, x = Piqirun.parse_required_field 2 parse_callret_analysis_results_callret_status x in
  Piqirun.check_unparsed_fields x;
  {
    Callret_analysis_results_call_data.addr = _addr;
    Callret_analysis_results_call_data.status = _status;
  }

and parse_callret_analysis_results_ret_data x =
  let x = Piqirun.parse_record x in
  let _ret_addr, x = Piqirun.parse_required_field 1 parse_uint64 x in
  let _status, x = Piqirun.parse_required_field 2 parse_callret_analysis_results_callret_status x in
  let _labels, x = Piqirun.parse_repeated_field 3 parse_callret_analysis_results_callret_labels x in
  let _returnsites, x = Piqirun.parse_repeated_field 4 parse_uint64 x in
  let _solve_count, x = Piqirun.parse_required_field 5 parse_uint32 x in
  let _calls, x = Piqirun.parse_repeated_field 6 parse_callret_analysis_results_call_data x in
  Piqirun.check_unparsed_fields x;
  {
    Callret_analysis_results_ret_data.ret_addr = _ret_addr;
    Callret_analysis_results_ret_data.status = _status;
    Callret_analysis_results_ret_data.labels = _labels;
    Callret_analysis_results_ret_data.returnsites = _returnsites;
    Callret_analysis_results_ret_data.solve_count = _solve_count;
    Callret_analysis_results_ret_data.calls = _calls;
  }

and parse_callret_analysis_results_callret_labels x =
  match Piqirun.int32_of_signed_varint x with
    | 1l -> `violable
    | 2l -> `aligned
    | 3l -> `disaligned
    | 4l -> `can_return
    | 5l -> `single
    | 6l -> `multiple
    | 7l -> `strong
    | 8l -> `weak
    | 9l -> `solver_wrong
    | 10l -> `no_call
    | 11l -> `has_returned
    | x -> Piqirun.error_enum_const x
and packed_parse_callret_analysis_results_callret_labels x =
  match Piqirun.int32_of_packed_signed_varint x with
    | 1l -> `violable
    | 2l -> `aligned
    | 3l -> `disaligned
    | 4l -> `can_return
    | 5l -> `single
    | 6l -> `multiple
    | 7l -> `strong
    | 8l -> `weak
    | 9l -> `solver_wrong
    | 10l -> `no_call
    | 11l -> `has_returned
    | x -> Piqirun.error_enum_const x

and parse_callret_analysis_results_callret_status x =
  match Piqirun.int32_of_signed_varint x with
    | 1l -> `ok
    | 2l -> `viol
    | x -> Piqirun.error_enum_const x
and packed_parse_callret_analysis_results_callret_status x =
  match Piqirun.int32_of_packed_signed_varint x with
    | 1l -> `ok
    | 2l -> `viol
    | x -> Piqirun.error_enum_const x

and parse_po_analysis_results x =
  let x = Piqirun.parse_record x in
  let _values, x = Piqirun.parse_repeated_field 1 parse_po_analysis_results_po_data x in
  Piqirun.check_unparsed_fields x;
  {
    Po_analysis_results.values = _values;
  }

and parse_po_analysis_results_po_data x =
  let x = Piqirun.parse_record x in
  let _jmp_addr, x = Piqirun.parse_required_field 1 parse_uint64 x in
  let _status, x = Piqirun.parse_required_field 2 parse_po_analysis_results_po_status x in
  let _ksteps, x = Piqirun.parse_required_field 3 parse_uint32 x in
  let _computation_time, x = Piqirun.parse_required_field 4 parse_float32 x in
  let _nb_paths, x = Piqirun.parse_optional_field 5 parse_uint32 x in
  let _alive_branch, x = Piqirun.parse_optional_field 6 parse_uint64 x in
  let _formula, x = Piqirun.parse_optional_field 7 parse_string x in
  Piqirun.check_unparsed_fields x;
  {
    Po_analysis_results_po_data.jmp_addr = _jmp_addr;
    Po_analysis_results_po_data.status = _status;
    Po_analysis_results_po_data.ksteps = _ksteps;
    Po_analysis_results_po_data.computation_time = _computation_time;
    Po_analysis_results_po_data.nb_paths = _nb_paths;
    Po_analysis_results_po_data.alive_branch = _alive_branch;
    Po_analysis_results_po_data.formula = _formula;
  }

and parse_po_analysis_results_po_status x =
  match Piqirun.int32_of_signed_varint x with
    | 1l -> `unknown
    | 2l -> `not_opaque
    | 3l -> `opaque
    | 4l -> `likely
    | x -> Piqirun.error_enum_const x
and packed_parse_po_analysis_results_po_status x =
  match Piqirun.int32_of_packed_signed_varint x with
    | 1l -> `unknown
    | 2l -> `not_opaque
    | 3l -> `opaque
    | 4l -> `likely
    | x -> Piqirun.error_enum_const x


let rec gen__uint64 code x = Piqirun.int64_to_varint code x
and packed_gen__uint64 x = Piqirun.int64_to_packed_varint x

and gen__bool code x = Piqirun.bool_to_varint code x
and packed_gen__bool x = Piqirun.bool_to_packed_varint x

and gen__string code x = Piqirun.string_to_block code x

and gen__uint32 code x = Piqirun.int32_to_varint code x
and packed_gen__uint32 x = Piqirun.int32_to_packed_varint x

and gen__float32 code x = Piqirun.float_to_fixed32 code x
and packed_gen__float32 x = Piqirun.float_to_packed_fixed32 x

and gen__specific_parameters_t code x =
  let _typeid = Piqirun.gen_required_field 1 gen__specific_parameters_t_analyse_type x.Specific_parameters_t.typeid in
  let _generic_params = Piqirun.gen_optional_field 2 gen__generic_analysis x.Specific_parameters_t.generic_params in
  let _standard_params = Piqirun.gen_optional_field 3 gen__standard_analysis x.Specific_parameters_t.standard_params in
  Piqirun.gen_record code (_typeid :: _generic_params :: _standard_params :: [])

and gen__specific_parameters_t_analyse_type code x =
  Piqirun.int32_to_signed_varint code (match x with
    | `none -> 0l
    | `generic -> 1l
    | `standard -> 2l
  )
and packed_gen__specific_parameters_t_analyse_type x =
  Piqirun.int32_to_packed_signed_varint (match x with
    | `none -> 0l
    | `generic -> 1l
    | `standard -> 2l
  )

and gen__standard_analysis code x =
  let _target_addr = Piqirun.gen_optional_field 1 gen__uint64 x.Standard_analysis.target_addr in
  let _uniq = Piqirun.gen_optional_field 2 gen__bool x.Standard_analysis.uniq in
  let _get_formula = Piqirun.gen_optional_field 3 gen__bool x.Standard_analysis.get_formula in
  Piqirun.gen_record code (_target_addr :: _uniq :: _get_formula :: [])

and gen__generic_analysis code x =
  let _kind = Piqirun.gen_required_field 1 gen__generic_analysis_query_type x.Generic_analysis.kind in
  let _target_addr = Piqirun.gen_required_field 2 gen__uint64 x.Generic_analysis.target_addr in
  let _dba = Piqirun.gen_required_field 3 gen__string x.Generic_analysis.dba in
  let _limit_values = Piqirun.gen_optional_field 4 gen__uint32 x.Generic_analysis.limit_values in
  let _get_formula = Piqirun.gen_optional_field 5 gen__bool x.Generic_analysis.get_formula in
  let _from_addr = Piqirun.gen_optional_field 6 gen__uint64 x.Generic_analysis.from_addr in
  let _to_addr = Piqirun.gen_optional_field 7 gen__uint64 x.Generic_analysis.to_addr in
  let _restrict_values_from = Piqirun.gen_optional_field 8 gen__uint64 x.Generic_analysis.restrict_values_from in
  let _restrict_values_to = Piqirun.gen_optional_field 9 gen__uint64 x.Generic_analysis.restrict_values_to in
  Piqirun.gen_record code (_kind :: _target_addr :: _dba :: _limit_values :: _get_formula :: _from_addr :: _to_addr :: _restrict_values_from :: _restrict_values_to :: [])

and gen__generic_analysis_query_type code x =
  Piqirun.int32_to_signed_varint code (match x with
    | `satisfiability -> 0l
    | `values -> 1l
  )
and packed_gen__generic_analysis_query_type x =
  Piqirun.int32_to_packed_signed_varint (match x with
    | `satisfiability -> 0l
    | `values -> 1l
  )

and gen__generic_analysis_results code x =
  let _result = Piqirun.gen_required_field 1 Common.gen__smt_result x.Generic_analysis_results.result in
  let _values = Piqirun.gen_repeated_field 2 gen__uint64 x.Generic_analysis_results.values in
  let _smt_formula = Piqirun.gen_optional_field 3 gen__string x.Generic_analysis_results.smt_formula in
  Piqirun.gen_record code (_result :: _values :: _smt_formula :: [])

and gen__callret_analysis_results code x =
  let _values = Piqirun.gen_repeated_field 1 gen__callret_analysis_results_ret_data x.Callret_analysis_results.values in
  Piqirun.gen_record code (_values :: [])

and gen__callret_analysis_results_call_data code x =
  let _addr = Piqirun.gen_required_field 1 gen__uint64 x.Callret_analysis_results_call_data.addr in
  let _status = Piqirun.gen_required_field 2 gen__callret_analysis_results_callret_status x.Callret_analysis_results_call_data.status in
  Piqirun.gen_record code (_addr :: _status :: [])

and gen__callret_analysis_results_ret_data code x =
  let _ret_addr = Piqirun.gen_required_field 1 gen__uint64 x.Callret_analysis_results_ret_data.ret_addr in
  let _status = Piqirun.gen_required_field 2 gen__callret_analysis_results_callret_status x.Callret_analysis_results_ret_data.status in
  let _labels = Piqirun.gen_repeated_field 3 gen__callret_analysis_results_callret_labels x.Callret_analysis_results_ret_data.labels in
  let _returnsites = Piqirun.gen_repeated_field 4 gen__uint64 x.Callret_analysis_results_ret_data.returnsites in
  let _solve_count = Piqirun.gen_required_field 5 gen__uint32 x.Callret_analysis_results_ret_data.solve_count in
  let _calls = Piqirun.gen_repeated_field 6 gen__callret_analysis_results_call_data x.Callret_analysis_results_ret_data.calls in
  Piqirun.gen_record code (_ret_addr :: _status :: _labels :: _returnsites :: _solve_count :: _calls :: [])

and gen__callret_analysis_results_callret_labels code x =
  Piqirun.int32_to_signed_varint code (match x with
    | `violable -> 1l
    | `aligned -> 2l
    | `disaligned -> 3l
    | `can_return -> 4l
    | `single -> 5l
    | `multiple -> 6l
    | `strong -> 7l
    | `weak -> 8l
    | `solver_wrong -> 9l
    | `no_call -> 10l
    | `has_returned -> 11l
  )
and packed_gen__callret_analysis_results_callret_labels x =
  Piqirun.int32_to_packed_signed_varint (match x with
    | `violable -> 1l
    | `aligned -> 2l
    | `disaligned -> 3l
    | `can_return -> 4l
    | `single -> 5l
    | `multiple -> 6l
    | `strong -> 7l
    | `weak -> 8l
    | `solver_wrong -> 9l
    | `no_call -> 10l
    | `has_returned -> 11l
  )

and gen__callret_analysis_results_callret_status code x =
  Piqirun.int32_to_signed_varint code (match x with
    | `ok -> 1l
    | `viol -> 2l
  )
and packed_gen__callret_analysis_results_callret_status x =
  Piqirun.int32_to_packed_signed_varint (match x with
    | `ok -> 1l
    | `viol -> 2l
  )

and gen__po_analysis_results code x =
  let _values = Piqirun.gen_repeated_field 1 gen__po_analysis_results_po_data x.Po_analysis_results.values in
  Piqirun.gen_record code (_values :: [])

and gen__po_analysis_results_po_data code x =
  let _jmp_addr = Piqirun.gen_required_field 1 gen__uint64 x.Po_analysis_results_po_data.jmp_addr in
  let _status = Piqirun.gen_required_field 2 gen__po_analysis_results_po_status x.Po_analysis_results_po_data.status in
  let _ksteps = Piqirun.gen_required_field 3 gen__uint32 x.Po_analysis_results_po_data.ksteps in
  let _computation_time = Piqirun.gen_required_field 4 gen__float32 x.Po_analysis_results_po_data.computation_time in
  let _nb_paths = Piqirun.gen_optional_field 5 gen__uint32 x.Po_analysis_results_po_data.nb_paths in
  let _alive_branch = Piqirun.gen_optional_field 6 gen__uint64 x.Po_analysis_results_po_data.alive_branch in
  let _formula = Piqirun.gen_optional_field 7 gen__string x.Po_analysis_results_po_data.formula in
  Piqirun.gen_record code (_jmp_addr :: _status :: _ksteps :: _computation_time :: _nb_paths :: _alive_branch :: _formula :: [])

and gen__po_analysis_results_po_status code x =
  Piqirun.int32_to_signed_varint code (match x with
    | `unknown -> 1l
    | `not_opaque -> 2l
    | `opaque -> 3l
    | `likely -> 4l
  )
and packed_gen__po_analysis_results_po_status x =
  Piqirun.int32_to_packed_signed_varint (match x with
    | `unknown -> 1l
    | `not_opaque -> 2l
    | `opaque -> 3l
    | `likely -> 4l
  )


let gen_uint64 x = gen__uint64 (-1) x
let gen_bool x = gen__bool (-1) x
let gen_string x = gen__string (-1) x
let gen_uint32 x = gen__uint32 (-1) x
let gen_float32 x = gen__float32 (-1) x
let gen_specific_parameters_t x = gen__specific_parameters_t (-1) x
let gen_specific_parameters_t_analyse_type x = gen__specific_parameters_t_analyse_type (-1) x
let gen_standard_analysis x = gen__standard_analysis (-1) x
let gen_generic_analysis x = gen__generic_analysis (-1) x
let gen_generic_analysis_query_type x = gen__generic_analysis_query_type (-1) x
let gen_generic_analysis_results x = gen__generic_analysis_results (-1) x
let gen_callret_analysis_results x = gen__callret_analysis_results (-1) x
let gen_callret_analysis_results_call_data x = gen__callret_analysis_results_call_data (-1) x
let gen_callret_analysis_results_ret_data x = gen__callret_analysis_results_ret_data (-1) x
let gen_callret_analysis_results_callret_labels x = gen__callret_analysis_results_callret_labels (-1) x
let gen_callret_analysis_results_callret_status x = gen__callret_analysis_results_callret_status (-1) x
let gen_po_analysis_results x = gen__po_analysis_results (-1) x
let gen_po_analysis_results_po_data x = gen__po_analysis_results_po_data (-1) x
let gen_po_analysis_results_po_status x = gen__po_analysis_results_po_status (-1) x


let rec default_uint64 () = 0L
and default_bool () = false
and default_string () = ""
and default_uint32 () = 0l
and default_float32 () = 0.0
and default_specific_parameters_t () =
  {
    Specific_parameters_t.typeid = default_specific_parameters_t_analyse_type ();
    Specific_parameters_t.generic_params = None;
    Specific_parameters_t.standard_params = None;
  }
and default_specific_parameters_t_analyse_type () = `none
and default_standard_analysis () =
  {
    Standard_analysis.target_addr = None;
    Standard_analysis.uniq = None;
    Standard_analysis.get_formula = None;
  }
and default_generic_analysis () =
  {
    Generic_analysis.kind = default_generic_analysis_query_type ();
    Generic_analysis.target_addr = default_uint64 ();
    Generic_analysis.dba = default_string ();
    Generic_analysis.limit_values = None;
    Generic_analysis.get_formula = None;
    Generic_analysis.from_addr = None;
    Generic_analysis.to_addr = None;
    Generic_analysis.restrict_values_from = None;
    Generic_analysis.restrict_values_to = None;
  }
and default_generic_analysis_query_type () = `satisfiability
and default_generic_analysis_results () =
  {
    Generic_analysis_results.result = Common.default_smt_result ();
    Generic_analysis_results.values = [];
    Generic_analysis_results.smt_formula = None;
  }
and default_callret_analysis_results () =
  {
    Callret_analysis_results.values = [];
  }
and default_callret_analysis_results_call_data () =
  {
    Callret_analysis_results_call_data.addr = default_uint64 ();
    Callret_analysis_results_call_data.status = default_callret_analysis_results_callret_status ();
  }
and default_callret_analysis_results_ret_data () =
  {
    Callret_analysis_results_ret_data.ret_addr = default_uint64 ();
    Callret_analysis_results_ret_data.status = default_callret_analysis_results_callret_status ();
    Callret_analysis_results_ret_data.labels = [];
    Callret_analysis_results_ret_data.returnsites = [];
    Callret_analysis_results_ret_data.solve_count = default_uint32 ();
    Callret_analysis_results_ret_data.calls = [];
  }
and default_callret_analysis_results_callret_labels () = `violable
and default_callret_analysis_results_callret_status () = `ok
and default_po_analysis_results () =
  {
    Po_analysis_results.values = [];
  }
and default_po_analysis_results_po_data () =
  {
    Po_analysis_results_po_data.jmp_addr = default_uint64 ();
    Po_analysis_results_po_data.status = default_po_analysis_results_po_status ();
    Po_analysis_results_po_data.ksteps = default_uint32 ();
    Po_analysis_results_po_data.computation_time = default_float32 ();
    Po_analysis_results_po_data.nb_paths = None;
    Po_analysis_results_po_data.alive_branch = None;
    Po_analysis_results_po_data.formula = None;
  }
and default_po_analysis_results_po_status () = `unknown


let piqi = "\226\202\2304\015analysis_config\226\231\249\238\001\025piqi/analysis_config.piqi\170\150\212\160\004\011\226\202\2304\006common\162\244\146\155\011\015analysis_config\218\244\134\182\012\194\002\138\233\142\251\014\187\002\210\203\242$a\232\146\150q\002\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\006typeid\208\215\133\174\005\000\210\171\158\194\006\"specific-parameters-t-analyse-type\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$W\232\146\150q\004\152\247\223\136\002\000\152\182\154\152\004\160\223\186\243\001\232\243\204\157\004\000\218\164\238\191\004\014generic-params\208\215\133\174\005\000\210\171\158\194\006\016generic-analysis\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$Y\232\146\150q\006\152\247\223\136\002\000\152\182\154\152\004\160\223\186\243\001\232\243\204\157\004\000\218\164\238\191\004\015standard-params\208\215\133\174\005\000\210\171\158\194\006\017standard-analysis\192\139\160\247\t\000\136\158\147\199\014\000\218\164\238\191\004\021specific-parameters-t\218\244\134\182\012\238\001\138\176\205\197\001\231\001\218\164\238\191\004\"specific-parameters-t-analyse-type\170\183\218\222\0055\232\146\150q\000\152\247\223\136\002\000\234\188\204\215\002\026specific_parameters_t_NONE\218\164\238\191\004\004NONE\170\183\218\222\005;\232\146\150q\002\152\247\223\136\002\000\234\188\204\215\002\029specific_parameters_t_GENERIC\218\164\238\191\004\007GENERIC\170\183\218\222\005=\232\146\150q\004\152\247\223\136\002\000\234\188\204\215\002\030specific_parameters_t_STANDARD\218\164\238\191\004\bSTANDARD\218\244\134\182\012\154\002\138\233\142\251\014\147\002\210\203\242$J\232\146\150q\002\152\247\223\136\002\000\152\182\154\152\004\160\223\186\243\001\232\243\204\157\004\000\218\164\238\191\004\011target-addr\208\215\133\174\005\000\210\171\158\194\006\006uint64\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$N\232\146\150q\004\152\247\223\136\002\000\170\131\252\172\003\007\218\148\211\024\002\b\001\152\182\154\152\004\160\223\186\243\001\232\243\204\157\004\000\218\164\238\191\004\004uniq\208\215\133\174\005\000\210\171\158\194\006\004bool\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$U\232\146\150q\006\152\247\223\136\002\000\170\131\252\172\003\007\218\148\211\024\002\b\001\152\182\154\152\004\160\223\186\243\001\232\243\204\157\004\000\218\164\238\191\004\011get-formula\208\215\133\174\005\000\210\171\158\194\006\004bool\192\139\160\247\t\000\136\158\147\199\014\000\218\164\238\191\004\017standard-analysis\218\244\134\182\012\128\006\138\233\142\251\014\249\005\210\203\242$X\232\146\150q\002\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\004kind\208\215\133\174\005\000\210\171\158\194\006\027generic-analysis-query-type\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$J\232\146\150q\004\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\011target-addr\208\215\133\174\005\000\210\171\158\194\006\006uint64\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$B\232\146\150q\006\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\003dba\208\215\133\174\005\000\210\171\158\194\006\006string\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$K\232\146\150q\b\152\247\223\136\002\000\152\182\154\152\004\160\223\186\243\001\232\243\204\157\004\000\218\164\238\191\004\012limit-values\208\215\133\174\005\000\210\171\158\194\006\006uint32\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$U\232\146\150q\n\152\247\223\136\002\000\170\131\252\172\003\007\218\148\211\024\002\b\001\152\182\154\152\004\160\223\186\243\001\232\243\204\157\004\000\218\164\238\191\004\011get-formula\208\215\133\174\005\000\210\171\158\194\006\004bool\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$H\232\146\150q\012\152\247\223\136\002\000\152\182\154\152\004\160\223\186\243\001\232\243\204\157\004\000\218\164\238\191\004\tfrom-addr\208\215\133\174\005\000\210\171\158\194\006\006uint64\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$F\232\146\150q\014\152\247\223\136\002\000\152\182\154\152\004\160\223\186\243\001\232\243\204\157\004\000\218\164\238\191\004\007to-addr\208\215\133\174\005\000\210\171\158\194\006\006uint64\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$S\232\146\150q\016\152\247\223\136\002\000\152\182\154\152\004\160\223\186\243\001\232\243\204\157\004\000\218\164\238\191\004\020restrict-values-from\208\215\133\174\005\000\210\171\158\194\006\006uint64\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$Q\232\146\150q\018\152\247\223\136\002\000\152\182\154\152\004\160\223\186\243\001\232\243\204\157\004\000\218\164\238\191\004\018restrict-values-to\208\215\133\174\005\000\210\171\158\194\006\006uint64\192\139\160\247\t\000\136\158\147\199\014\000\218\164\238\191\004\016generic-analysis\218\244\134\182\012\172\001\138\176\205\197\001\165\001\218\164\238\191\004\027generic-analysis-query-type\170\183\218\222\005D\232\146\150q\000\152\247\223\136\002\000\234\188\204\215\002\031generic_analysis_SATISFIABILITY\218\164\238\191\004\014SATISFIABILITY\170\183\218\222\0054\232\146\150q\002\152\247\223\136\002\000\234\188\204\215\002\023generic_analysis_VALUES\218\164\238\191\004\006VALUES\218\244\134\182\012\147\002\138\233\142\251\014\140\002\210\203\242$P\232\146\150q\002\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\006result\208\215\133\174\005\000\210\171\158\194\006\017common/smt-result\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$E\232\146\150q\004\152\247\223\136\002\000\152\182\154\152\004\250\248\214\130\001\232\243\204\157\004\000\218\164\238\191\004\006values\208\215\133\174\005\000\210\171\158\194\006\006uint64\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$J\232\146\150q\006\152\247\223\136\002\000\152\182\154\152\004\160\223\186\243\001\232\243\204\157\004\000\218\164\238\191\004\011smt-formula\208\215\133\174\005\000\210\171\158\194\006\006string\192\139\160\247\t\000\136\158\147\199\014\000\218\164\238\191\004\024generic-analysis-results\218\244\134\182\012\138\001\138\233\142\251\014\131\001\210\203\242$`\232\146\150q\002\152\247\223\136\002\000\152\182\154\152\004\250\248\214\130\001\232\243\204\157\004\000\218\164\238\191\004\006values\208\215\133\174\005\000\210\171\158\194\006!callret-analysis-results-ret-data\192\139\160\247\t\000\136\158\147\199\014\000\218\164\238\191\004\024callret-analysis-results\218\244\134\182\012\226\001\138\233\142\251\014\219\001\210\203\242$C\232\146\150q\002\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\004addr\208\215\133\174\005\000\210\171\158\194\006\006uint64\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$f\232\146\150q\004\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\006status\208\215\133\174\005\000\210\171\158\194\006'callret-analysis-results-callret-status\192\139\160\247\t\000\136\158\147\199\014\000\218\164\238\191\004\"callret-analysis-results-call-data\218\244\134\182\012\211\004\138\233\142\251\014\204\004\210\203\242$G\232\146\150q\002\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\bret-addr\208\215\133\174\005\000\210\171\158\194\006\006uint64\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$f\232\146\150q\004\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\006status\208\215\133\174\005\000\210\171\158\194\006'callret-analysis-results-callret-status\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$f\232\146\150q\006\152\247\223\136\002\000\152\182\154\152\004\250\248\214\130\001\232\243\204\157\004\000\218\164\238\191\004\006labels\208\215\133\174\005\000\210\171\158\194\006'callret-analysis-results-callret-labels\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$J\232\146\150q\b\152\247\223\136\002\000\152\182\154\152\004\250\248\214\130\001\232\243\204\157\004\000\218\164\238\191\004\011returnsites\208\215\133\174\005\000\210\171\158\194\006\006uint64\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$J\232\146\150q\n\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\011solve-count\208\215\133\174\005\000\210\171\158\194\006\006uint32\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$`\232\146\150q\012\152\247\223\136\002\000\152\182\154\152\004\250\248\214\130\001\232\243\204\157\004\000\218\164\238\191\004\005calls\208\215\133\174\005\000\210\171\158\194\006\"callret-analysis-results-call-data\192\139\160\247\t\000\136\158\147\199\014\000\218\164\238\191\004!callret-analysis-results-ret-data\218\244\134\182\012\186\006\138\176\205\197\001\179\006\218\164\238\191\004'callret-analysis-results-callret-labels\170\183\218\222\005@\232\146\150q\002\152\247\223\136\002\000\234\188\204\215\002!callret_analysis_results_VIOLABLE\218\164\238\191\004\bVIOLABLE\170\183\218\222\005>\232\146\150q\004\152\247\223\136\002\000\234\188\204\215\002 callret_analysis_results_ALIGNED\218\164\238\191\004\007ALIGNED\170\183\218\222\005D\232\146\150q\006\152\247\223\136\002\000\234\188\204\215\002#callret_analysis_results_DISALIGNED\218\164\238\191\004\nDISALIGNED\170\183\218\222\005D\232\146\150q\b\152\247\223\136\002\000\234\188\204\215\002#callret_analysis_results_CAN_RETURN\218\164\238\191\004\nCAN-RETURN\170\183\218\222\005<\232\146\150q\n\152\247\223\136\002\000\234\188\204\215\002\031callret_analysis_results_SINGLE\218\164\238\191\004\006SINGLE\170\183\218\222\005@\232\146\150q\012\152\247\223\136\002\000\234\188\204\215\002!callret_analysis_results_MULTIPLE\218\164\238\191\004\bMULTIPLE\170\183\218\222\005<\232\146\150q\014\152\247\223\136\002\000\234\188\204\215\002\031callret_analysis_results_STRONG\218\164\238\191\004\006STRONG\170\183\218\222\0058\232\146\150q\016\152\247\223\136\002\000\234\188\204\215\002\029callret_analysis_results_WEAK\218\164\238\191\004\004WEAK\170\183\218\222\005H\232\146\150q\018\152\247\223\136\002\000\234\188\204\215\002%callret_analysis_results_SOLVER_WRONG\218\164\238\191\004\012SOLVER-WRONG\170\183\218\222\005>\232\146\150q\020\152\247\223\136\002\000\234\188\204\215\002 callret_analysis_results_NO_CALL\218\164\238\191\004\007NO-CALL\170\183\218\222\005H\232\146\150q\022\152\247\223\136\002\000\234\188\204\215\002%callret_analysis_results_HAS_RETURNED\218\164\238\191\004\012HAS-RETURNED\218\244\134\182\012\172\001\138\176\205\197\001\165\001\218\164\238\191\004'callret-analysis-results-callret-status\170\183\218\222\0054\232\146\150q\002\152\247\223\136\002\000\234\188\204\215\002\027callret_analysis_results_OK\218\164\238\191\004\002OK\170\183\218\222\0058\232\146\150q\004\152\247\223\136\002\000\234\188\204\215\002\029callret_analysis_results_VIOL\218\164\238\191\004\004VIOL\218\244\134\182\012~\138\233\142\251\014x\210\203\242$Z\232\146\150q\002\152\247\223\136\002\000\152\182\154\152\004\250\248\214\130\001\232\243\204\157\004\000\218\164\238\191\004\006values\208\215\133\174\005\000\210\171\158\194\006\027po-analysis-results-po-data\192\139\160\247\t\000\136\158\147\199\014\000\218\164\238\191\004\019po-analysis-results\218\244\134\182\012\219\004\138\233\142\251\014\212\004\210\203\242$G\232\146\150q\002\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\bjmp-addr\208\215\133\174\005\000\210\171\158\194\006\006uint64\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$\\\232\146\150q\004\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\006status\208\215\133\174\005\000\210\171\158\194\006\029po-analysis-results-po-status\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$E\232\146\150q\006\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\006ksteps\208\215\133\174\005\000\210\171\158\194\006\006uint32\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$P\232\146\150q\b\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\016computation-time\208\215\133\174\005\000\210\171\158\194\006\007float32\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$G\232\146\150q\n\152\247\223\136\002\000\152\182\154\152\004\160\223\186\243\001\232\243\204\157\004\000\218\164\238\191\004\bnb-paths\208\215\133\174\005\000\210\171\158\194\006\006uint32\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$K\232\146\150q\012\152\247\223\136\002\000\152\182\154\152\004\160\223\186\243\001\232\243\204\157\004\000\218\164\238\191\004\012alive-branch\208\215\133\174\005\000\210\171\158\194\006\006uint64\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$F\232\146\150q\014\152\247\223\136\002\000\152\182\154\152\004\160\223\186\243\001\232\243\204\157\004\000\218\164\238\191\004\007formula\208\215\133\174\005\000\210\171\158\194\006\006string\192\139\160\247\t\000\136\158\147\199\014\000\218\164\238\191\004\027po-analysis-results-po-data\218\244\134\182\012\168\002\138\176\205\197\001\161\002\218\164\238\191\004\029po-analysis-results-po-status\170\183\218\222\0059\232\146\150q\002\152\247\223\136\002\000\234\188\204\215\002\027po_analysis_results_UNKNOWN\218\164\238\191\004\007UNKNOWN\170\183\218\222\005?\232\146\150q\004\152\247\223\136\002\000\234\188\204\215\002\030po_analysis_results_NOT_OPAQUE\218\164\238\191\004\nNOT-OPAQUE\170\183\218\222\0057\232\146\150q\006\152\247\223\136\002\000\234\188\204\215\002\026po_analysis_results_OPAQUE\218\164\238\191\004\006OPAQUE\170\183\218\222\0057\232\146\150q\b\152\247\223\136\002\000\234\188\204\215\002\026po_analysis_results_LIKELY\218\164\238\191\004\006LIKELY"
include Analysis_config_piqi
